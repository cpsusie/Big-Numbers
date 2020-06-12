#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

EndGameKeyDefinition5Men1Pawn2Equal::EndGameKeyDefinition5Men1Pawn2Equal(PieceKey pk2, PieceKey pk34)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk34)
{
  assert((GET_TYPE_FROMKEY(pk2)  == Pawn  )
      && (GET_TYPE_FROMKEY(pk34) != Pawn  )
        );
}

EndGamePosIndex EndGameKeyDefinition5Men1Pawn2Equal::keyToIndex(EndGameKey key) const {
  key.sort2Pos(3, 4);
  // pos4 is biggest
  const UINT pi3 = key.getP3OffDiagIndex();
  const UINT pi4 = key.getP4OffDiagIndexEqualP34();

  return ADDPIT(key, ADD2EQUAL(ONE_PAWN_3MEN(key, 2), MAXINDEX_KK_WITH_PAWN_3MEN, pi3, pi4))
       - MININDEX;
}

EndGameKey EndGameKeyDefinition5Men1Pawn2Equal::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  SETPIT(         result, index   );
  SET2POS2EQUAL(  result, index, MAXINDEX_KK_WITH_PAWN_3MEN, 3, 4);
  SETPAWN1POS(    result, index, 2);
  SETKK_WITH_PAWN(result, index   );
  result.p34IndexToOffDiagPosEqualP34();
  return result;
}


SymmetricTransformation EndGameKeyDefinition5Men1Pawn2Equal::getSymTransformation(EndGameKey key) const {
  return getPawnSymTransformation(key);
}

#if defined(TABLEBASE_BUILDER)
void EndGameKeyDefinition5Men1Pawn2Equal::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;
  for(int pawnCol = 0; pawnCol < 4; pawnCol++) {
    const int pawnPositions[] = { MAKE_POSITION(1, pawnCol), MAKE_POSITION(6, pawnCol) };
    for(int pp = 0; pp < ARRAYSIZE(pawnPositions); pp++) {
      const int pawnPos = pawnPositions[pp];
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G2, H2);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G3, H2);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G2, H3);
      tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawnPos, G3, H3);
    }
  }
}

void EndGameKeyDefinition5Men1Pawn2Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const {
  if(pIndex == getPieceCount()) {
    checkForBothPlayers(key);
  } else {
    UINT pos;
    switch(pIndex) {
    case 0:
      for(pos = 0; pos < 64; pos++) {
        key.setPosition(0,pos);
        scanPositions(key, pIndex+1);
        key.clearField(pos);
      }
      break;
    case 1:
      for(pos = 0; pos < 64; pos++) {
        if(POSADJACENT(key.getWhiteKingPosition(), pos) || key.isOccupied(pos)) continue;
        key.setPosition(1,pos);
        scanPositions(key, pIndex+1);
        key.clearField(pos);
      }
      break;
    case 2:
      { for(int i = 0; i < PAWN1_POSCOUNT; i++) {
          pos = s_pawnIndexToPos[i];
          if(key.isOccupied(pos)) continue;
          key.setPosition(2,pos);
          scanPositions(key, pIndex+1);
          key.clearField(pos);
        }
      }
      break;
    case 3:
      for(pos = 0; pos < 64; pos++) {
        if(key.isOccupied(pos)) continue;
        key.setPosition(3,pos);
        scanPositions(key, pIndex+1);
        key.clearField(pos);
      }
      break;
    case 4:
      for(pos = 0; pos < 64; pos++) {
        if((pos <= key.getPosition(3)) || key.isOccupied(pos)) continue;
        key.setPosition(4,pos);
        scanPositions(key, pIndex+1);
        key.clearField(pos);
      }
      break;
    }
  }
}

void EndGameKeyDefinition5Men1Pawn2Equal::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  scanPositions(key, 0);
}

bool EndGameKeyDefinition5Men1Pawn2Equal::keysEqual(EndGameKey key1, EndGameKey key2) const {
  return keysMatch34Equal(key2,key1);
}

String EndGameKeyDefinition5Men1Pawn2Equal::getCodecName() const {
  return _T("5Men1Pawn2Equal");
}

#endif
