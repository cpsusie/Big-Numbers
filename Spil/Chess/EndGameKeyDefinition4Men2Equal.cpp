#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

#define KK_OFFDIAG_POSCOUNT   (MAXINDEX_KK_OFFDIAG_2MEN  - MININDEX_KK_OFFDIAG_2MEN )
#define KK_ONDIAG_POSCOUNT    (MAXINDEX_KK_ONDIAG_2MEN   - MININDEX_KK_ONDIAG_2MEN  )
#define KKP2_ONDIAG_POSCOUNT  (MAXINDEX_KKP2_ONDIAG_4MEN - MININDEX_KKP2_ONDIAG_4MEN)

#define START_RANGE_KK_ONDIAG_P23_BELOWDIAG    (                                       GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT,61))
#define START_RANGE_KK_ONDIAG_P2_BELOWDIAG     (START_RANGE_KK_ONDIAG_P23_BELOWDIAG  + GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT ,27))
#define START_RANGE_KKP2_ONDIAG                (START_RANGE_KK_ONDIAG_P2_BELOWDIAG   + GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT ,28))
#define START_RANGE_KKP23_ONDIAG               (START_RANGE_KKP2_ONDIAG              + KKP2_ONDIAG_POSCOUNT)

EndGameKeyDefinition4Men2Equal::EndGameKeyDefinition4Men2Equal(PieceKey pk23)
: EndGameKeyDefinitionDupletsAllowed(pk23)
{
  assert(GET_TYPE_FROMKEY(pk23) != Pawn); // for pk23 = Pawn  , use superclass EndGameKeyDefinition2Pawns

/*
  DUMP_MACRO(     KK_OFFDIAG_POSCOUNT                        );
  DUMP_MACRO(     KK_ONDIAG_POSCOUNT                         );
  DUMP_MACRO(     KKP2_ONDIAG_POSCOUNT                       );

  DUMP_MACRO(     START_RANGE_KK_ONDIAG_P23_BELOWDIAG        );
  DUMP_MACRO(     START_RANGE_KK_ONDIAG_P2_BELOWDIAG         );
  DUMP_MACRO(     START_RANGE_KKP2_ONDIAG                    );
  DUMP_MACRO(     START_RANGE_KKP23_ONDIAG                   );
*/
}

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos2 > pos3) {                                                                                                               \
    tmp.setPosition2(pos3);                                                                                                       \
    tmp.setPosition3(pos2);                                                                                                       \
  }                                                                                                                               \
  const UINT pi2 = tmp.getP2OffDiagIndex();                                                                                       \
  const UINT pi3 = tmp.getP3OffDiagIndexEqualP23();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_OFFDIAG_2MEN(tmp), KK_OFFDIAG_POSCOUNT, pi2, pi3))                                              \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KK_ONDIAG_NOFLIP(key)                                                                                              \
{ UINT pi2 = offDiagPosToIndex[pos2];                                                                                             \
  UINT pi3 = offDiagPosToIndex[pos3];                                                                                             \
  SORT2(pi2, pi3); /* pi2 < pi3 */                                                                                                \
  return ADDPIT(key, ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3))                                                \
       + START_RANGE_KK_ONDIAG_P23_BELOWDIAG                                                                                      \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KK_ONDIAG_FLIPi(key,i,j)                                                                                           \
{ UINT pi = offDiagPosToIndex[pos##i] - 28;                                                                                       \
  UINT pj = offDiagPosToIndex[pos##j];                                                                                            \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi, pj))                                      \
       + START_RANGE_KK_ONDIAG_P2_BELOWDIAG                                                                                       \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KKP2_ONDIAG(key) return (KKP2_ONDIAG_4MEN_INDEX(key) + START_RANGE_KKP2_ONDIAG - MININDEX)

#define ENCODE_KKP3_ONDIAG(key)                                                                                                   \
{ EndGameKey tmp = key;                                                                                                           \
  tmp.setPosition2(pos3);                                                                                                         \
  tmp.setPosition3(pos2);                                                                                                         \
  ENCODE_KKP2_ONDIAG(tmp);                                                                                                        \
}

#define ENCODE_KKP23_ONDIAG(key)                                                                                                  \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos2 > pos3) {                                                                                                               \
    tmp.setPosition2(pos3);                                                                                                       \
    tmp.setPosition3(pos2);                                                                                                       \
  }                                                                                                                               \
  const UINT pi2 = tmp.getP2DiagIndex();                                                                                  \
  const UINT pi3 = tmp.getP3DiagIndexEqualP23();                                                                          \
  return ADDPIT(tmp, ADD2EQUAL(KK_ONDIAG_2MEN(tmp), KK_ONDIAG_POSCOUNT, pi2, pi3))                                                \
       + START_RANGE_KKP23_ONDIAG                                                                                                 \
       - MININDEX;                                                                                                                \
}

unsigned long EndGameKeyDefinition4Men2Equal::keyToIndex(const EndGameKey &key) const {
  UINT pos2 = key.getPosition2();
  UINT pos3 = key.getPosition3();

  if(!key.kingsOnMainDiag1()) {                                         // kings off maindiag => p2, p3 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else {                                                              // kings on    diag
    switch(BOOL2MASK(IS_OFFMAINDIAG1, pos2, pos3)) {
    case 0: ENCODE_KKP23_ONDIAG(           key  );                      // 2,3   on    diag
    case 1: ENCODE_KKP3_ONDIAG(            key  );                      //   3   on    diag
    case 2: ENCODE_KKP2_ONDIAG(            key  );                      // 2     on    diag
    case 3:                                                             // none  on    diag
      switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos2, pos3)) {
      case 0: ENCODE_KK_ONDIAG_NOFLIP(key    );                         // 2,3   below diag
      case 1: ENCODE_KK_ONDIAG_FLIPi( key,2,3);                         //   3   below diag
      case 2: ENCODE_KK_ONDIAG_FLIPi( key,3,2);                         // 2     below diag
      case 3: ;                                                         // none  below diag => error
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition4Men2Equal::indexToKey(unsigned long index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_P23_BELOWDIAG) {
    SETPIT(              result, index   );
    SET2POS2EQUAL(       result, index, KK_OFFDIAG_POSCOUNT, 2, 3);
    SETKK_OFFDIAG(       result, index   );
    result.p23IndexToOffDiagPosEqualP23();
  } else if(index < START_RANGE_KK_ONDIAG_P2_BELOWDIAG) {
    index -= START_RANGE_KK_ONDIAG_P23_BELOWDIAG;
    SETPIT(              result, index   );
    SET2OFFDIAGPOSNOFLIP(result, index, KK_ONDIAG_POSCOUNT, 2, 3);
    SETKK_ONDIAG(        result, index   );
  } else if(index < START_RANGE_KKP2_ONDIAG) {
    index -= START_RANGE_KK_ONDIAG_P2_BELOWDIAG;
    SETPIT(              result, index   );
    SET2OFFDIAGPOSFLIPj( result, index, KK_ONDIAG_POSCOUNT, 2, 3);
    SETKK_ONDIAG(        result, index   );
  } else if(index < START_RANGE_KKP23_ONDIAG) {
    index -= START_RANGE_KKP2_ONDIAG;
    SETPIT(              result, index   );
    SETPOS_BELOWDIAG(    result, index, 3);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
  } else {
    index -= START_RANGE_KKP23_ONDIAG;
    SETPIT(              result, index   );
    SET2POS2EQUAL(       result, index, KK_ONDIAG_POSCOUNT, 2, 3);
    SETKK_ONDIAG(        result, index   );
    result.p23IndexToDiagPosEqualP23();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition4Men2Equal::getSymTransformation(const EndGameKey &key) const {
  return getSym8Transformation4Men2Equal(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition4Men2Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
    if(allPreviousOnDiag) {
      for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) {
        const int pos2 = subDiagIndexToPos[i];
        if(key.isOccupied(pos2)) {
          continue;
        }
        key.setPosition2(pos2);
        scanPositions(key, 3, IS_ONMAINDIAG1(pos2));
        key.clearField(pos2);
      }
    } else {
      for(int pos2 = 0; pos2 < 64; pos2++) {
        if(key.isOccupied(pos2)) {
          continue;
        }
        key.setPosition2(pos2);
        scanPositions(key, 3, false);
        key.clearField(pos2);
      }
    }
    break;
  case 3:
    { const int pos2 = key.getPosition2();
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) {
          const int pos3 = subDiagIndexToPos[i];
          if(key.isOccupied(pos3)) {
            continue;
          }
          if(IS_ONMAINDIAG1(pos3) && (pos3 <= pos2)) {
            continue;
          }
          key.setPosition3(pos3);
          checkForBothPlayers(key);
          key.clearField(pos3);
        }
      } else if(!key.kingsOnMainDiag1()) { // kings off maindiag => pos3 = [pos2+1..63]
        for(int pos3 = pos2+1; pos3 < 64; pos3++) {
          if(key.isOccupied(pos3)) {
            continue;
          }
          key.setPosition3(pos3);
          checkForBothPlayers(key);
          key.clearField(pos3);
        }
      } else {     // kings on maindiag, p2 off maindiag => p3 must be off maindiag and "above" p2
        assert(key.kingsOnMainDiag1() && !IS_ONMAINDIAG1(pos2));
        const int pi2 = offDiagPosToIndex[pos2];
        for(int pi3 = pi2+1; pi3 < ARRAYSIZE(offDiagIndexToPos); pi3++) {
          const int pos3 = offDiagIndexToPos[pi3];
          if(key.isOccupied(pos3)) {
            continue;
          }
          if(!IS_SAMESIDEMAINDIAG1(pos2, pos3)) {
            if(pi2 > pi3) {
              if(pi2 - 28 > pi3) continue;
            } else {
              if(pi2 > pi3 - 28) continue;;
            }
          }
          key.setPosition3(pos3);
          checkForBothPlayers(key);
          key.clearField(pos3);
        }
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition4Men2Equal::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition4Men2Equal::scanPositions);
}

String EndGameKeyDefinition4Men2Equal::getCodecName() const {
  return _T("4Men2Equal");
}

#endif

#ifdef _DEBUG

UINT add2Equal(UINT addr, UINT maxAddr, UINT lp, UINT hp) {
  maxAddr /= 2;
  if((lp >= hp) || (addr >= maxAddr)) {
    throwException(_T("add2Equal:addr=%u, maxAddr=%u, (lp,hp)=(%u,%u). Assume (addr < maxAddr) && (lp < hp)")
                  ,addr, maxAddr, lp, hp);
  }
  UINT a = addr * hp;
  a += lp;
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr,hp-1);
  return a + rs;
}

UINT add2EqualAllowEqualLH(UINT addr, UINT maxAddr, UINT lp, UINT hp) {
  maxAddr /= 2;
  if((lp > hp) || (addr >= maxAddr)) {
    throwException(_T("add2EqualAllowEqualLH:addr=%u, maxAddr=%u, (lp,hp)=(%u,%u). Assume (addr < maxAddr) && (lp <= hp)")
                  ,addr, maxAddr, lp, hp);
  }
  UINT a = addr * (hp+1);
  a += lp;
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, hp);
  return a + rs;
}

void set2Pos2Equal(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  key.setPosition(hpIndex, r);
  key.setPosition(lpIndex, addr % r);
  addr /= r;
}

void set2OffDiagPosNoFlip(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  key.setPosition(hpIndex, EndGameKeyDefinition::offDiagIndexToPos[r]);
  key.setPosition(lpIndex, EndGameKeyDefinition::offDiagIndexToPos[addr % r]);
  addr /= r;
}

void set2OffDiagPosFlipi(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  key.setPosition(hpIndex, EndGameKeyDefinition::offDiagIndexToPos[r]);
  r++;
  key.setPosition(lpIndex, EndGameKeyDefinition::offDiagIndexToPos[addr % r + 28]);
  addr /= r;
}

void set2OffDiagPosFlipj(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  key.setPosition(hpIndex, EndGameKeyDefinition::offDiagIndexToPos[r+28]);
  r++;
  key.setPosition(lpIndex, EndGameKeyDefinition::offDiagIndexToPos[addr % r]);
  addr /= r;
}

void set2OffDiagPosFlipij(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  const UINT rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  key.setPosition(hpIndex, EndGameKeyDefinition::offDiagIndexToPos[r+28]);
  key.setPosition(lpIndex, EndGameKeyDefinition::offDiagIndexToPos[addr % r + 28]);
  addr /= r;
}

#endif
