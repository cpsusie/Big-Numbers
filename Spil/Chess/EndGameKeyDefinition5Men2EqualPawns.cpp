#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 6820

#define KKP2_WITH_PAWN_POSCOUNT                   (MAXINDEX_KK_WITH_PAWN_2MEN * P2POSCOUNT)
#define KKP2_WITH_PAWN(          key)             (KK_WITH_PAWN_2MEN(key) * P2POSCOUNT + key.getP2OffDiagIndex())
#define KKP2_WITH_PAWN_INDEX(    key)             ADDPIT(key, KKP2_WITH_PAWN(key))

#ifdef _DEBUG

static int leftWkkP2WithPawnIndex(EndGameKey key, int pawnIndex) {
  int result = LEFTWKK_ONE_PAWN_3MEN(key, pawnIndex);
  const int p2PosCount = P2POSCOUNT;
  result *= P2POSCOUNT;
  result += key.getP2OffDiagIndex();
  result *= 2;
  result |= key.getPlayerInTurn();
  return result;
}

#define LEFTWKKP2_WITH_PAWN_INDEX(key, pawnIndex) leftWkkP2WithPawnIndex(key, pawnIndex)

#else

#define LEFTWKKP2_WITH_PAWN(      key, pawnIndex) (LEFTWKK_ONE_PAWN_3MEN(key, pawnIndex) * P2POSCOUNT + key.getP2OffDiagIndex())
#define LEFTWKKP2_WITH_PAWN_INDEX(key, pawnIndex) ADDPIT(key, LEFTWKKP2_WITH_PAWN(key, pawnIndex))

#endif

#define START_RANGE_P34_QUEENSIDE    0
#define START_RANGE_P3_QUEENSIDE    (START_RANGE_P34_QUEENSIDE + GET_RANGESTART2EQUAL(KKP2_WITH_PAWN_POSCOUNT, 23))
#define START_RANGE_SYMMETRIC_PAWNS (START_RANGE_P3_QUEENSIDE  + GET_RANGESTART2EQUAL(KKP2_WITH_PAWN_POSCOUNT, 23))


EndGameKeyDefinition5Men2EqualPawns::EndGameKeyDefinition5Men2EqualPawns(PieceKey pk2, PieceKey pk34) : EndGameKeyDefinition2EqualPawns(pk2, pk34) {
}

#define ENCODE_NOFLIP(key)                                                                                                        \
{ UINT pi3 = pawnPosToIndex[pos3];                                                                                                \
  UINT pi4 = pawnPosToIndex[pos4];                                                                                                \
  SORT2(pi3, pi4);                                                                                                                \
  return ADDPIT(key, ADD2EQUAL(KKP2_WITH_PAWN(key), KKP2_WITH_PAWN_POSCOUNT, pi3, pi4))                                           \
       + START_RANGE_P34_QUEENSIDE                                                                                                \
       - MININDEX;                                                                                                                \
}

#define ENCODE_FLIPi(key, i, j)                                                                                                   \
{ UINT pi = pawnPosToIndex[MIRRORCOLUMN(pos##i)];                                                                                 \
  UINT pj = pawnPosToIndex[pos##j];                                                                                               \
  if(pi == pj) {                                                                                                                  \
    return LEFTWKKP2_WITH_PAWN_INDEX(key, j)                                                                                      \
         + START_RANGE_SYMMETRIC_PAWNS                                                                                            \
         - MININDEX;                                                                                                              \
                                                                                                                                  \
  }                                                                                                                               \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUAL(KKP2_WITH_PAWN(key), KKP2_WITH_PAWN_POSCOUNT, pi, pj))                                             \
       + START_RANGE_P3_QUEENSIDE                                                                                                 \
       - MININDEX;                                                                                                                \
}

unsigned long EndGameKeyDefinition5Men2EqualPawns::keyToIndex(const EndGameKey &key) const {
  UINT pos3 = key.getPosition3();
  UINT pos4 = key.getPosition4();
  switch(BOOL2MASK(IS_KINGSIDE, pos3, pos4)) {
  case 0: ENCODE_NOFLIP(key      );                     // 3,4 queenside
  case 1: ENCODE_FLIPi( key, 3, 4);                     //   4 queenside
  case 2: ENCODE_FLIPi( key, 4, 3);                     // 3   queenside
  case 3: ;                                             // both kingside => error
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition5Men2EqualPawns::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;
  if(index < START_RANGE_P3_QUEENSIDE) {
    index -= START_RANGE_P34_QUEENSIDE;
    SETPIT(                result, index  );
    SET2EQUALPAWNSNOFLIP(  result, index, KKP2_WITH_PAWN_POSCOUNT, 3, 4);
    SETP2_INDEX(           result, index  );
    SETKK_WITH_PAWN(       result, index  );
    result.p2IndexToOffDiagPos();
  } else if(index < START_RANGE_SYMMETRIC_PAWNS) {
    index -= START_RANGE_P3_QUEENSIDE;
    SETPIT(                result, index  );
    SET2EQUALPAWNSFLIPj(   result, index, KKP2_WITH_PAWN_POSCOUNT, 3, 4);
    SETP2_INDEX(           result, index  );
    SETKK_WITH_PAWN(       result, index  );
    result.p2IndexToOffDiagPos();
  } else {
    index -= START_RANGE_SYMMETRIC_PAWNS;
    SETPIT(                result, index  );
    SETP2_INDEX(           result, index  );
    SET2SYMMETRICPAWNS(    result, index, 1, 3, 4);
    SETLEFTWKK_WITH_PAWN(  result, index);
    result.p2IndexToOffDiagPos();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men2EqualPawns::getSymTransformation(const EndGameKey &key) const {
  return get5Men2EqualPawnsSymTransformation(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition5Men2EqualPawns::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const int pawnStartRow = GETPAWNSTARTROW(getPawnsOwner());
  const int pawnRow2     = (getPawnsOwner() == WHITEPLAYER) ? (pawnStartRow+1) : (pawnStartRow-1);
  for(int pawn1Col = 0; pawn1Col < 8; pawn1Col++) {
    const int pos3 = MAKE_POSITION(pawnStartRow, pawn1Col);
    for(int pawn2Col = 0; pawn2Col < 8; pawn2Col++) {
      int pos4 = MAKE_POSITION(pawnStartRow, pawn2Col);
      if(pos4 == pos3) {
        pos4 = MAKE_POSITION(pawnRow2, pawn2Col);
      }
      tablebase.addInitPosition(EndGameKey(*this, WHITEPLAYER, wkPos, bkPos, G4, pos3, pos4), true);
      tablebase.addInitPosition(EndGameKey(*this, WHITEPLAYER, wkPos, bkPos, H4, pos3, pos4), true);
    }
  }
}

void EndGameKeyDefinition5Men2EqualPawns::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const {
  switch(pIndex) {
  case 0:
  case 1:
  case 2:
    { for(int pos = 0; pos < 64; pos++) {
        if(((pIndex == 1) && KINGSADJACENT(key.getWhiteKingPosition(), pos)) || key.isOccupied(pos)) {
          continue;
        }
        key.setPosition(pIndex, pos);
        scanPositions(key, pIndex+1);
        key.clearField(pos);
      }
    }
    break;
  case 3:
    { for(int i = 0; i < PAWN1_POSCOUNT; i++) {
        const int pos3 = pawnIndexToPos[i];
        if(key.isOccupied(pos3)) {
          continue;
        }
        key.setPosition3(pos3);
        scanPositions(key, pIndex+1);
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const UINT pi3 = pawnPosToIndex[key.getPosition3()];                  // p3 always on queen side
      for(int pi4 = 0; pi4 < PAWN_POSCOUNT; pi4++) {
        const int pos4 = pawnIndexToPos[pi4];
        if(key.isOccupied(pos4)) {
          continue;
        }
        if(IS_QUEENSIDE(pos4)) {
          if(pi3 > pi4) {
            continue;
          }
        } else {
          const UINT pi4m = pawnPosToIndex[MIRRORCOLUMN(pos4)];
          if(pi3 > pi4m) {
            continue;
          } else if((pi3 == pi4m) && IS_KINGSIDE(key.getWhiteKingPosition())) {
            continue;
          }
        }
        key.setPosition4(pos4);
        checkForBothPlayers(key);
        key.clearField(pos4);
      }
    }
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition5Men2EqualPawns::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  scanPositions(key, 0);
}

String EndGameKeyDefinition5Men2EqualPawns::getCodecName() const {
  return _T("5Men2EqualPawns");
}

#endif
