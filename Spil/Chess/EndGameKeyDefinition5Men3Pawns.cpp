#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 2646

EndGameKeyDefinition5Men3Pawns::EndGameKeyDefinition5Men3Pawns(PieceKey pk2, PieceKey pk34)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk34)
{
#ifdef TABLEBASE_BUILDER
  setPawnOwners();
#endif
}

SymmetricTransformation EndGameKeyDefinition5Men3Pawns::getSymTransformation(const EndGameKey &key) const {
  return getPawnSymTransformation(key);
}

EndGamePosIndex EndGameKeyDefinition5Men3Pawns::keyToIndex(const EndGameKey &key) const {
  UINT pi3 = key.getP3Pawn2Index()         - 8;
  UINT pi4 = key.getP4Pawn3IndexEqualP34() - 8;

  SORT2(pi3,pi4);
  return ADDPIT(key, ADD2EQUAL(ONE_PAWN_3MEN(key, 2), KK_WITH_PAWN_POSCOUNT_3MEN, pi3, pi4))
       - MININDEX;
}

EndGameKey EndGameKeyDefinition5Men3Pawns::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;
  EndGameKey result;
  SETPIT(               result, index  );
  SET2POS2EQUAL(        result, index, KK_WITH_PAWN_POSCOUNT_3MEN, 3, 4);
  SETPAWN1POS(          result, index, 2);
  SETKK_WITH_PAWN(      result, index  );
  result.p34IndexToPawn23PosEqualP34();
  return result;
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition5Men3Pawns::setPawnOwners() {
  for(int i = 2; i < 5; i++) {
    checkIsPawn(i, true);
    m_pawnOwner.setOwner(i-2, getPieceOwner(i));
  }
  checkSameOwner(2, 3, false);
  checkSameOwner(3, 4, true );
}

void EndGameKeyDefinition5Men3Pawns::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const int pawn1StartRow = GETPAWNSTARTROW(m_pawnOwner.getOwner(0));
  const int pawn2StartRow = GETPAWNSTARTROW(m_pawnOwner.getOwner(1));
  const int pawn3StartRow = GETPAWNSTARTROW(m_pawnOwner.getOwner(2));
  const int pawn3Row2     = (m_pawnOwner.getOwner(2) == WHITEPLAYER) ? (pawn3StartRow+1) : (pawn3StartRow-1);
  for(int col1 = 0; col1 < 4; col1++) {
    const int pawn1Pos = MAKE_POSITION(pawn1StartRow, col1);
    for(int col2 = 0; col2 < 8; col2++) {
      const int pawn2Pos = MAKE_POSITION(pawn2StartRow, col2);
      for(int col3 = 0; col3 < 8; col3++) {
        int pawn3Pos = MAKE_POSITION(pawn3StartRow, col3);
        if(pawn3Pos == pawn2Pos) {
          pawn3Pos = MAKE_POSITION(pawn3Row2, col3);
        }
        tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos, pawn3Pos);
      }
    }
  }
}

void EndGameKeyDefinition5Men3Pawns::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const {
  switch(pIndex) {
  case 0:
  case 1:
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
    { for(int i = 0; i < PAWN_POSCOUNT; i++) {
        const int pos3 = pawnIndexToPos[i];
        if(key.isOccupied(pos3)) {
          continue;
        }
        key.setPosition3(pos3);
        scanPositions(key, 4);
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition3();
      for(int i = 0; i < PAWN_POSCOUNT; i++) {
        const int pos4 = pawnIndexToPos[i];
        if(pos3 > pos4) {
          continue;
        }
        if(key.isOccupied(pos4)) {
          continue;
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

void EndGameKeyDefinition5Men3Pawns::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  scanPositions(key, 0);
}

String EndGameKeyDefinition5Men3Pawns::getCodecName() const {
  return _T("5Men3Pawns");
}

#endif
