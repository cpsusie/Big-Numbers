#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

#define KK_OFFDIAG_POSCOUNT   (MAXINDEX_KK_OFFDIAG_3MEN   - MININDEX_KK_OFFDIAG_3MEN  )
#define KK_ONDIAG_POSCOUNT    (MAXINDEX_KK_ONDIAG_3MEN    - MININDEX_KK_ONDIAG_3MEN   )
#define KKP2_ONDIAG_POSCOUNT  (MAXINDEX_KKP2_ONDIAG_3MEN  - MININDEX_KKP2_ONDIAG_3MEN )
#define KKP23_ONDIAG_POSCOUNT (MAXINDEX_KKP23_ONDIAG_5MEN - MININDEX_KKP23_ONDIAG_5MEN)

#define START_RANGE_KK_ONDIAG1                    (                                            GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT ,60))
#define START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG     (START_RANGE_KK_ONDIAG1                    + GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT  ,60))
#define START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG      (START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG     + GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT,27))
#define START_RANGE_KKP23_ONDIAG                  (START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG      + GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT,28))
#define START_RANGE_KKP234_ONDIAG                 (START_RANGE_KKP23_ONDIAG                  + KKP23_ONDIAG_POSCOUNT)

EndGameKeyDefinition5Men2Equal::EndGameKeyDefinition5Men2Equal(PieceKey pk2, PieceKey pk34)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk34)
{
  assert((pk2 != pk34)
      && (GET_TYPE_FROMKEY(pk34) != Pawn  )  // for pk34 = Pawn  : use EndGameKeyDefinition5Men2Pawns
      && (GET_TYPE_FROMKEY(pk2 ) != Pawn  )  // for pk2  = Pawn  : use EndGameKeyDefinition5Men1Pawn2Equal
        );
/*

  DUMP_MACRO(     KK_OFFDIAG_POSCOUNT                        );
  DUMP_MACRO(     KK_ONDIAG_POSCOUNT                         );
  DUMP_MACRO(     KKP2_ONDIAG_POSCOUNT                       );
  DUMP_MACRO(     KKP23_ONDIAG_POSCOUNT                      );

  DUMP_MACRO(     START_RANGE_KK_ONDIAG1                     );
  DUMP_MACRO(     START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG      );
  DUMP_MACRO(     START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG       );
  DUMP_MACRO(     START_RANGE_KKP23_ONDIAG                   );
  DUMP_MACRO(     START_RANGE_KKP234_ONDIAG                  );
*/
}

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos3 > pos4) {                                                                                                               \
    tmp.setPosition3(pos4);                                                                                                       \
    tmp.setPosition4(pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = tmp.getP4OffDiagIndexEqualP34();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_OFFDIAG_3MEN(tmp), KK_OFFDIAG_POSCOUNT, pi3, pi4))                                              \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KK_ONDIAG(key)                                                                                                     \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos3 > pos4) {                                                                                                               \
    tmp.setPosition3(pos4);                                                                                                       \
    tmp.setPosition4(pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = tmp.getP4OffDiagIndexEqualP34();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_ONDIAG_3MEN(tmp), KK_ONDIAG_POSCOUNT, pi3, pi4))                                                \
       + START_RANGE_KK_ONDIAG1                                                                                                   \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KKP2_ONDIAG_NOFLIP(key)                                                                                            \
{ UINT pi3 = offDiagPosToIndex[pos3];                                                                                             \
  UINT pi4 = offDiagPosToIndex[pos4];                                                                                             \
  SORT2(pi3,pi4);                                                                                                                 \
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi3, pi4))                                            \
       + START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KKP2_ONDIAG_FLIPi(key,i,j)                                                                                         \
{ UINT pi = offDiagPosToIndex[pos##i] - 28;                                                                                       \
  UINT pj = offDiagPosToIndex[pos##j];                                                                                            \
  SORT2(pi,pj);                                                                                                                   \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))                                  \
       + START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG                                                                                     \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KKP23_ONDIAG(key)  return (KKP23_ONDIAG_5MEN_INDEX(key) + START_RANGE_KKP23_ONDIAG - MININDEX);

#define ENCODE_KKP24_ONDIAG(key)                                                                                                  \
{ EndGameKey tmp = key;                                                                                                           \
  tmp.setPosition3(pos4);                                                                                                         \
  tmp.setPosition4(pos3);                                                                                                         \
  ENCODE_KKP23_ONDIAG(tmp);                                                                                                       \
}

#define ENCODE_KKP234_ONDIAG(key)                                                                                                 \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos3 > pos4) {                                                                                                               \
    tmp.setPosition3(pos4);                                                                                                       \
    tmp.setPosition4(pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3DiagIndex();                                                                                  \
  const UINT pi4 = tmp.getP4DiagIndexEqualP34();                                                                          \
  return ADDPIT(tmp, ADD2EQUAL(KKP2_ONDIAG_3MEN(tmp), KKP2_ONDIAG_POSCOUNT, pi3, pi4))                                            \
       + START_RANGE_KKP234_ONDIAG                                                                                                \
       - MININDEX;                                                                                                                \
}

unsigned long EndGameKeyDefinition5Men2Equal::keyToIndex(const EndGameKey &key) const {
  const UINT pos3 = key.getPosition3();
  const UINT pos4 = key.getPosition4();

  if(!key.kingsOnMainDiag1()) {                                // Kings off maindiag => p2, p3, p4 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else if(!key.p2OnMainDiag1()) {                            // Kings on maindiag, p2 below, p3, p4 anywhere
    ENCODE_KK_ONDIAG(key)
  } else {                                                     // Kings, p2 on mandiag
    switch(BOOL2MASK(IS_OFFMAINDIAG1, pos3, pos4)) {
    case 0: ENCODE_KKP234_ONDIAG(          key  );                      // 3,4   on    diag
    case 1: ENCODE_KKP24_ONDIAG(           key  );                      //   4   on    diag
    case 2: ENCODE_KKP23_ONDIAG(           key  );                      // 3     on    diag
    case 3:
      { switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos3, pos4)) {
        case 0: ENCODE_KKP2_ONDIAG_NOFLIP(key    );                     // 3,4   below diag
        case 1: ENCODE_KKP2_ONDIAG_FLIPi( key,3,4);                     //   4   below diag
        case 2: ENCODE_KKP2_ONDIAG_FLIPi( key,4,3);                     // 3     below diag
        case 3: ;                                                       // none  below diag => error
        }
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition5Men2Equal::indexToKey(unsigned long index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG1) {
    SETPIT(              result, index   );
    SET2POS2EQUAL(       result, index, KK_OFFDIAG_POSCOUNT,  3, 4);
    SETP2_INDEX(         result, index   );
    SETKK_OFFDIAG(       result, index   );
    result.p2IndexToOffDiagPos();
    result.p34IndexToOffDiagPosEqualP34();
  } else if(index < START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG) {
    index -= START_RANGE_KK_ONDIAG1;
    SETPIT(              result, index   );
    SET2POS2EQUAL(       result, index, KK_ONDIAG_POSCOUNT,   3, 4);
    SETPOS_BELOWDIAG(    result, index, 2);
    SETKK_ONDIAG(        result, index   );
    result.p34IndexToOffDiagPosEqualP34();
  } else if(index < START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG) {
    index -= START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG;
    SETPIT(              result, index   );
    SET2OFFDIAGPOSNOFLIP(result, index, KKP2_ONDIAG_POSCOUNT, 3, 4);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
  } else if(index < START_RANGE_KKP23_ONDIAG) {
    index -= START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG;
    SETPIT(              result, index   );
    SET2OFFDIAGPOSFLIPj( result, index, KKP2_ONDIAG_POSCOUNT, 3, 4);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
  } else if(index < START_RANGE_KKP234_ONDIAG) {
    index -= START_RANGE_KKP23_ONDIAG;
    SETPIT(              result, index   );
    SETPOS_BELOWDIAG(    result, index, 4);
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p23IndexToDiagPos();
  } else {
    index -= START_RANGE_KKP234_ONDIAG;
    SETPIT(              result, index   );
    SET2POS2EQUAL(       result, index, KKP2_ONDIAG_POSCOUNT, 3, 4);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
    result.p34IndexToDiagPosEqualP34();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men2Equal::getSymTransformation(const EndGameKey &key) const {
  return getSym8Transformation5Men2Equal(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition5Men2Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
    sym8PositionScanner(key, 2, allPreviousOnDiag, (PositionScanner)&EndGameKeyDefinition5Men2Equal::scanPositions);
    break;
  case 3:
    if(allPreviousOnDiag) {
      for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) {
        const int pos3 = subDiagIndexToPos[i];
        if(key.isOccupied(pos3)) {
          continue;
        }
        key.setPosition3(pos3);
        scanPositions(key, 4, IS_ONMAINDIAG1(pos3));
        key.clearField(pos3);
      }
    } else {
      for(int pos3 = 0; pos3 < 64; pos3++) {
        if(key.isOccupied(pos3)) {
          continue;
        }
        key.setPosition3(pos3);
        scanPositions(key, 4, false);
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition3();
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) {
          const int pos4 = subDiagIndexToPos[i];
          if(key.isOccupied(pos4)) {
            continue;
          }
          if(IS_ONMAINDIAG1(pos4) && (pos4 <= pos3)) {
            continue;
          }
          key.setPosition4(pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else if(!key.kingsOnMainDiag1() || !key.p2OnMainDiag1()) {
        for(int pos4 = pos3+1; pos4 < 64; pos4++) {
          if(key.isOccupied(pos4)) {
            continue;
          }
          key.setPosition4(pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else { // kings, p2 on maindiag and p3 off maindiag
        const int pi3 = offDiagPosToIndex[pos3];
        for(int pi4 = pi3+1; pi4 < ARRAYSIZE(offDiagIndexToPos); pi4++) {
          const int pos4 = offDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) {
            continue;
          }

          if(!IS_SAMESIDEMAINDIAG1(pos3, pos4)) {
            if(pi3 > pi4) {
              if(pi3 - 28 > pi4) continue;
            } else {
              if(pi3 > pi4 - 28) continue;
            }
          }
          key.setPosition4(pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition5Men2Equal::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition5Men2Equal::scanPositions);
}

String EndGameKeyDefinition5Men2Equal::getCodecName() const {
  return _T("5Men2Equal");
}

#endif
