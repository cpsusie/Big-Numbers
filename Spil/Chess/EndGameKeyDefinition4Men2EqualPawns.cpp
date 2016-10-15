#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 110

#define KK_WITH_PAWN_POSCOUNT   MAXINDEX_KK_WITH_PAWN_2MEN

#define START_RANGE_P23_QUEENSIDE    0
#define START_RANGE_P2_QUEENSIDE    (START_RANGE_P23_QUEENSIDE + GET_RANGESTART2EQUAL(KK_WITH_PAWN_POSCOUNT, 23))
#define START_RANGE_SYMMETRIC_PAWNS (START_RANGE_P2_QUEENSIDE  + GET_RANGESTART2EQUAL(KK_WITH_PAWN_POSCOUNT, 23))

EndGameKeyDefinition4Men2EqualPawns::EndGameKeyDefinition4Men2EqualPawns(PieceKey pk23) : EndGameKeyDefinition2EqualPawns(pk23) {
};

#define ENCODE_NOFLIP(key)                                                                                                        \
{ UINT pi2 = pawnPosToIndex[pos2];                                                                                                \
  UINT pi3 = pawnPosToIndex[pos3];                                                                                                \
  SORT2(pi2, pi3);                                                                                                                \
  return ADDPIT(key, ADD2EQUAL(KK_WITH_PAWN_2MEN(key), KK_WITH_PAWN_POSCOUNT, pi2, pi3))                                          \
       + START_RANGE_P23_QUEENSIDE                                                                                                \
       - MININDEX;                                                                                                                \
}

#define ENCODE_FLIPi(key, i, j)                                                                                                   \
{ UINT pi = pawnPosToIndex[MIRRORCOLUMN(pos##i)];                                                                                 \
  UINT pj = pawnPosToIndex[pos##j];                                                                                               \
  if(pi == pj) {                                                                                                                  \
    return LEFTWKK_ONE_PAWN_3MEN_INDEX(key, j)                                                                                    \
         + START_RANGE_SYMMETRIC_PAWNS                                                                                            \
         - MININDEX;                                                                                                              \
  }                                                                                                                               \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUAL(KK_WITH_PAWN_2MEN(key), KK_WITH_PAWN_POSCOUNT, pi, pj))                                            \
       + START_RANGE_P2_QUEENSIDE                                                                                                 \
       - MININDEX;                                                                                                                \
}

unsigned long EndGameKeyDefinition4Men2EqualPawns::keyToIndex(const EndGameKey &key) const {
  UINT pos2 = key.getPosition2();
  UINT pos3 = key.getPosition3();
  switch(BOOL2MASK(IS_KINGSIDE, pos2, pos3)) {
  case 0: ENCODE_NOFLIP(key  );                         // 2,3 queenside
  case 1: ENCODE_FLIPi( key,2,3);                       //   3 queenside
  case 2: ENCODE_FLIPi( key,3,2);                       // 2   queenside
  case 3: ;                                             // both kingside => error
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition4Men2EqualPawns::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;
  if(index < START_RANGE_P2_QUEENSIDE) {
    index -= START_RANGE_P23_QUEENSIDE;
    SETPIT(                result, index );
    SET2EQUALPAWNSNOFLIP(  result, index, KK_WITH_PAWN_POSCOUNT, 2, 3);
    SETKK_WITH_PAWN(       result, index );
  } else if(index < START_RANGE_SYMMETRIC_PAWNS) {
    index -= START_RANGE_P2_QUEENSIDE;
    SETPIT(                result, index );
    SET2EQUALPAWNSFLIPj(   result, index, KK_WITH_PAWN_POSCOUNT, 2, 3);
    SETKK_WITH_PAWN(       result, index );
  } else {
    index -= START_RANGE_SYMMETRIC_PAWNS;
    SETPIT(                result, index );
    SET2SYMMETRICPAWNS(    result, index, 1, 2, 3);
    SETLEFTWKK_WITH_PAWN(  result, index);
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition4Men2EqualPawns::getSymTransformation(const EndGameKey &key) const {
  return get4Men2EqualPawnsSymTransformation(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition4Men2EqualPawns::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const int pawnStartRow = GETPAWNSTARTROW(getPawnsOwner());
  const int pawnRow2     = (getPawnsOwner() == WHITEPLAYER) ? (pawnStartRow+1) : (pawnStartRow-1);
  for(int pawn1Col = 0; pawn1Col < 8; pawn1Col++) {
    const int pos2 = MAKE_POSITION(pawnStartRow, pawn1Col);
    for(int pawn2Col = 0; pawn2Col < 8; pawn2Col++) {
      int pos3 = MAKE_POSITION(pawnStartRow, pawn2Col);
      if(pos3 == pos2) {
        pos3 = MAKE_POSITION(pawnRow2, pawn2Col);
      }
      tablebase.addInitPosition(EndGameKey(*this, WHITEPLAYER, wkPos, bkPos, pos2, pos3), true);
    }
  }
}

void EndGameKeyDefinition4Men2EqualPawns::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const {
  switch(pIndex) {
  case 0:
    { for(int pos = 0; pos < 64; pos++) {
        key.setPosition(pIndex, pos);
        scanPositions(key, 1);
        key.clearField(pos);
      }
    }
    break;
  case 1:
    { const int wkPos = key.getWhiteKingPosition();
      for(int pos = 0; pos < 64; pos++) {
        if((KINGSADJACENT(wkPos, pos)) || key.isOccupied(pos)) {
          continue;
        }
        key.setPosition(pIndex, pos);
        scanPositions(key, 2);
        key.clearField(pos);
      }
    }
    break;
  case 2:
    { for(int i = 0; i < PAWN1_POSCOUNT; i++) {
        const int pos2 = pawnIndexToPos[i];
        if(key.isOccupied(pos2)) {
          continue;
        }
        key.setPosition2(pos2);
        scanPositions(key, 3);
        key.clearField(pos2);
      }
    }
    break;
  case 3:
    { const UINT pi2 = pawnPosToIndex[key.getPosition2()];                  // p2 always on queen side
      for(UINT i = 0; i < PAWN_POSCOUNT; i++) {
        const int pos3 = pawnIndexToPos[i];
        if(key.isOccupied(pos3)) {
          continue;
        }
        if(IS_QUEENSIDE(pos3)) {
          if(pi2 > i) {
            continue;
          }
        } else {
          const UINT pi3 = pawnPosToIndex[MIRRORCOLUMN(pos3)];
          if(pi2 > pi3) {
            continue;
          } else if((pi2 == pi3) && IS_KINGSIDE(key.getWhiteKingPosition())) {
            continue;
          }
        }
        key.setPosition3(pos3);
        checkForBothPlayers(key);
        key.clearField(pos3);
      }
    }
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition4Men2EqualPawns::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  scanPositions(key, 0);
}

String EndGameKeyDefinition4Men2EqualPawns::getCodecName() const {
  return _T("4Men2EqualPawns");
}

#endif
