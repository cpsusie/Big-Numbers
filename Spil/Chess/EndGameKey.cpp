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

#define IFBLOCK(f, i, stmt)                               \
if(p >= getPosition(i)) {                                 \
  p += f;                                                 \
  stmt;                                                   \
}

#define EMPTYSTMT

#define IFBLOCK1(f,i) IFBLOCK(f, i,EMPTYSTMT)

#define IFELSEBLOCK2(f,i1,i2)                             \
{ IFBLOCK(f,i1, IFBLOCK1(f,i2))                           \
  else                                                    \
  IFBLOCK(f,i2, IFBLOCK1(f,i1))                           \
}

#define IFELSEBLOCK3(f,i1,i2,i3)                          \
{ IFBLOCK(f,i1, IFELSEBLOCK2(f,i2,i3))                    \
  else                                                    \
  IFBLOCK(f,i2, IFELSEBLOCK2(f,i1,i3))                    \
  else                                                    \
  IFBLOCK(f,i3, IFELSEBLOCK2(f,i1,i2))                    \
}

#define IFELSEBLOCK4(f,i1,i2,i3,i4)                       \
{ IFBLOCK(f,i1, IFELSEBLOCK3(f,i2,i3,i4))                 \
  else                                                    \
  IFBLOCK(f,i2, IFELSEBLOCK3(f,i1,i3,i4))                 \
  else                                                    \
  IFBLOCK(f,i3, IFELSEBLOCK3(f,i1,i2,i4))                 \
  else                                                    \
  IFBLOCK(f,i4, IFELSEBLOCK3(f,i1,i2,i3))                 \
}

#define ADJUSTPOS_CHECK1POS(pIndex, f, n, i1)             \
{ p += (f)*(n);                                           \
  IFBLOCK1(f,i1)                                          \
  setPosition(pIndex, p);                                 \
}

#define ADJUSTPOS_CHECK2POS(pIndex, f, n, i1, i2)         \
{ p += (f)*(n);                                           \
  IFELSEBLOCK2(f,i1,i2)                                   \
  setPosition(pIndex, p);                                 \
}

#define ADJUSTPOS_CHECK3POS(pIndex, f, n, i1, i2, i3)     \
{ p += (f)*(n);                                           \
  IFELSEBLOCK3(f,i1,i2,i3)                                \
  setPosition(pIndex, p);                                 \
}

#define ADJUSTPOS_CHECK4POS(pIndex, f, n, i1, i2, i3, i4) \
{ p += (f)*(n);                                           \
  IFELSEBLOCK4(f,i1,i2,i3,i4)                             \
  setPosition(pIndex, p);                                 \
}

#define ADJUSTP2_OFFDIAG_CHECK1POS(           i         )  ADJUSTPOS_CHECK1POS(2, 1, 1, i         )
#define ADJUSTP3_OFFDIAG_CHECK1POS(           i         )  ADJUSTPOS_CHECK1POS(3, 1, 2, i         )
#define ADJUSTP3_OFFDIAG_CHECK2POS(           i, j      )  ADJUSTPOS_CHECK2POS(3, 1, 1, i, j      )
#define ADJUSTP4_OFFDIAG_CHECK1POS(           i         )  ADJUSTPOS_CHECK1POS(4, 1, 3, i         )
#define ADJUSTP4_OFFDIAG_CHECK2POS(           i, j      )  ADJUSTPOS_CHECK2POS(4, 1, 2, i, j      )
#define ADJUSTP4_OFFDIAG_CHECK3POS(           i, j, k   )  ADJUSTPOS_CHECK3POS(4, 1, 1, i, j, k   )
#define ADJUSTP5_OFFDIAG_CHECK1POS(           i         )  ADJUSTPOS_CHECK1POS(5, 1, 4, i         )
#define ADJUSTP5_OFFDIAG_CHECK2POS(           i, j      )  ADJUSTPOS_CHECK2POS(5, 1, 3, i, j      )
#define ADJUSTP5_OFFDIAG_CHECK3POS(           i, j, k   )  ADJUSTPOS_CHECK3POS(5, 1, 2, i, j, k   )
#define ADJUSTP5_OFFDIAG_CHECK4POS(           i, j, k, l)  ADJUSTPOS_CHECK4POS(5, 1, 1, i, j, k, l)

#define ADJUSTP2_ONDIAG_CHECK1POS(            i         )  ADJUSTPOS_CHECK1POS(2, 9, 1, i         )
#define ADJUSTP3_ONDIAG_CHECK1POS(            i         )  ADJUSTPOS_CHECK1POS(3, 9, 2, i         )
#define ADJUSTP3_ONDIAG_CHECK2POS(            i, j      )  ADJUSTPOS_CHECK2POS(3, 9, 1, i, j      )
#define ADJUSTP4_ONDIAG_CHECK1POS(            i         )  ADJUSTPOS_CHECK1POS(4, 9, 3, i         )
#define ADJUSTP4_ONDIAG_CHECK2POS(            i, j      )  ADJUSTPOS_CHECK2POS(4, 9, 2, i, j      )
#define ADJUSTP4_ONDIAG_CHECK3POS(            i, j, k   )  ADJUSTPOS_CHECK3POS(4, 9, 1, i, j, k   )
#define ADJUSTP5_ONDIAG_CHECK1POS(            i         )  ADJUSTPOS_CHECK1POS(5, 9, 4, i         )
#define ADJUSTP5_ONDIAG_CHECK2POS(            i, j      )  ADJUSTPOS_CHECK2POS(5, 9, 3, i, j      )
#define ADJUSTP5_ONDIAG_CHECK3POS(            i, j, k   )  ADJUSTPOS_CHECK3POS(5, 9, 2, i, j, k   )
#define ADJUSTP5_ONDIAG_CHECK4POS(            i, j, k, l)  ADJUSTPOS_CHECK4POS(5, 9, 1, i, j, k, l)

#define ADJUSTP4_PAWN_CHECK1POS(              i         )  ADJUSTPOS_CHECK1POS(4, 1, 1, i         )
#define ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(  i         )  ADJUSTPOS_CHECK1POS(3, 1, 1, i         )
#define ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(  i         )  ADJUSTPOS_CHECK1POS(4, 1, 2, i         )
#define ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(  i, j      )  ADJUSTPOS_CHECK2POS(4, 1, 1, i, j      )
#define ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP45(  i         )  ADJUSTPOS_CHECK1POS(5, 1, 3, i         )
#define ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(  i, j      )  ADJUSTPOS_CHECK2POS(5, 1, 2, i, j      )
#define ADJUSTP5_OFFDIAG_CHECK3POS_EQUALP45(  i, j, k   )  ADJUSTPOS_CHECK3POS(5, 1, 1, i, j, k   )
#define ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234( i         )  ADJUSTPOS_CHECK1POS(4, 1, 1, i         )
#define ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP345( i         )  ADJUSTPOS_CHECK1POS(5, 1, 2, i         )
#define ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP345( i, j      )  ADJUSTPOS_CHECK2POS(5, 1, 1, i, j      )

#define ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(   i         )  ADJUSTPOS_CHECK1POS(3, 9, 1, i         )
#define ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(   i         )  ADJUSTPOS_CHECK1POS(4, 9, 2, i         )
#define ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(   i, j      )  ADJUSTPOS_CHECK2POS(4, 9, 1, i, j      )
#define ADJUSTP5_ONDIAG_CHECK1POS_EQUALP45(   i         )  ADJUSTPOS_CHECK1POS(5, 9, 3, i         )
#define ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(   i, j      )  ADJUSTPOS_CHECK2POS(5, 9, 2, i, j      )
#define ADJUSTP5_ONDIAG_CHECK3POS_EQUALP45(   i, j, k   )  ADJUSTPOS_CHECK3POS(5, 9, 1, i, j, k   )
#define ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(  i         )  ADJUSTPOS_CHECK1POS(4, 9, 1, i         )
#define ADJUSTP5_ONDIAG_CHECK1POS_EQUALP345(  i         )  ADJUSTPOS_CHECK1POS(5, 9, 2, i         )
#define ADJUSTP5_ONDIAG_CHECK2POS_EQUALP345(  i, j      )  ADJUSTPOS_CHECK2POS(5, 9, 1, i, j      )

#define ADJUSTP2_PAWN_CHECK1POS(              i         )  ADJUSTPOS_CHECK1POS(2, 1, 3, i         )
#define ADJUSTP2_PAWN_CHECK2POS(              i, j      )  ADJUSTPOS_CHECK2POS(2, 1, 2, i, j      )
#define ADJUSTP2_PAWN_CHECK3POS(              i, j, k   )  ADJUSTPOS_CHECK3POS(2, 1, 1, i, j, k   )

UINT EndGameKey::getP2OffDiagIndex() const {         // Kings off maindiag
  const UINT p   = getPosition(2);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  return p - dec;
}

UINT EndGameKey::getP3OffDiagIndex() const {         // Kings, p2 off maindiag
  const UINT p   = getPosition(3);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndex() const {         // Kings, p2 or p3 off maindiag
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  if(p > getPosition(3)) dec++;
  return p - dec;
}

UINT EndGameKey::getP5OffDiagIndex() const {         // Kings, p2 or p3 or p4 off maindiag
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  if(p > getPosition(3)) dec++;
  if(p > getPosition(4)) dec++;
  return p - dec;
}

UINT EndGameKey::getP3OffDiagIndexEqualP23() const { // Kings or p2 off maindiag, ignore p2
  const UINT p   = getPosition(3);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndexEqualP34() const { // Kings, p2 or p3 off maindiag, ignore p3
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  return p - dec;
}

UINT EndGameKey::getP5OffDiagIndexEqualP45() const { // Kings, p2 or p3 or p4 off maindiag, ignore p4
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  if(p > getPosition(3)) dec++;
  return p - dec;
}

UINT EndGameKey::getP4OffDiagIndexEqualP234() const { // Kings, p2 or p3 off maindiag, ignore p2,p3
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  return p - dec;
}

UINT EndGameKey::getP5OffDiagIndexEqualP345() const { // Kings, p2, p3 or p4 off maindiag, ignore p3,p4
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 1 : 0;
  if(p > getPosition(1)) dec++;
  if(p > getPosition(2)) dec++;
  return p - dec;
}

UINT EndGameKey::getP2DiagIndex() const {            // Kings, p2 on maindiag
  const UINT p   = getPosition(2);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP3DiagIndex() const {            // Kings, p2, p3 on maindiag
  const UINT p   = getPosition(3);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndex() const {            // Kings, p2, p3, p4 on maindiag
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  if(p > getPosition(3)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP5DiagIndex() const {            // Kings, p2, p3, p4, p5 on maindiag
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  if(p > getPosition(3)) dec += 9;
  if(p > getPosition(4)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP3DiagIndexEqualP23() const {    // Kings, p2, p3 on maindiag, ignore p2
  const UINT p   = getPosition(3);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndexEqualP34() const {    // Kings, p2, p3, p4 on maindiag, ignore p3
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP5DiagIndexEqualP45() const {   // Kings, p2, p3, p4, p5 on maindiag, ignore p4
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  if(p > getPosition(3)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP4DiagIndexEqualP234() const {   // Kings, p2, p3, p4 on maindiag, ignore p2,p3
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  return (p - dec) / 9;
}

UINT EndGameKey::getP5DiagIndexEqualP345() const {   // Kings, p2, p3, p4, p5 on maindiag, ignore p3,p4
  const UINT p   = getPosition(5);
  UINT       dec = (p > getPosition(0)) ? 9 : 0;
  if(p > getPosition(1)) dec += 9;
  if(p > getPosition(2)) dec += 9;
  return (p - dec) / 9;
}

void EndGameKey::p2IndexToOffDiagPos() {
  UINT p = getPosition(2);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1
  case 0x01: ADJUSTP2_OFFDIAG_CHECK1POS(  1);            return; // p0          < p <    p1
  case 0x02: ADJUSTP2_OFFDIAG_CHECK1POS(0  );            return; //    p1       < p < p0
  case 0x03: setPosition(2,p + 2);                       return; // p0,p1       < p
  }
}

void EndGameKey::p3IndexToOffDiagPos() {
  UINT p = getPosition(3);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1,p2
  case 0x01: ADJUSTP3_OFFDIAG_CHECK2POS(  1,2);          return; // p0          < p <    p1,p2
  case 0x02: ADJUSTP3_OFFDIAG_CHECK2POS(0,  2);          return; //    p1       < p < p0,   p2
  case 0x03: ADJUSTP3_OFFDIAG_CHECK1POS(    2);          return; // p0,p1       < p <       p2
  case 0x04: ADJUSTP3_OFFDIAG_CHECK2POS(0,1  );          return; //       p2    < p < p0,p1
  case 0x05: ADJUSTP3_OFFDIAG_CHECK1POS(  1  );          return; // p0,   p2    < p <    p1
  case 0x06: ADJUSTP3_OFFDIAG_CHECK1POS(0    );          return; //    p1,p2    < p < p0
  case 0x07: setPosition(3,p + 3);                       return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToOffDiagPos() {
  UINT p = getPosition(4);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1,p2,p3
  case 0x01: ADJUSTP4_OFFDIAG_CHECK3POS(  1,2,3);        return; // p0          < p <    p1,p2,p3
  case 0x02: ADJUSTP4_OFFDIAG_CHECK3POS(0,  2,3);        return; //    p1       < p < p0,   p2,p3
  case 0x03: ADJUSTP4_OFFDIAG_CHECK2POS(    2,3);        return; // p0,p1       < p <       p2,p3
  case 0x04: ADJUSTP4_OFFDIAG_CHECK3POS(0,1,  3);        return; //       p2    < p < p0,p1,   p3
  case 0x05: ADJUSTP4_OFFDIAG_CHECK2POS(  1,  3);        return; // p0,   p2    < p <    p1,   p3
  case 0x06: ADJUSTP4_OFFDIAG_CHECK2POS(0,    3);        return; //    p1,p2    < p < p0,      p3
  case 0x07: ADJUSTP4_OFFDIAG_CHECK1POS(      3);        return; // p0,p1,p2    < p <          p3
  case 0x08: ADJUSTP4_OFFDIAG_CHECK3POS(0,1,2  );        return; //          p3 < p < p0,p1,p2
  case 0x09: ADJUSTP4_OFFDIAG_CHECK2POS(  1,2  );        return; // p0,      p3 < p <    p1,p2
  case 0x0A: ADJUSTP4_OFFDIAG_CHECK2POS(0,  2  );        return; //    p1,   p3 < p < p0,   p2
  case 0x0B: ADJUSTP4_OFFDIAG_CHECK1POS(    2  );        return; // p0,p1,   p3 < p <       p2
  case 0x0C: ADJUSTP4_OFFDIAG_CHECK2POS(0,1    );        return; //       p2,p3 < p < p0,p1
  case 0x0D: ADJUSTP4_OFFDIAG_CHECK1POS(  1    );        return; // p0,   p2,p3 < p <    p1
  case 0x0E: ADJUSTP4_OFFDIAG_CHECK1POS(0      );        return; //    p1,p2,p3 < p < p0
  case 0x0F: setPosition(4,p + 4);                       return; // p0,p1,p2,p3 < p
  }
}

void EndGameKey::p5IndexToOffDiagPos() {
  UINT p = getPosition(5);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  if(p >= getPosition(4)) ge |= 0x10;
  switch(ge) {
  case 0x00:                                             return; //                  p < p0,p1,p2,p3,p4
  case 0x01: ADJUSTP5_OFFDIAG_CHECK4POS(  1,2,3,4);      return; // p0             < p <    p1,p2,p3,p4
  case 0x02: ADJUSTP5_OFFDIAG_CHECK4POS(0,  2,3,4);      return; //    p1          < p < p0,   p2,p3,p4
  case 0x03: ADJUSTP5_OFFDIAG_CHECK3POS(    2,3,4);      return; // p0,p1          < p <       p2,p3,p4
  case 0x04: ADJUSTP5_OFFDIAG_CHECK4POS(0,1,  3,4);      return; //       p2       < p < p0,p1,   p3,p4
  case 0x05: ADJUSTP5_OFFDIAG_CHECK3POS(  1,  3,4);      return; // p0,   p2       < p <    p1,   p3,p4
  case 0x06: ADJUSTP5_OFFDIAG_CHECK3POS(0,    3,4);      return; //    p1,p2       < p < p0,      p3,p4
  case 0x07: ADJUSTP5_OFFDIAG_CHECK2POS(      3,4);      return; // p0,p1,p2       < p <          p3,p4
  case 0x08: ADJUSTP5_OFFDIAG_CHECK4POS(0,1,2  ,4);      return; //          p3    < p < p0,p1,p2   ,p4
  case 0x09: ADJUSTP5_OFFDIAG_CHECK3POS(  1,2  ,4);      return; // p0,      p3    < p <    p1,p2   ,p4
  case 0x0A: ADJUSTP5_OFFDIAG_CHECK3POS(0,  2  ,4);      return; //    p1,   p3    < p < p0,   p2   ,p4
  case 0x0B: ADJUSTP5_OFFDIAG_CHECK2POS(    2  ,4);      return; // p0,p1,   p3    < p <       p2   ,p4
  case 0x0C: ADJUSTP5_OFFDIAG_CHECK3POS(0,1    ,4);      return; //       p2,p3    < p < p0,p1      ,p4
  case 0x0D: ADJUSTP5_OFFDIAG_CHECK2POS(  1    ,4);      return; // p0,   p2,p3    < p <    p1      ,p4
  case 0x0E: ADJUSTP5_OFFDIAG_CHECK2POS(0      ,4);      return; //    p1,p2,p3    < p < p0         ,p4
  case 0x0F: ADJUSTP5_OFFDIAG_CHECK1POS(        4);      return; // p0,p1,p2,p3    < p <             p4
  case 0x10: ADJUSTP5_OFFDIAG_CHECK4POS(0,1,2,3  );      return; //             p4 < p < p0,p1,p2,p3
  case 0x11: ADJUSTP5_OFFDIAG_CHECK3POS(  1,2,3  );      return; // p0         ,p4 < p <    p1,p2,p3
  case 0x12: ADJUSTP5_OFFDIAG_CHECK3POS(0,  2,3  );      return; //    p1      ,p4 < p < p0,   p2,p3
  case 0x13: ADJUSTP5_OFFDIAG_CHECK2POS(    2,3  );      return; // p0,p1      ,p4 < p <       p2,p3
  case 0x14: ADJUSTP5_OFFDIAG_CHECK3POS(0,1,  3  );      return; //       p2   ,p4 < p < p0,p1,   p3
  case 0x15: ADJUSTP5_OFFDIAG_CHECK2POS(  1,  3  );      return; // p0,   p2   ,p4 < p <    p1,   p3
  case 0x16: ADJUSTP5_OFFDIAG_CHECK2POS(0,    3  );      return; //    p1,p2   ,p4 < p < p0,      p3
  case 0x17: ADJUSTP5_OFFDIAG_CHECK1POS(      3  );      return; // p0,p1,p2   ,p4 < p <          p3
  case 0x18: ADJUSTP5_OFFDIAG_CHECK3POS(0,1,2    );      return; //          p3,p4 < p < p0,p1,p2
  case 0x19: ADJUSTP5_OFFDIAG_CHECK2POS(  1,2    );      return; // p0,      p3,p4 < p <    p1,p2
  case 0x1A: ADJUSTP5_OFFDIAG_CHECK2POS(0,  2    );      return; //    p1,   p3,p4 < p < p0,   p2
  case 0x1B: ADJUSTP5_OFFDIAG_CHECK1POS(    2    );      return; // p0,p1,   p3,p4 < p <       p2
  case 0x1C: ADJUSTP5_OFFDIAG_CHECK2POS(0,1      );      return; //       p2,p3,p4 < p < p0,p1
  case 0x1D: ADJUSTP5_OFFDIAG_CHECK1POS(  1      );      return; // p0,   p2,p3,p4 < p <    p1
  case 0x1E: ADJUSTP5_OFFDIAG_CHECK1POS(0        );      return; //    p1,p2,p3,p4 < p < p0
  case 0x1F: setPosition(5,p + 5);                       return; // p0,p1,p2,p3,p4 < p
  }
}

void EndGameKey::p3IndexToOffDiagPosEqualP23() {
  UINT p = getPosition(3);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1
  case 0x01: ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(  1);   return; // p0          < p <    p1
  case 0x02: ADJUSTP3_OFFDIAG_CHECK1POS_EQUALP23(0  );   return; //    p1       < p < p0
  case 0x03: setPosition(3,p + 2);                       return; // p0,p1       < p
  }
}

void EndGameKey::p4IndexToOffDiagPosEqualP34() {
  UINT p = getPosition(4);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1,p2
  case 0x01: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(  1,2); return; // p0          < p <    p1,p2
  case 0x02: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(0,  2); return; //    p1       < p < p0,   p2
  case 0x03: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(    2); return; // p0,p1       < p <       p2
  case 0x04: ADJUSTP4_OFFDIAG_CHECK2POS_EQUALP34(0,1  ); return; //       p2    < p < p0,p1
  case 0x05: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(  1  ); return; // p0,   p2    < p <    p1
  case 0x06: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP34(0    ); return; //    p1,p2    < p < p0
  case 0x07: setPosition(4,p + 3);                       return; // p0,p1,p2    < p
  }
}

void EndGameKey::p5IndexToOffDiagPosEqualP45() {
  UINT p = getPosition(5);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  switch(ge) {
  case 0x00:                                               return; //               p < p0,p1,p2,p3
  case 0x01: ADJUSTP5_OFFDIAG_CHECK3POS_EQUALP45(  1,2,3); return; // p0          < p <    p1,p2,p3
  case 0x02: ADJUSTP5_OFFDIAG_CHECK3POS_EQUALP45(0,  2,3); return; //    p1       < p < p0,   p2,p3
  case 0x03: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(    2,3); return; // p0,p1       < p <       p2,p3
  case 0x04: ADJUSTP5_OFFDIAG_CHECK3POS_EQUALP45(0,1  ,3); return; //       p2    < p < p0,p1   ,p3
  case 0x05: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(  1  ,3); return; // p0,   p2    < p <    p1   ,p3
  case 0x06: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(0    ,3); return; //    p1,p2    < p < p0      ,p3
  case 0x07: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP45(      3); return; // p0,p1,p2    < p <          p3
  case 0x08: ADJUSTP5_OFFDIAG_CHECK3POS_EQUALP45(0,1,2  ); return; //          p3 < p < p0,p1,p2
  case 0x09: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(  1,2  ); return; // p0      ,p3 < p <    p1,p2
  case 0x0A: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(0,  2  ); return; //    p1   ,p3 < p < p0,   p2
  case 0x0B: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP45(    2  ); return; // p0,p1   ,p3 < p <       p2
  case 0x0C: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP45(0,1    ); return; //       p2,p3 < p < p0,p1
  case 0x0D: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP45(  1    ); return; // p0,   p2,p3 < p <    p1
  case 0x0E: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP45(0      ); return; //    p1,p2,p3 < p < p0
  case 0x0F: setPosition(5,p + 4);                         return; // p0,p1,p2,p3 < p
  }
}

void EndGameKey::p4IndexToOffDiagPosEqualP234() {
  UINT p = getPosition(4);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00:                                             return; //               p < p0,p1
  case 0x01: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234(  1);  return; // p0          < p <    p1
  case 0x02: ADJUSTP4_OFFDIAG_CHECK1POS_EQUALP234(0  );  return; //    p1       < p < p0
  case 0x03: setPosition(4,p + 2);                       return; // p0,p1       < p
  }
}

void EndGameKey::p5IndexToOffDiagPosEqualP345() {
  UINT p = getPosition(5);
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00:                                               return; //               p < p0,p1,p2
  case 0x01: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP345(  1,2);  return; // p0          < p <    p1,p2
  case 0x02: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP345(0  ,2);  return; //    p1       < p < p0   ,p2
  case 0x03: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP345(    2);  return; // p0,p1       < p <       p2
  case 0x04: ADJUSTP5_OFFDIAG_CHECK2POS_EQUALP345(0,1  );  return; //       p2      p < p0,p1
  case 0x05: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP345(  1  );  return; // p0   ,p2    < p <    p1
  case 0x06: ADJUSTP5_OFFDIAG_CHECK1POS_EQUALP345(0    );  return; //    p1,p2    < p < p0
  case 0x07: setPosition(5,p + 3);                         return; // p0,p1,p2    < p
  }
}

void EndGameKey::p2IndexToDiagPos() {
  UINT p = getPosition(2) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00 : setPosition(2,p);                          return; //               p < p0,p1
  case 0x01 : ADJUSTP2_ONDIAG_CHECK1POS(  1);            return; // p0          < p <    p1
  case 0x02 : ADJUSTP2_ONDIAG_CHECK1POS(0  );            return; //    p1       < p < p0
  case 0x03 : setPosition(2,p + 2*9);                    return; // p0,p1       < p
  }
}

void EndGameKey::p3IndexToDiagPos() {
  UINT p = getPosition(3) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00 : setPosition(3,p);                          return; //               p < p0,p1,p2
  case 0x01 : ADJUSTP3_ONDIAG_CHECK2POS(  1,2);          return; // p0          < p <    p1,p2
  case 0x02 : ADJUSTP3_ONDIAG_CHECK2POS(0,  2);          return; //    p1       < p < p0,   p2
  case 0x03 : ADJUSTP3_ONDIAG_CHECK1POS(    2);          return; // p0,p1       < p <       p2
  case 0x04 : ADJUSTP3_ONDIAG_CHECK2POS(0,1  );          return; //       p2    < p < p0,p1
  case 0x05 : ADJUSTP3_ONDIAG_CHECK1POS(  1  );          return; // p0,   p2    < p <    p1
  case 0x06 : ADJUSTP3_ONDIAG_CHECK1POS(0    );          return; //    p1,p2    < p < p0
  case 0x07 : setPosition(3,p + 3*9);                    return; // p0,p1,p2    < p
  }
}

void EndGameKey::p4IndexToDiagPos() {
  UINT p = getPosition(4) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  switch(ge) {
  case 0x00: setPosition(4,p);                          return; //               p < p0,p1,p2,p3
  case 0x01: ADJUSTP4_ONDIAG_CHECK3POS(  1,2,3);        return; // p0          < p <    p1,p2,p3
  case 0x02: ADJUSTP4_ONDIAG_CHECK3POS(0,  2,3);        return; //    p1       < p < p0,   p2,p3
  case 0x03: ADJUSTP4_ONDIAG_CHECK2POS(    2,3);        return; // p0,p1       < p <       p2,p3
  case 0x04: ADJUSTP4_ONDIAG_CHECK3POS(0,1,  3);        return; //       p2    < p < p0,p1,   p3
  case 0x05: ADJUSTP4_ONDIAG_CHECK2POS(  1,  3);        return; // p0,   p2    < p <    p1,   p3
  case 0x06: ADJUSTP4_ONDIAG_CHECK2POS(0,    3);        return; //    p1,p2    < p < p0,      p3
  case 0x07: ADJUSTP4_ONDIAG_CHECK1POS(      3);        return; // p0,p1,p2    < p <          p3
  case 0x08: ADJUSTP4_ONDIAG_CHECK3POS(0,1,2  );        return; //          p3 < p < p0,p1,p2
  case 0x09: ADJUSTP4_ONDIAG_CHECK2POS(  1,2  );        return; // p0,      p3 < p <    p1,p2
  case 0x0A: ADJUSTP4_ONDIAG_CHECK2POS(0,  2  );        return; //    p1,   p3 < p < p0,   p2
  case 0x0B: ADJUSTP4_ONDIAG_CHECK1POS(    2  );        return; // p0,p1,   p3 < p <       p2
  case 0x0C: ADJUSTP4_ONDIAG_CHECK2POS(0,1    );        return; //       p2,p3 < p < p0,p1
  case 0x0D: ADJUSTP4_ONDIAG_CHECK1POS(  1    );        return; // p0,   p2,p3 < p <    p1
  case 0x0E: ADJUSTP4_ONDIAG_CHECK1POS(0      );        return; //    p1,p2,p3 < p <    p1
  case 0x0F: setPosition(4,p + 4*9);                    return; // p0,p1,p2,p3 < p
  }
}

void EndGameKey::p5IndexToDiagPos() {
  UINT p = getPosition(5) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  if(p >= getPosition(4)) ge |= 0x10;
  switch(ge) {
  case 0x00: setPosition(5,p);                            return; //                  p < p0,p1,p2,p3,p4
  case 0x01: ADJUSTP5_ONDIAG_CHECK4POS(  1,2,3,4);        return; // p0             < p <    p1,p2,p3,p4
  case 0x02: ADJUSTP5_ONDIAG_CHECK4POS(0,  2,3,4);        return; //    p1          < p < p0,   p2,p3,p4
  case 0x03: ADJUSTP5_ONDIAG_CHECK3POS(    2,3,4);        return; // p0,p1          < p <       p2,p3,p4
  case 0x04: ADJUSTP5_ONDIAG_CHECK4POS(0,1,  3,4);        return; //       p2       < p < p0,p1,   p3,p4
  case 0x05: ADJUSTP5_ONDIAG_CHECK3POS(  1,  3,4);        return; // p0,   p2       < p <    p1,   p3,p4
  case 0x06: ADJUSTP5_ONDIAG_CHECK3POS(0,    3,4);        return; //    p1,p2       < p < p0,      p3,p4
  case 0x07: ADJUSTP5_ONDIAG_CHECK2POS(      3,4);        return; // p0,p1,p2       < p <          p3,p4
  case 0x08: ADJUSTP5_ONDIAG_CHECK4POS(0,1,2  ,4);        return; //          p3    < p < p0,p1,p2   ,p4
  case 0x09: ADJUSTP5_ONDIAG_CHECK3POS(  1,2  ,4);        return; // p0,      p3    < p <    p1,p2   ,p4
  case 0x0A: ADJUSTP5_ONDIAG_CHECK3POS(0,  2  ,4);        return; //    p1,   p3    < p < p0,   p2   ,p4
  case 0x0B: ADJUSTP5_ONDIAG_CHECK2POS(    2  ,4);        return; // p0,p1,   p3    < p <       p2   ,p4
  case 0x0C: ADJUSTP5_ONDIAG_CHECK3POS(0,1    ,4);        return; //       p2,p3    < p < p0,p1      ,p4
  case 0x0D: ADJUSTP5_ONDIAG_CHECK2POS(  1    ,4);        return; // p0,   p2,p3    < p <    p1      ,p4
  case 0x0E: ADJUSTP5_ONDIAG_CHECK2POS(0      ,4);        return; //    p1,p2,p3    < p < p0         ,p4
  case 0x0F: ADJUSTP5_ONDIAG_CHECK1POS(        4);        return; // p0,p1,p2,p3    < p <            ,p4
  case 0x10: ADJUSTP5_ONDIAG_CHECK4POS(0,1,2,3  );        return; //            ,p4   p < p0,p1,p2,p3
  case 0x11: ADJUSTP5_ONDIAG_CHECK3POS(  1,2,3  );        return; // p0         ,p4 < p <    p1,p2,p3
  case 0x12: ADJUSTP5_ONDIAG_CHECK3POS(0,  2,3  );        return; //    p1      ,p4 < p < p0,   p2,p3
  case 0x13: ADJUSTP5_ONDIAG_CHECK2POS(    2,3  );        return; // p0,p1      ,p4 < p <       p2,p3
  case 0x14: ADJUSTP5_ONDIAG_CHECK3POS(0,1,  3  );        return; //       p2   ,p4 < p < p0,p1,   p3
  case 0x15: ADJUSTP5_ONDIAG_CHECK2POS(  1,  3  );        return; // p0,   p2   ,p4 < p <    p1,   p3
  case 0x16: ADJUSTP5_ONDIAG_CHECK2POS(0,    3  );        return; //    p1,p2   ,p4 < p < p0,      p3
  case 0x17: ADJUSTP5_ONDIAG_CHECK1POS(      3  );        return; // p0,p1,p2   ,p4 < p <          p3
  case 0x18: ADJUSTP5_ONDIAG_CHECK3POS(0,1,2    );        return; //          p3,p4 < p < p0,p1,p2
  case 0x19: ADJUSTP5_ONDIAG_CHECK2POS(  1,2    );        return; // p0,      p3,p4 < p <    p1,p2
  case 0x1A: ADJUSTP5_ONDIAG_CHECK2POS(0,  2    );        return; //    p1,   p3,p4 < p < p0,   p2
  case 0x1B: ADJUSTP5_ONDIAG_CHECK1POS(    2    );        return; // p0,p1,   p3,p4 < p <       p2
  case 0x1C: ADJUSTP5_ONDIAG_CHECK2POS(0,1      );        return; //       p2,p3,p4 < p < p0,p1
  case 0x1D: ADJUSTP5_ONDIAG_CHECK1POS(  1      );        return; // p0,   p2,p3,p4 < p <    p1
  case 0x1E: ADJUSTP5_ONDIAG_CHECK1POS(0        );        return; //    p1,p2,p3,p4 < p < p0
  case 0x1F: setPosition(5,p + 5*9);                      return; // p0,p1,p2,p3,p4 < p
  }
}

void EndGameKey::p3IndexToDiagPosEqualP23() {
  UINT p = getPosition(3) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00 : setPosition(3,p);                          return; //               p < p0,p1
  case 0x01 : ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(1);     return; // p0          < p <    p1
  case 0x02 : ADJUSTP3_ONDIAG_CHECK1POS_EQUALP23(0);     return; //    p1       < p < p0
  case 0x03 : setPosition(3,p + 2*9);                    return; // p0,p1       < p
  }
}

void EndGameKey::p4IndexToDiagPosEqualP34() {
  UINT p = getPosition(4) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00 : setPosition(4,p);                          return; //               p < p0,p1,p2
  case 0x01 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(  1,2); return; // p0          < p <    p1,p2
  case 0x02 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(0,  2); return; //    p1       < p < p0,   p2
  case 0x03 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(    2); return; // p0,p1       < p <       p2
  case 0x04 : ADJUSTP4_ONDIAG_CHECK2POS_EQUALP34(0,1  ); return; //       p2    < p < p0,p1
  case 0x05 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(  1  ); return; // p0,   p2    < p <    p1
  case 0x06 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP34(0    ); return; //    p1,p2    < p < p0
  case 0x07 : setPosition(4,p + 3*9);                    return; // p0,p1,p2    < p
  }
}

void EndGameKey::p5IndexToDiagPosEqualP45() {
  UINT p = getPosition(5) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  if(p >= getPosition(3)) ge |= 0x08;
  switch(ge) {
  case 0x00 : setPosition(5,p);                            return; //               p < p0,p1,p2,p3
  case 0x01 : ADJUSTP5_ONDIAG_CHECK3POS_EQUALP45(  1,2,3); return; // p0          < p <    p1,p2,p3
  case 0x02 : ADJUSTP5_ONDIAG_CHECK3POS_EQUALP45(0,  2,3); return; //    p1       < p < p0,   p2,p3
  case 0x03 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(    2,3); return; // p0,p1       < p <       p2,p3
  case 0x04 : ADJUSTP5_ONDIAG_CHECK3POS_EQUALP45(0,1  ,3); return; //       p2    < p < p0,p1   ,p3
  case 0x05 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(  1  ,3); return; // p0,   p2    < p <    p1   ,p3
  case 0x06 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(0    ,3); return; //    p1,p2    < p < p0      ,p3
  case 0x07 : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP45(      3); return; // p0,p1,p2    < p            p3
  case 0x08 : ADJUSTP5_ONDIAG_CHECK3POS_EQUALP45(0,1,2  ); return; //         ,p3   p < p0,p1,p2
  case 0x09 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(  1,2  ); return; // p0      ,p3 < p <    p1,p2
  case 0x0A : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(0,  2  ); return; //    p1   ,p3 < p < p0,   p2
  case 0x0B : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP45(    2  ); return; // p0,p1   ,p3 < p <       p2
  case 0x0C : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP45(0,1    ); return; //       p2,p3 < p < p0,p1
  case 0x0D : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP45(  1    ); return; // p0,   p2,p3 < p <    p1
  case 0x0E : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP45(0      ); return; //    p1,p2,p3 < p < p0
  case 0x0F : setPosition(5,p + 4*9);                      return; // p0,p1,p2,p3 < p

  }
}

void EndGameKey::p4IndexToDiagPosEqualP234() {
  UINT p = getPosition(4) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  switch(ge) {
  case 0x00 : setPosition(4,p);                          return; //               p < p0,p1
  case 0x01 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(1);    return; // p0          < p <    p1
  case 0x02 : ADJUSTP4_ONDIAG_CHECK1POS_EQUALP234(0);    return; //    p1       < p < p0
  case 0x03 : setPosition(4,p + 2*9);                    return; // p0,p1       < p
  }
}

void EndGameKey::p5IndexToDiagPosEqualP345() {
  UINT p = getPosition(5) * 9;
  int ge = 0;
  if(p >= getPosition(0)) ge |= 0x01;
  if(p >= getPosition(1)) ge |= 0x02;
  if(p >= getPosition(2)) ge |= 0x04;
  switch(ge) {
  case 0x00 : setPosition(5,p);                           return; //               p < p0,p1,p2
  case 0x01 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP345(  1,2); return; // p0          < p <    p1,p2
  case 0x02 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP345(0  ,2); return; //    p1       < p < p0   ,p2
  case 0x03 : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP345(    2); return; // p0,p1         p <       p2
  case 0x04 : ADJUSTP5_ONDIAG_CHECK2POS_EQUALP345(0,1  ); return; //       p2    < p < p0,p1
  case 0x05 : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP345(  1  ); return; // p0   ,p2    < p <    p1
  case 0x06 : ADJUSTP5_ONDIAG_CHECK1POS_EQUALP345(0    ); return; //    p1,p2    < p < p0
  case 0x07 : setPosition(5,p + 3*9);                     return; // p0,p1,p2    < p
  }
}

UINT EndGameKey::getP3Pawn2Index() const { // higest position is G7
  const UINT p   = getPosition(3);
  UINT       dec = (p > getPosition(2)) ? 1 : 0;
  return p - dec;
}

UINT EndGameKey::getP4Pawn3Index() const { // higest position is G7
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(2)) ? 1 : 0;
  if(p > getPosition(3)) dec++;
  return p - dec;
}

void EndGameKey::p3IndexToPawn2Pos() {
  const UINT p = getPosition(3);
  if(p >= getPosition(2)) {
    setPosition(3,p+1);
  }
}

void EndGameKey::p4IndexToPawn3PosEqualP34() {
  const UINT p = getPosition(4);
  if(p >= getPosition(2)) {
    setPosition(4,p+1);
  }
}

void EndGameKey::p34IndexToPawn23PosEqualP34() {
  setPosition(3,getPosition(3) + 8);
  setPosition(4,getPosition(4) + 8);
  p3IndexToPawn2Pos();
  p4IndexToPawn3PosEqualP34();
}

void EndGameKey::p4IndexToPawn3Pos() {
  UINT p = getPosition(4);
  int ge = 0;
  if(p >= getPosition(2)) ge |= 0x01;
  if(p >= getPosition(3)) ge |= 0x02;
  switch(ge) {
  case 0x00:                                             return; //               p < p2,p3
  case 0x01: ADJUSTP4_PAWN_CHECK1POS(3);                 return; // p2          < p <    p3
  case 0x02: ADJUSTP4_PAWN_CHECK1POS(2);                 return; //    p3       < p < p2
  case 0x03: setPosition(4,p + 2);                       return; // p2,p3       < p
  }
}

UINT EndGameKey::getP4Pawn3IndexEqualP34() const {
  const UINT p   = getPosition(4);
  UINT       dec = (p > getPosition(2)) ? 1 : 0;
  return p - dec;
}

String EndGameKey::toString(const EndGameKeyDefinition &keydef, bool initFormat) const {
  TCHAR result[200];
  return keydef.createKeyString(result, *this, initFormat);
}
