#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

#define KK_OFFDIAG_POSCOUNT                    (MAXINDEX_KK_OFFDIAG_3MEN     - MININDEX_KK_OFFDIAG_3MEN    )
#define KK_ONDIAG_POSCOUNT                     (MAXINDEX_KK_ONDIAG_3MEN      - MININDEX_KK_ONDIAG_3MEN     )
#define KKP2_ONDIAG_POSCOUNT                   (MAXINDEX_KKP2_ONDIAG_3MEN    - MININDEX_KKP2_ONDIAG_3MEN   )
#define KKP23_ONDIAG_POSCOUNT                  (MAXINDEX_KKP23_ONDIAG_5MEN   - MININDEX_KKP23_ONDIAG_5MEN  )



#define START_RANGE_KK_ONDIAG1                 (                                         GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT ,60))
#define START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG  (START_RANGE_KK_ONDIAG1                 + GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT  ,60))
#define START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG   (START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG  + GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT,27))
#define START_RANGE_KKP23_ONDIAG               (START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG   + GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT,28))
#define START_RANGE_KKP234_ONDIAG              (START_RANGE_KKP23_ONDIAG               + KKP23_ONDIAG_POSCOUNT)

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
    tmp.setPosition(3,pos4);                                                                                                       \
    tmp.setPosition(4,pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = tmp.getP4OffDiagIndexEqualP34();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_OFFDIAG_3MEN(tmp), KK_OFFDIAG_POSCOUNT, pi3, pi4))                                              \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_OFFDIAG(key, index)                                                                                             \
{ SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT,  3, 4);                                                                   \
  SETP2_INDEX(         key, index   );                                                                                            \
  SETKK_OFFDIAG(       key, index   );                                                                                            \
  key.p2IndexToOffDiagPos();                                                                                                      \
  key.p34IndexToOffDiagPosEqualP34();                                                                                             \
}

#define ENCODE_KK_ONDIAG(key)                                                                                                     \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos3 > pos4) {                                                                                                               \
    tmp.setPosition(3,pos4);                                                                                                       \
    tmp.setPosition(4,pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = tmp.getP4OffDiagIndexEqualP34();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_ONDIAG_3MEN(tmp), KK_ONDIAG_POSCOUNT, pi3, pi4))                                                \
       + START_RANGE_KK_ONDIAG1                                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG(key, index)                                                                                              \
{ index -= START_RANGE_KK_ONDIAG1;                                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT, 3, 4);                                                                     \
  SETPOS_BELOWDIAG(    key, index, 2);                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p34IndexToOffDiagPosEqualP34();                                                                                             \
}

#define ENCODE_KKP2_ONDIAG_NOFLIP(key)                                                                                            \
{ UINT pi3 = s_offDiagPosToIndex[pos3];                                                                                           \
  UINT pi4 = s_offDiagPosToIndex[pos4];                                                                                           \
  SORT2(pi3,pi4);                                                                                                                 \
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi3, pi4))                                            \
       + START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_NOFLIP(key, index)                                                                                     \
{ index -= START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG;                                                                                 \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSNOFLIP(key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);                                                                   \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2_ONDIAG_FLIPi(key,i,j)                                                                                         \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi,pj);                                                                                                                   \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))                                  \
       + START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG                                                                                     \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_FLIPi(key, index)                                                                                      \
{ index -= START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG;                                                                                  \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSFLIPj( key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);                                                                   \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP23_ONDIAG(key)  return (KKP23_ONDIAG_5MEN_INDEX(key) + START_RANGE_KKP23_ONDIAG - MININDEX);

#define ENCODE_KKP24_ONDIAG(key)                                                                                                  \
{ EndGameKey tmp = key;                                                                                                           \
  tmp.setPosition(3,pos4);                                                                                                         \
  tmp.setPosition(4,pos3);                                                                                                         \
  ENCODE_KKP23_ONDIAG(tmp);                                                                                                       \
}

#define DECODE_KKP23_ONDIAG(key, index)                                                                                           \
{ index -= START_RANGE_KKP23_ONDIAG;                                                                                              \
  SETPIT(              key, index   );                                                                                            \
  SETPOS_BELOWDIAG(    key, index, 4);                                                                                            \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
}

#define ENCODE_KKP234_ONDIAG(key)                                                                                                 \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos3 > pos4) {                                                                                                               \
    tmp.setPosition(3,pos4);                                                                                                       \
    tmp.setPosition(4,pos3);                                                                                                       \
  }                                                                                                                               \
  const UINT pi3 = tmp.getP3DiagIndex();                                                                                          \
  const UINT pi4 = tmp.getP4DiagIndexEqualP34();                                                                                  \
  return ADDPIT(tmp, ADD2EQUAL(KKP2_ONDIAG_3MEN(tmp), KKP2_ONDIAG_POSCOUNT, pi3, pi4))                                            \
       + START_RANGE_KKP234_ONDIAG                                                                                                \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP234_ONDIAG(key, index)                                                                                          \
{ index -= START_RANGE_KKP234_ONDIAG;                                                                                             \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);                                                                   \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
  key.p34IndexToDiagPosEqualP34();                                                                                                \
}

EndGamePosIndex EndGameKeyDefinition5Men2Equal::keyToIndex(EndGameKey key) const {
  const UINT pos3 = key.getPosition(3);
  const UINT pos4 = key.getPosition(4);

  if(!key.kingsOnMainDiag1()) {                                // Kings off maindiag => p2, p3, p4 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else if(!key.p2OnMainDiag1()) {                            // Kings on maindiag, p2 below, p3, p4 anywhere
    ENCODE_KK_ONDIAG(key)
  } else {                                                     // Kings, p2 on mandiag
    switch(BOOL2MASK(IS_OFFMAINDIAG1, pos3, pos4)) {
    case 0: ENCODE_KKP234_ONDIAG(          key  );             // 3,4   on    diag
    case 1: ENCODE_KKP24_ONDIAG(           key  );             //   4   on    diag
    case 2: ENCODE_KKP23_ONDIAG(           key  );             // 3     on    diag
    case 3:
      { switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos3, pos4)) {
        case 0: ENCODE_KKP2_ONDIAG_NOFLIP(key    );            // 3,4   below diag
        case 1: ENCODE_KKP2_ONDIAG_FLIPi( key,3,4);            //   4   below diag
        case 2: ENCODE_KKP2_ONDIAG_FLIPi( key,4,3);            // 3     below diag
        case 3: ;                                              // none  below diag => error
        }
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition5Men2Equal::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG1) {
    DECODE_KK_OFFDIAG(result, index)
  } else if(index < START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG) {
    DECODE_KK_ONDIAG(result, index)
  } else if(index < START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_NOFLIP(result, index)
  } else if(index < START_RANGE_KKP23_ONDIAG) {
    DECODE_KKP2_ONDIAG_FLIPi(result, index)
  } else if(index < START_RANGE_KKP234_ONDIAG) {
    DECODE_KKP23_ONDIAG(result, index)
  } else {
    DECODE_KKP234_ONDIAG(result, index)
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men2Equal::getSymTransformation(EndGameKey key) const {
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
      for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
        const int pos3 = s_subDiagIndexToPos[i];
        if(key.isOccupied(pos3)) continue;
        key.setPosition(3,pos3);
        scanPositions(key, 4, IS_ONMAINDIAG1(pos3));
        key.clearField(pos3);
      }
    } else {
      for(int pos3 = 0; pos3 < 64; pos3++) {
        if(key.isOccupied(pos3)) continue;
        key.setPosition(3,pos3);
        scanPositions(key, 4, false);
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition(3);
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
          const int pos4 = s_subDiagIndexToPos[i];
          if(key.isOccupied(pos4)) continue;
          if(IS_ONMAINDIAG1(pos4) && (pos4 <= pos3)) continue;
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else if(!key.kingsOnMainDiag1() || !key.p2OnMainDiag1()) {
        for(int pos4 = pos3+1; pos4 < 64; pos4++) {
          if(key.isOccupied(pos4)) continue;
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else { // kings, p2 on maindiag and p3 off maindiag
        const int pi3 = s_offDiagPosToIndex[pos3];
        for(int pi4 = pi3+1; pi4 < ARRAYSIZE(s_offDiagIndexToPos); pi4++) {
          const int pos4 = s_offDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;

          if(!IS_SAMESIDEMAINDIAG1(pos3, pos4)) {
            if(pi3 > pi4) {
              if(pi3 - 28 > pi4) continue;
            } else {
              if(pi3 > pi4 - 28) continue;
            }
          }
          key.setPosition(4,pos4);
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

void EndGameKeyDefinition5Men2Equal::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition5Men2Equal::scanPositions);
}

bool EndGameKeyDefinition5Men2Equal::keysEqual(EndGameKey key1, EndGameKey key2) const {
  if(key2 == key1) return true;
  key2.swapPos(3,4);
  return key2 == key1;
}

String EndGameKeyDefinition5Men2Equal::getCodecName() const {
  return _T("5Men2Equal");
}

#endif
