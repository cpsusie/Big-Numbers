#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

EndGameKeyDefinition5Men1Pawn::EndGameKeyDefinition5Men1Pawn(PieceKey pk2, PieceKey pk3, PieceKey pk4) 
: EndGameKeyDefinition1Pawn(pk2, pk3, pk4)
{
}

unsigned long EndGameKeyDefinition5Men1Pawn::keyToIndex(const EndGameKey &key) const {
  return ONE_PAWN_5MEN_INDEX(key) - MININDEX;
}

EndGameKey EndGameKeyDefinition5Men1Pawn::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;
  SETPIT(         result, index   );
  SETP4_INDEX(    result, index   );
  SETP3_INDEX(    result, index   );
  SETPAWN1POS(    result, index, 2);
  SETKK_WITH_PAWN(result, index   );
  result.p34IndexToOffDiagPos();
  return result;
}
