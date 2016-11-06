#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

#define KK_OFFDIAG_POSCOUNT                    (MAXINDEX_KK_OFFDIAG_4MEN     - MININDEX_KK_OFFDIAG_4MEN    )
#define KK_ONDIAG_POSCOUNT                     (MAXINDEX_KK_ONDIAG_4MEN      - MININDEX_KK_ONDIAG_4MEN     )
#define KKP2_ONDIAG_POSCOUNT                   (MAXINDEX_KKP2_ONDIAG_4MEN    - MININDEX_KKP2_ONDIAG_4MEN   )
#define KKP23_ONDIAG_POSCOUNT                  (MAXINDEX_KKP23_ONDIAG_4MEN   - MININDEX_KKP23_ONDIAG_4MEN  )
#define KKP234_ONDIAG_POSCOUNT                 (MAXINDEX_KKP234_ONDIAG_6MEN  - MININDEX_KKP234_ONDIAG_6MEN )

#define START_RANGE_KK_ONDIAG1                 (                                         GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT  ,59))
#define START_RANGE_KKP2_ONDIAG1               (START_RANGE_KK_ONDIAG1                 + GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT   ,59))
#define START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG (START_RANGE_KKP2_ONDIAG1               + GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT ,59))
#define START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG  (START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG + GET_RANGESTART2EQUAL(KKP23_ONDIAG_POSCOUNT,27))
#define START_RANGE_KKP234_ONDIAG              (START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG  + GET_RANGESTART2EQUAL(KKP23_ONDIAG_POSCOUNT,28))
#define START_RANGE_KKP2345_ONDIAG             (START_RANGE_KKP234_ONDIAG              + KKP234_ONDIAG_POSCOUNT )

EndGameKeyDefinition6Men2Equal::EndGameKeyDefinition6Men2Equal(PieceKey pk2, PieceKey pk3, PieceKey pk45)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk3, pk45)
{
  assert((pk2 != pk3) && (pk2 != pk45) && (pk3 != pk45)
      && (GET_TYPE_FROMKEY(pk45) != Pawn  )  // for pk45 = Pawn  : use EndGameKeyDefinition6Men2Pawns
      && (GET_TYPE_FROMKEY(pk2 ) != Pawn  )  // for pk2  = Pawn  : use EndGameKeyDefinition5Men1Pawn2Equal
      && (GET_TYPE_FROMKEY(pk3 ) != Pawn  )  // for pk2  = Pawn  : use EndGameKeyDefinition5Men1Pawn2Equal
        );

/*
  DUMP_MACRO(     KK_OFFDIAG_POSCOUNT                        );
  DUMP_MACRO(     KK_ONDIAG_POSCOUNT                         );
  DUMP_MACRO(     KKP2_ONDIAG_POSCOUNT                       );
  DUMP_MACRO(     KKP23_ONDIAG_POSCOUNT                      );
  DUMP_MACRO(     KKP234_ONDIAG_POSCOUNT                     );

  DUMP_MACRO(     START_RANGE_KK_ONDIAG1                     );
  DUMP_MACRO(     START_RANGE_KKP2_ONDIAG1                   );
  DUMP_MACRO(     START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG     );
  DUMP_MACRO(     START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG      );
  DUMP_MACRO(     START_RANGE_KKP234_ONDIAG                  );
  DUMP_MACRO(     START_RANGE_KKP2345_ONDIAG                 );
*/
}

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos4 > pos5) {                                                                                                               \
    tmp.setPosition(4,pos5);                                                                                                       \
    tmp.setPosition(5,pos4);                                                                                                       \
  }                                                                                                                               \
  const UINT pi4 = tmp.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = tmp.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_OFFDIAG_4MEN(tmp), KK_OFFDIAG_POSCOUNT, pi4, pi5))                                              \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_OFFDIAG(key, index)                                                                                             \
{ SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT,  4, 5);                                                                   \
  SETP3_INDEX(         key, index   );                                                                                            \
  SETP2_INDEX(         key, index   );                                                                                            \
  SETKK_OFFDIAG(       key, index   );                                                                                            \
  key.p23IndexToOffDiagPos();                                                                                                     \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KK_ONDIAG(key)                                                                                                     \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos4 > pos5) {                                                                                                               \
    tmp.setPosition(4,pos5);                                                                                                      \
    tmp.setPosition(5,pos4);                                                                                                      \
  }                                                                                                                               \
  const UINT pi4 = tmp.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = tmp.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KK_ONDIAG_4MEN(tmp), KK_ONDIAG_POSCOUNT, pi4, pi5))                                                \
       + START_RANGE_KK_ONDIAG1                                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG(key, index)                                                                                              \
{ index -= START_RANGE_KK_ONDIAG1;                                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT,   4, 5);                                                                   \
  SETP3_INDEX(         key, index   );                                                                                            \
  SETPOS_BELOWDIAG(    key, index, 2);                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p3IndexToOffDiagPos();                                                                                                      \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KKP2_ONDIAG(key)                                                                                                   \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos4 > pos5) {                                                                                                               \
    tmp.setPosition(4,pos5);                                                                                                      \
    tmp.setPosition(5,pos4);                                                                                                      \
  }                                                                                                                               \
  const UINT pi4 = tmp.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = tmp.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(tmp, ADD2EQUAL(KKP2_ONDIAG_4MEN(tmp), KKP2_ONDIAG_POSCOUNT, pi4, pi5))                                            \
       + START_RANGE_KKP2_ONDIAG1                                                                                                 \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG(key, index)                                                                                            \
{ index -= START_RANGE_KKP2_ONDIAG1;                                                                                              \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP2_ONDIAG_POSCOUNT, 4, 5);                                                                   \
  SETPOS_BELOWDIAG(    key, index, 3);                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KKP23_ONDIAG_NOFLIP(key)                                                                                           \
{ UINT pi4 = s_offDiagPosToIndex[pos4];                                                                                           \
  UINT pi5 = s_offDiagPosToIndex[pos5];                                                                                           \
  SORT2(pi4,pi5);                                                                                                                 \
  return ADDPIT(key, ADD2EQUAL(KKP23_ONDIAG_4MEN(key), KKP23_ONDIAG_POSCOUNT, pi4, pi5))                                          \
       + START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_NOFLIP(key, index)                                                                                    \
{ index -= START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG;                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSNOFLIP(key, index, KKP23_ONDIAG_POSCOUNT, 4, 5);                                                                  \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
}

#define ENCODE_KKP23_ONDIAG_FLIPi(key,i,j)                                                                                        \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi,pj);                                                                                                                   \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP23_ONDIAG_4MEN(key), KKP23_ONDIAG_POSCOUNT, pi, pj))                                \
       + START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_FLIPi(key, index)                                                                                     \
{ index -= START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG;                                                                                 \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSFLIPj( key, index, KKP23_ONDIAG_POSCOUNT, 4, 5);                                                                  \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
}

#define ENCODE_KKP234_ONDIAG(key)  return (KKP234_ONDIAG_6MEN_INDEX(key) + START_RANGE_KKP234_ONDIAG - MININDEX);

#define ENCODE_KKP235_ONDIAG(key)                                                                                                 \
{ EndGameKey tmp = key;                                                                                                           \
  tmp.setPosition(4,pos5);                                                                                                        \
  tmp.setPosition(5,pos4);                                                                                                        \
  ENCODE_KKP234_ONDIAG(tmp);                                                                                                      \
}

#define DECODE_KKP234_ONDIAG(key, index)                                                                                          \
{ index -= START_RANGE_KKP234_ONDIAG;                                                                                             \
  SETPIT(              key, index   );                                                                                            \
  SETPOS_BELOWDIAG(    key, index, 5);                                                                                            \
  SETP4_ONDIAG(        key, index   );                                                                                            \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p234IndexToDiagPos();                                                                                                       \
}

#define ENCODE_KKP2345_ONDIAG(key)                                                                                                \
{ EndGameKey tmp = key;                                                                                                           \
  if(pos4 > pos5) {                                                                                                               \
    tmp.setPosition(4,pos5);                                                                                                      \
    tmp.setPosition(5,pos4);                                                                                                      \
  }                                                                                                                               \
  const UINT pi4 = tmp.getP4DiagIndex();                                                                                          \
  const UINT pi5 = tmp.getP5DiagIndexEqualP45();                                                                                  \
  return ADDPIT(tmp, ADD2EQUAL(KKP23_ONDIAG_4MEN(tmp), KKP23_ONDIAG_POSCOUNT, pi4, pi5))                                          \
       + START_RANGE_KKP2345_ONDIAG                                                                                               \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2345_ONDIAG(key, index)                                                                                         \
{ index -= START_RANGE_KKP2345_ONDIAG;                                                                                            \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP23_ONDIAG_POSCOUNT, 4, 5);                                                                  \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
  key.p45IndexToDiagPosEqualP45();                                                                                                \
}

EndGamePosIndex EndGameKeyDefinition6Men2Equal::keyToIndex(EndGameKey key) const {
  const UINT pos4 = key.getPosition(4);
  const UINT pos5 = key.getPosition(5);

  if(!key.kingsOnMainDiag1()) {                                // Kings off maindiag => p2, p3, p4, p5 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else if(!key.p2OnMainDiag1()) {                            // Kings on maindiag, p2 below, p3, p4, p5 anywhere
    ENCODE_KK_ONDIAG(key)
  } else if(!key.p3OnMainDiag1()) {                            // Kings, p2 on maindiag, p3 below, p4, p5 anywhere
    ENCODE_KKP2_ONDIAG(key)
  } else {                                                     // Kings, p2, p3 on maindiag
    switch(BOOL2MASK(IS_OFFMAINDIAG1, pos4, pos5)) {
    case 0: ENCODE_KKP2345_ONDIAG(         key  );             // 4,5   on    diag
    case 1: ENCODE_KKP235_ONDIAG(          key  );             //   5   on    diag
    case 2: ENCODE_KKP234_ONDIAG(          key  );             // 4     on    diag
    case 3:
      { switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos4, pos5)) {
        case 0: ENCODE_KKP23_ONDIAG_NOFLIP(key    );           // 4,5   below diag
        case 1: ENCODE_KKP23_ONDIAG_FLIPi( key,4,5);           //   5   below diag
        case 2: ENCODE_KKP23_ONDIAG_FLIPi( key,5,4);           // 4     below diag
        case 3: ;                                              // none  below diag => error
        }
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition6Men2Equal::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG1) {                         // Kings off diag, p2, p3, p4, p5 anywhere
    DECODE_KK_OFFDIAG(result, index)
  } else if(index < START_RANGE_KKP2_ONDIAG1) {                // Kings on diag, p2 below, p3, p4, p5 anywhere
    DECODE_KK_ONDIAG(result, index)
  } else if(index < START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG) {  // Kings, p2 on diag, p3 below, p4, p5 anywhere
    DECODE_KKP2_ONDIAG(result, index)
  } else if(index < START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG) {   // Kings, p2, p3 on diag, p4, p5 below
    DECODE_KKP23_ONDIAG_NOFLIP(result, index)
  } else if(index < START_RANGE_KKP234_ONDIAG) {               // Kings, p2, p3 on diag, p4 below, p5 above
    DECODE_KKP23_ONDIAG_FLIPi(result, index)
  } else if(index < START_RANGE_KKP2345_ONDIAG) {              // Kings, p2, p3, p4 on diag, p5 below
    DECODE_KKP234_ONDIAG(result, index)
  } else {                                                     // Kings, p2, p3, p4, p5 ondiag
    DECODE_KKP2345_ONDIAG(result, index)
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition6Men2Equal::getSymTransformation(EndGameKey key) const {
  return getSym8Transformation6Men2Equal(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition6Men2Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
  case 3:
    sym8PositionScanner(key, pIndex, allPreviousOnDiag, (PositionScanner)&EndGameKeyDefinition6Men2Equal::scanPositions);
    break;
  case 4:
    if(allPreviousOnDiag) {
      for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
        const int pos4 = s_subDiagIndexToPos[i];
        if(key.isOccupied(pos4)) continue;
        key.setPosition(4,pos4);
        scanPositions(key, 5, IS_ONMAINDIAG1(pos4));
        key.clearField(pos4);
      }
    } else {
      for(int pos4 = 0; pos4 < 64; pos4++) {
        if(key.isOccupied(pos4)) continue;
        key.setPosition(4,pos4);
        scanPositions(key, 5, false);
        key.clearField(pos4);
      }
    }
    break;
  case 5:
    { const int pos4 = key.getPosition(4);
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
          const int pos5 = s_subDiagIndexToPos[i];
          if(key.isOccupied(pos5)) continue;
          if(IS_ONMAINDIAG1(pos5) && (pos5 <= pos4)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else if(!key.kingsOnMainDiag1() || !key.p2OnMainDiag1() || !key.p3OnMainDiag1()) {
        for(int pos5 = pos4+1; pos5 < 64; pos5++) {
          if(key.isOccupied(pos5)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else { // kings, p2 on maindiag and p3 off maindiag
        const int pi4 = s_offDiagPosToIndex[pos4];
        for(int pi5 = pi4+1; pi5 < ARRAYSIZE(s_offDiagIndexToPos); pi5++) {
          const int pos5 = s_offDiagIndexToPos[pi5];
          if(key.isOccupied(pos5)) continue;

          if(!IS_SAMESIDEMAINDIAG1(pos4, pos5)) {
            if(pi4 > pi5) {
              if(pi4 - 28 > pi5) continue;
            } else {
              if(pi4 > pi5 - 28) continue;
            }
          }
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition6Men2Equal::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition6Men2Equal::scanPositions);
}

bool EndGameKeyDefinition6Men2Equal::keysEqual(EndGameKey key1, EndGameKey key2) const {
  if(key2 == key1) return true;
  key2.swapPos(4,5);
  return key2 == key1;
}

String EndGameKeyDefinition6Men2Equal::getCodecName() const {
  return _T("6Men2Equal");
}

#endif
