#include "stdafx.h"
#include "EndGameUtil.h"

EndGameKeyDefinition1Pawn::EndGameKeyDefinition1Pawn(PieceKey pk2)
: EndGameKeyDefinitionDupletsNotAllowed(pk2) {
#ifdef TABLEBASE_BUILDER
  setPawnOwner();
#endif
}

EndGameKeyDefinition1Pawn::EndGameKeyDefinition1Pawn(PieceKey pk2, PieceKey pk3)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3) {
#ifdef TABLEBASE_BUILDER
  setPawnOwner();
#endif
}

EndGameKeyDefinition1Pawn::EndGameKeyDefinition1Pawn(PieceKey pk2, PieceKey pk3, PieceKey pk4)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3, pk4)
{
#ifdef TABLEBASE_BUILDER
  setPawnOwner();
#endif
  assert(pk3 != pk4);
}

#ifdef TABLEBASE_BUILDER
void EndGameKeyDefinition1Pawn::setPawnOwner() {
  checkIsPawn(2, true);
  for(int i = 3; i < getPieceCount(); i++) {
    checkIsPawn(i, false);
  }
  m_pawnOwner.setOwner(0, getPieceOwner(2));
}
#endif

SymmetricTransformation EndGameKeyDefinition1Pawn::getSymTransformation(EndGameKey key) const {
  return getPawnSymTransformation(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition1Pawn::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const int startRow = GETPAWNSTARTROW(getPawnOwner());
  for(int col = 0; col < 4; col++) {
    const int pawnPos = MAKE_POSITION(startRow, col);
    switch(getPieceCount()) {
    case 3:
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos);
      break;
    case 4:
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G2);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G3);
      break;
    case 5:
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G2, H2);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G3, H2);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G2, H3);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G3, H3);
      break;
    default:
      invalidPieceCountError(__TFUNCTION__);
    }
  }
}

void EndGameKeyDefinition1Pawn::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, PositionScanner1Pawn nextScanner) const {
  if(pIndex == getPieceCount()) {
    checkForBothPlayers(key);
  } else if(pIndex != 2) {
    for(int pos = 0; pos < 64; pos++) {
      if(((pIndex == 1) && POSADJACENT(key.getWhiteKingPosition(), pos)) || key.isOccupied(pos)) {
        continue;
      }
      key.setPosition(pIndex, pos);
      if(nextScanner && (pIndex > 1)) { // for kings (pIndex = [0..1]) always use this function as scanner
        (this->*(nextScanner))(key, pIndex+1);
      } else {
        scanPositions(key, pIndex+1, nextScanner);
      }
      key.clearField(pos);
    }
  } else { // pIndex == 2
    for(int i = 0; i < PAWN1_POSCOUNT; i++) {
      const int pos = s_pawnIndexToPos[i];
      if(key.isOccupied(pos)) continue;
      key.setPosition(2, pos);
      if(nextScanner) {
        (this->*(nextScanner))(key, 3);
      } else {
        scanPositions(key, 3, nextScanner);
      }
      key.clearField(pos);
    }
  }
}

void EndGameKeyDefinition1Pawn::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  scanPositions(key, 0);
}

String EndGameKeyDefinition1Pawn::getCodecName() const {
  return format(_T("%dMen1Pawn"), getPieceCount());
}

#endif
