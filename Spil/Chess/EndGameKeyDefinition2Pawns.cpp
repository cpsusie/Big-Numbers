#include "stdafx.h"
#include "EndGameUtil.h"

EndGameKeyDefinition2Pawns::EndGameKeyDefinition2Pawns(PieceKey pk2, PieceKey pk3)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3) {
  assert(pk2 != pk3);
#if defined(TABLEBASE_BUILDER)
  setPawnOwners();
#endif
}

EndGameKeyDefinition2Pawns::EndGameKeyDefinition2Pawns(PieceKey pk2, PieceKey pk3, PieceKey pk4)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3, pk4)
{
#if defined(TABLEBASE_BUILDER)
  setPawnOwners();
#endif
}

SymmetricTransformation EndGameKeyDefinition2Pawns::getSymTransformation(EndGameKey key) const {
  return getPawnSymTransformation(key);
}

#if defined(TABLEBASE_BUILDER)

void EndGameKeyDefinition2Pawns::setPawnOwners() {
  checkIsPawn(2, true);
  checkIsPawn(3, true);
  if(getPieceCount() == 5) {
    checkIsPawn(4, false);
  }
  checkSameOwner(2, 3, false);

  m_pawnOwner.setOwner(0, getPieceOwner(2));
  m_pawnOwner.setOwner(1, getPieceOwner(3));
}

void EndGameKeyDefinition2Pawns::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const int pawn1StartRow = GETPAWNSTARTROW(getPawnOwner(0));
  const int pawn2StartRow = GETPAWNSTARTROW(getPawnOwner(1));
  for(int col1 = 0; col1 < 4; col1++) {
    const int pawn1Pos = MAKE_POSITION(pawn1StartRow, col1);
    for(int col2 = 0; col2 < 8; col2++) {
      const int pawn2Pos = MAKE_POSITION(pawn2StartRow, col2);
      if(pawn1Pos == pawn2Pos) {
        continue;
      }
      switch(getPieceCount()) {
      case 4:
        tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos);
        break;
      case 5:
        tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos, G3);
        tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos, H3);
        break;
      default:
        invalidPieceCountError(__TFUNCTION__);
      }
    }
  }
}

void EndGameKeyDefinition2Pawns::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, PositionScanner2Pawns nextScanner) const {
  if(pIndex == getPieceCount()) {
    checkForBothPlayers(key);
  } else {
    switch(pIndex) {
    case 0:
      { for(int pos = 0; pos < 64; pos++) {
          key.setPosition(0, pos);
          scanPositions(key, 1, nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 1:
      { const int wkPos = key.getWhiteKingPosition();
        for(int pos = 0; pos < 64; pos++) {
          if(POSADJACENT(wkPos, pos)) continue;
          key.setPosition(1, pos);
          scanPositions(key, 2, nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 4:
      { for(int pos = 0; pos < 64; pos++) {
          if(key.isOccupied(pos)) continue;
          key.setPosition(4, pos);
          scanPositions(key, 5, nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 2:
      { for(int i = 0; i < PAWN1_POSCOUNT; i++) {
          const int pos = s_pawnIndexToPos[i];
          if(key.isOccupied(pos)) continue;
          key.setPosition(2, pos);
          scanPositions(key, 3, nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 3:
      { for(int i = 0; i < PAWN_POSCOUNT; i++) {
          const int pos = s_pawnIndexToPos[i];
          if(key.isOccupied(pos)) continue;
          key.setPosition(3, pos);
          if(nextScanner) { // for kings (pIndex = [0..1]) always use this function as scanner
            (this->*(nextScanner))(key, 4);
          } else {
            scanPositions(key, 4, nextScanner);
          }
          key.clearField(pos);
        }
      }
      break;
    }
  }
}

void EndGameKeyDefinition2Pawns::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  scanPositions(key, 0);
}

String EndGameKeyDefinition2Pawns::getCodecName() const {
  return format(_T("%dMen2Pawns"), getPieceCount());
}

#endif
