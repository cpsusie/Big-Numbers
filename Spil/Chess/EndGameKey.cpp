#include "stdafx.h"
#include "EndGameKey.h"

EndGameKey::EndGameKey(const EndGameKeyDefinition &keydef, Player playerInTurn, ...) {
  DEFINEMETHODNAME;
  m_hashCode = 0;
  setPlayerInTurn(playerInTurn);
  va_list argptr;
  va_start(argptr, playerInTurn);
  try {
    FieldSet used;
    for(int i = 0; i < keydef.getPieceCount(); i++) {
      const UINT pos = va_arg(argptr, UINT);
      validatePosition(method, pos);
      if(used.contains(pos)) {
        throwInvalidArgumentException(method,_T("Position %s already used"), getFieldName(pos));
      }
      used.add(pos);
      setPosition(i, pos);
    }
  } catch(...) {
    va_end(argptr);
    throw;
  }
  va_end(argptr);
}

GameKey EndGameKey::getGameKey(const EndGameKeyDefinition &keydef) const {
  GameKey   result;
  const int pieceCount = keydef.getPieceCount();
  result.clear(getPlayerInTurn());
  for(int i = 0; i < pieceCount; i++) {
    result.m_pieceKey[getPosition(i)] = keydef.getPieceKey(i);
  }
  return result;
}

#define ADJUSTPOS_CHECK1POS(pIndex, f, n, i)    \
{ p += (f)*(n);                                 \
  if(p >= getPosition##i()) {                   \
    p += f;                                     \
  }                                             \
  setPosition##pIndex(p);                       \
}

#define ADJUSTPOS_CHECK2POS(pIndex, f, n, i, j) \
{ p += (f)*(n);                                 \
  if(p >= getPosition##i()) {                   \
    p += f;                                     \
    if(p >= getPosition##j()) {                 \
      p += f;                                   \
    }                                           \
  } else if(p >= getPosition##j()) {            \
    p += f;                                     \
    if(p >= getPosition##i()) {                 \
      p += f;                                   \
    }                                           \
  }                                             \
  setPosition##pIndex(p);                       \
}

#define ADJUSTPOS_CHECK3POS(pIndex, f, i, j, k) \
{ p += f;                                       \
  if(p == getPosition##i()) {                   \
    p += f;                                     \
    if(p == getPosition##j()) {                 \
      p += f;                                   \
      if(p == getPosition##k()) {               \
        p += f;                                 \
      }                                         \
    } else if(p == getPosition##k()) {          \
      p += f;                                   \
      if(p == getPosition##j()) {               \
        p += f;                                 \
      }                                         \
    }                                           \
  } else if(p == getPosition##j()) {            \
    p += f;                                     \
    if(p == getPosition##i()) {                 \
      p += f;                                   \
      if(p == getPosition##k()) {               \
        p += f;                                 \
      }                                         \
    } if(p == getPosition##k()) {               \
      p += f;                                   \
      if(p == getPosition##i()) {               \
        p += f;                                 \
      }                                         \
    }                                           \
  } else if(p == getPosition##k()) {            \
    p += f;                                     \
    if(p == getPosition##i()) {                 \
      p += f;                                   \
      if(p == getPosition##j()) {               \
        p += f;                                 \
      }                                         \
    } if(p == getPosition##j()) {               \
      p += f;                                   \
      if(p == getPosition##i()) {               \
        p += f;                                 \
      }                                         \
    }                                           \
  }                                             \
  setPosition##pIndex(p);                       \
}

#define ADJUSTP2_OFFDIAG_CHECK1POS(           i      )  ADJUSTPOS_CHECK1POS(2, 1, 1, i   )
#define ADJUSTP3_OFFDIAG_CHECK1POS(           i      )  ADJUSTPOS_CHECK1POS(3, 1, 2, i   )
#define ADJUSTP3_OFFDIAG_CHECK2POS(           i, j   )  ADJUSTPOS_CHECK2POS(3, 1, 1, i, j)
#define ADJUSTP4_OFFDIAG_CHECK1POS(           i      )  ADJUSTPOS_CHECK1POS(4, 1, 3, i   )
#define ADJUSTP4_OFFDIAG_CHECK2POS(           i, j   )  ADJUSTPOS_CHECK2POS(4, 1, 2, i, j)
#define ADJUSTP4_OFFDIAG_CHECK3POS(           i, j, k)  ADJUSTPOS_CHECK3POS(4, 1, i, j, k)

#define ADJUSTP2_ONDIAG_CHECK1POS(            i      )  ADJUSTPOS_CHECK1POS(2, 9, 1, i   )
#define ADJUSTP3_ONDIAG_CHECK1POS(            i      )  ADJUSTPOS_CHECK1POS(3, 9, 2, i   )
#define ADJUSTP3_ONDIAG_CHECK2POS(            i, j   )  ADJUSTPOS_CHECK2POS(3, 9, 1, i, j)
#define ADJUSTP4_ONDIAG_CHECK1POS(            i      )  ADJUSTPOS_CHECK1POS(4, 9, 3, i   )
#define ADJUSTP4_ONDIAG_CHECK2POS(            i, j   )  ADJUSTPOS_CHECK2POS(4, 9, 2, i, j)
#define ADJUSTP4_ONDIAG_CHECK3POS(            i, j, k)  ADJUSTPOS_CHECK3POS(4, 9, i, j, k)

#define ADJUSTP4_PAWN_CHECK1POS(              i      )  ADJUSTPOS_CHECK1POS(4, 1, 1, i   )

#define ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(  i      )  ADJUSTPOS_CHECK1POS(3, 1, 1, i   )
#define ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(  i      )  ADJUSTPOS_CHECK1POS(4, 1, 2, i   )
#define ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(  i, j   )  ADJUSTPOS_CHECK2POS(4, 1, 1, i, j)
#define ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234( i      )  ADJUSTPOS_CHECK1POS(4, 1, 1, i   )

#define ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(   i      )  ADJUSTPOS_CHECK1POS(3, 9, 1, i   )
#define ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(   i      )  ADJUSTPOS_CHECK1POS(4, 9, 2, i   )
#define ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(   i, j   )  ADJUSTPOS_CHECK2POS(4, 9, 1, i, j)
#define ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(  i      )  ADJUSTPOS_CHECK1POS(4, 9, 1, i   )

#define ADJUSTP2_PAWN_CHECK1POS(              i      )  ADJUSTPOS_CHECK1POS(2, 1, 3, i   )
#define ADJUSTP2_PAWN_CHECK2POS(              i, j   )  ADJUSTPOS_CHECK2POS(2, 1, 2, i, j)
#define ADJUSTP2_PAWN_CHECK3POS(              i, j, k)  ADJUSTPOS_CHECK3POS(2, 1, i, j, k)

UINT EndGameKey::getP2OffDiagIndex() const {         // Kings off maindiag
  const UINT p   = getPosition2();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  return p - dec;
}

UINT EndGameKey::getP3OffDiagIndex() const {         // Kings, p2 off maindiag
  const UINT p   = getPosition3();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  if(p > getPosition2()) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndex() const {         // Kings, p2 or p3 off maindiag
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  if(p > getPosition2()) dec++;
  if(p > getPosition3()) dec++;
  return p - dec;
}

UINT EndGameKey::getP3OffDiagIndexEqualP23() const { // Kings or p2 off maindiag, ignore p2
  const UINT p   = getPosition3();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndexEqualP34() const { // Kings, p2 or p3 off maindiag, ignore p3
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  if(p > getPosition2()) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndexEqualP234() const { // Kings, p2 or p3 off maindiag, ignore p2,p3
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 1 : 0;
  if(p > getPosition1()) dec++;
  return p - dec;
}

UINT EndGameKey::getP2DiagIndex() const {            // Kings, p2 on maindiag
  const UINT p   = getPosition2();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP3DiagIndex() const {            // Kings, p2, p3 on maindiag
  const UINT p   = getPosition3();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  if(p > getPosition2()) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndex() const {            // Kings, p2, p3, p4 on maindiag
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  if(p > getPosition2()) dec += 9;
  if(p > getPosition3()) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP3DiagIndexEqualP23() const {    // Kings, p2, p3 on maindiag, ignore p2
  const UINT p   = getPosition3();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndexEqualP34() const {    // Kings, p2, p3, p4 on maindiag, ignore p3
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  if(p > getPosition2()) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndexEqualP234() const {   // Kings, p2, p3, p4 on maindiag, ignore p2,p3
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition0()) ? 9 : 0;
  if(p > getPosition1()) dec += 9;
  return (p - dec) / 9;
}

void EndGameKey::p2IndexToOffDiagPos() {
  UINT p = getPosition2();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1
  case 1: ADJUSTP2_OFFDIAG_CHECK1POS(  1);            return; // p0          < p <    p1
  case 2: ADJUSTP2_OFFDIAG_CHECK1POS(0  );            return; //    p1       < p < p0
  case 3: setPosition2(p + 2);                        return; // p0,p1       < p
  }
}

void EndGameKey::p3IndexToOffDiagPos() {
  UINT p = getPosition3();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1,p2
  case 1: ADJUSTP3_OFFDIAG_CHECK2POS(  1,2);          return; // p0          < p <    p1,p2
  case 2: ADJUSTP3_OFFDIAG_CHECK2POS(0,  2);          return; //    p1       < p < p0,   p2
  case 3: ADJUSTP3_OFFDIAG_CHECK1POS(    2);          return; // p0,p1       < p <       p2
  case 4: ADJUSTP3_OFFDIAG_CHECK2POS(0,1  );          return; //       p2    < p < p0,p1
  case 5: ADJUSTP3_OFFDIAG_CHECK1POS(  1  );          return; // p0,   p2    < p <    p1
  case 6: ADJUSTP3_OFFDIAG_CHECK1POS(0    );          return; //    p1,p2    < p < p0
  case 7: setPosition3(p + 3);                        return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToOffDiagPos() {
  UINT p = getPosition4();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  if(p >= getPosition3()) ge |= 8;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1,p2,p3
  case 1 : ADJUSTP4_OFFDIAG_CHECK3POS(  1,2,3);       return; // p0          < p <    p1,p2,p3
  case 2 : ADJUSTP4_OFFDIAG_CHECK3POS(0,  2,3);       return; //    p1       < p < p0,   p2,p3
  case 3 : ADJUSTP4_OFFDIAG_CHECK2POS(    2,3);       return; // p0,p1       < p <       p2,p3
  case 4 : ADJUSTP4_OFFDIAG_CHECK3POS(0,1,  3);       return; //       p2    < p < p0,p1,   p3
  case 5 : ADJUSTP4_OFFDIAG_CHECK2POS(  1,  3);       return; // p0,   p2    < p <    p1,   p3
  case 6 : ADJUSTP4_OFFDIAG_CHECK2POS(0,    3);       return; //    p1,p2    < p < p0,      p3
  case 7 : ADJUSTP4_OFFDIAG_CHECK1POS(      3);       return; // p0,p1,p2    < p <          p3
  case 8 : ADJUSTP4_OFFDIAG_CHECK3POS(0,1,2  );       return; //          p3 < p < p0,p1,p2
  case 9 : ADJUSTP4_OFFDIAG_CHECK2POS(  1,2  );       return; // p0,      p3 < p <    p1,p2
  case 10: ADJUSTP4_OFFDIAG_CHECK2POS(0,  2  );       return; //    p1,   p3 < p < p0,   p2
  case 11: ADJUSTP4_OFFDIAG_CHECK1POS(    2  );       return; // p0,p1,   p3 < p <       p2
  case 12: ADJUSTP4_OFFDIAG_CHECK2POS(0,1    );       return; //       p2,p3 < p < p0,p1
  case 13: ADJUSTP4_OFFDIAG_CHECK1POS(  1    );       return; // p0,   p2,p3 < p <    p1
  case 14: ADJUSTP4_OFFDIAG_CHECK1POS(0      );       return; //    p1,p2,p3 < p < p0
  case 15: setPosition4(p + 4);                       return; // p0,p1,p2,p3 < p
  }
}

void EndGameKey::p3IndexToOffDiagPosEqualP23() {
  UINT p = getPosition3();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1
  case 1: ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(  1);   return; // p0          < p <    p1
  case 2: ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(0  );   return; //    p1       < p < p0
  case 3: setPosition3(p + 2);                        return; // p0,p1       < p
  }
}

void EndGameKey::p4IndexToOffDiagPosEqualP34() {
  UINT p = getPosition4();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1,p2
  case 1: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(  1,2); return; // p0          < p <    p1,p2
  case 2: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(0,  2); return; //    p1       < p < p0,   p2
  case 3: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(    2); return; // p0,p1       < p <       p2
  case 4: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(0,1  ); return; //       p2    < p < p0,p1
  case 5: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(  1  ); return; // p0,   p2    < p <    p1
  case 6: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(0    ); return; //    p1,p2    < p < p0
  case 7: setPosition4(p + 3);                        return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToOffDiagPosEqualP234() {
  UINT p = getPosition4();
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0:                                             return; //               p < p0,p1
  case 1: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234(  1);  return; // p0          < p <    p1
  case 2: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234(0  );  return; //    p1       < p < p0
  case 3: setPosition4(p + 2);                        return; // p0,p1       < p
  }
}

void EndGameKey::p23IndexToOffDiagPos() {
  p2IndexToOffDiagPos();
  p3IndexToOffDiagPos();
}

void EndGameKey::p234IndexToOffDiagPos() {
  p23IndexToOffDiagPos();
  p4IndexToOffDiagPos();
}

void EndGameKey::p34IndexToOffDiagPos() {
  p3IndexToOffDiagPos();
  p4IndexToOffDiagPos();
}

void EndGameKey::p23IndexToOffDiagPosEqualP23() {
  p2IndexToOffDiagPos();
  p3IndexToOffDiagPosEqualP23();
}

void EndGameKey::p34IndexToOffDiagPosEqualP34() {
  p3IndexToOffDiagPos();
  p4IndexToOffDiagPosEqualP34();
}

void EndGameKey::p234IndexToOffDiagPosEqualP234() {
  p2IndexToOffDiagPos();
  p3IndexToOffDiagPosEqualP23();
  p4IndexToOffDiagPosEqualP234();
}

void EndGameKey::p2IndexToDiagPos() {
  UINT p = getPosition2() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0 : setPosition2(p);                           return; //               p < p0,p1
  case 1 : ADJUSTP2_ONDIAG_CHECK1POS(  1);            return; // p0          < p <    p1
  case 2 : ADJUSTP2_ONDIAG_CHECK1POS(0  );            return; //    p1       < p < p0
  case 3 : setPosition2(p + 2*9);                     return; // p0,p1       < p
  }
}

void EndGameKey::p3IndexToDiagPos() {
  UINT p = getPosition3() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  switch(ge) {
  case 0 : setPosition3(p);                           return; //               p < p0,p1,p2
  case 1 : ADJUSTP3_ONDIAG_CHECK2POS(  1,2);          return; // p0          < p <    p1,p2
  case 2 : ADJUSTP3_ONDIAG_CHECK2POS(0,  2);          return; //    p1       < p < p0,   p2
  case 3 : ADJUSTP3_ONDIAG_CHECK1POS(    2);          return; // p0,p1       < p <       p2
  case 4 : ADJUSTP3_ONDIAG_CHECK2POS(0,1  );          return; //       p2    < p < p0,p1
  case 5 : ADJUSTP3_ONDIAG_CHECK1POS(  1  );          return; // p0,   p2    < p <    p1
  case 6 : ADJUSTP3_ONDIAG_CHECK1POS(0    );          return; //    p1,p2    < p < p0
  case 7 : setPosition3(p + 3*9);                     return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToDiagPos() {
  UINT p = getPosition4() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  if(p >= getPosition3()) ge |= 8;
  switch(ge) {
  case 0:  setPosition4(p);                           return; //               p < p0,p1,p2,p3
  case 1 : ADJUSTP4_ONDIAG_CHECK3POS(  1,2,3);        return; // p0          < p <    p1,p2,p3
  case 2 : ADJUSTP4_ONDIAG_CHECK3POS(0,  2,3);        return; //    p1       < p < p0,   p2,p3
  case 3 : ADJUSTP4_ONDIAG_CHECK2POS(    2,3);        return; // p0,p1       < p <       p2,p3
  case 4 : ADJUSTP4_ONDIAG_CHECK3POS(0,1,  3);        return; //       p2    < p < p0,p1,   p3
  case 5 : ADJUSTP4_ONDIAG_CHECK2POS(  1,  3);        return; // p0,   p2    < p <    p1,   p3
  case 6 : ADJUSTP4_ONDIAG_CHECK2POS(0,    3);        return; //    p1,p2    < p < p0,      p3
  case 7 : ADJUSTP4_ONDIAG_CHECK1POS(      3);        return; // p0,p1,p2    < p <          p3
  case 8 : ADJUSTP4_ONDIAG_CHECK3POS(0,1,2  );        return; //          p3 < p < p0,p1,p2
  case 9 : ADJUSTP4_ONDIAG_CHECK2POS(  1,2  );        return; // p0,      p3 < p <    p1,p2
  case 10: ADJUSTP4_ONDIAG_CHECK2POS(0,  2  );        return; //    p1,   p3 < p < p0,   p2
  case 11: ADJUSTP4_ONDIAG_CHECK1POS(    2  );        return; // p0,p1,   p3 < p <       p2
  case 12: ADJUSTP4_ONDIAG_CHECK2POS(0,1    );        return; //       p2,p3 < p < p0,p1
  case 13: ADJUSTP4_ONDIAG_CHECK1POS(  1    );        return; // p0,   p2,p3 < p <    p1
  case 14: ADJUSTP4_ONDIAG_CHECK1POS(0      );        return; //    p1,p2,p3 < p <    p1
  case 15: setPosition4(p + 4*9);                     return; // p0,p1,p2,p3 < p
  }
}

void EndGameKey::p3IndexToDiagPosEqualP23() {
  UINT p = getPosition3() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0 : setPosition3(p);                           return; //               p < p0,p1
  case 1 : ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(1);     return; // p0          < p <    p1
  case 2 : ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(0);     return; //    p1       < p < p0
  case 3 : setPosition3(p + 2*9);                     return; // p0,p1       < p
  }
}

void EndGameKey::p4IndexToDiagPosEqualP34() {
  UINT p = getPosition4() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  if(p >= getPosition2()) ge |= 4;
  switch(ge) {
  case 0 : setPosition4(p);                           return; //               p < p0,p1,p2
  case 1 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(  1,2); return; // p0          < p <    p1,p2
  case 2 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(0,  2); return; //    p1       < p < p0,   p2
  case 3 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(    2); return; // p0,p1       < p <       p2
  case 4 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(0,1  ); return; //       p2    < p < p0,p1
  case 5 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(  1  ); return; // p0,   p2    < p <    p1
  case 6 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(0    ); return; //    p1,p2    < p < p0
  case 7 : setPosition4(p + 3*9);                     return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToDiagPosEqualP234() {
  UINT p = getPosition4() * 9;
  int ge = 0;
  if(p >= getPosition0()) ge |= 1;
  if(p >= getPosition1()) ge |= 2;
  switch(ge) {
  case 0 : setPosition4(p);                           return; //               p < p0,p1
  case 1 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(1);    return; // p0          < p <    p1
  case 2 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(0);    return; //    p1       < p < p0
  case 3 : setPosition4(p + 2*9);                     return; // p0,p1       < p
  }
}

void EndGameKey::p23IndexToDiagPos() {
  p2IndexToDiagPos();
  p3IndexToDiagPos();
}

void EndGameKey::p234IndexToDiagPos() {
  p23IndexToDiagPos();
  p4IndexToDiagPos();
}

void EndGameKey::p23IndexToDiagPosEqualP23() {
  p2IndexToDiagPos();
  p3IndexToDiagPosEqualP23();
}

void EndGameKey::p34IndexToDiagPosEqualP34() {
  p3IndexToDiagPos();
  p4IndexToDiagPosEqualP34();
}

void EndGameKey::p34IndexToDiagPosEqualP234() {
  p3IndexToDiagPosEqualP23();
  p4IndexToDiagPosEqualP234();
}

void EndGameKey::p234IndexToDiagPosEqualP234() {
  p2IndexToDiagPos();
  p3IndexToDiagPosEqualP23();
  p4IndexToDiagPosEqualP234();
}

UINT EndGameKey::getP3Pawn2Index() const { // higest position is G7
  const UINT p   = getPosition3();
  UINT       dec = (p > getPosition2()) ? 1 : 0;
  return p - dec;
}

UINT EndGameKey::getP4Pawn3Index() const { // higest position is G7
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition2()) ? 1 : 0;
  if(p > getPosition3()) dec++;
  return p - dec;
}

void EndGameKey::p3IndexToPawn2Pos() {
  const UINT p = getPosition3();
  if(p >= getPosition2()) {
    setPosition3(p+1);
  }
}

void EndGameKey::p4IndexToPawn3PosEqualP34() {
  const UINT p = getPosition4();
  if(p >= getPosition2()) {
    setPosition4(p+1);
  }
}

void EndGameKey::p34IndexToPawn23PosEqualP34() {
  m_state.m_piece3Pos += 8;
  m_state.m_piece4Pos += 8;
  p3IndexToPawn2Pos();
  p4IndexToPawn3PosEqualP34();
}

void EndGameKey::p4IndexToPawn3Pos() {
  UINT p = getPosition4();
  int ge = 0;
  if(p >= getPosition2()) ge |= 1;
  if(p >= getPosition3()) ge |= 2;
  switch(ge) {
  case 0:                                             return; //               p < p2,p3
  case 1: ADJUSTP4_PAWN_CHECK1POS(3);                 return; // p2          < p <    p3
  case 2: ADJUSTP4_PAWN_CHECK1POS(2);                 return; //    p3       < p < p2
  case 3: setPosition4(p + 2);                        return; // p2,p3       < p
  }
}

void EndGameKey::p34IndexToPawn23Pos() {
  p3IndexToPawn2Pos();
  p4IndexToPawn3Pos();
}

UINT EndGameKey::getP4Pawn3IndexEqualP34() const {
  const UINT p   = getPosition4();
  UINT       dec = (p > getPosition2()) ? 1 : 0;
  return p - dec;
}

String EndGameKey::toString(const EndGameKeyDefinition &keydef, bool initFormat) const {
  TCHAR result[200];
  return keydef.createKeyString(result, *this, initFormat);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyWithOccupiedPositions::setPosition(UINT pIndex, UINT pos) {
  EndGameKey::setPosition(pIndex, pos);
  m_occupiedPositions.add(pos);
}

void EndGameKeyWithOccupiedPositions::setPosition0(UINT pos) {
  EndGameKey::setPosition0(pos);
  m_occupiedPositions.add(pos);
}

void EndGameKeyWithOccupiedPositions::setPosition1(UINT pos) {
  EndGameKey::setPosition1(pos);
  m_occupiedPositions.add(pos);
}

void EndGameKeyWithOccupiedPositions::setPosition2(UINT pos) {
  EndGameKey::setPosition2(pos);
  m_occupiedPositions.add(pos);
}

void EndGameKeyWithOccupiedPositions::setPosition3(UINT pos) {
  EndGameKey::setPosition3(pos);
  m_occupiedPositions.add(pos);
}

void EndGameKeyWithOccupiedPositions::setPosition4(UINT pos) {
  EndGameKey::setPosition4(pos);
  m_occupiedPositions.add(pos);
}

#endif
