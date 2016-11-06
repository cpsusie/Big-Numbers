#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

EndGamePosIndex EndGameKeyDefinition3Men1Pawn::keyToIndex(EndGameKey key) const {
  return ONE_PAWN_3MEN_INDEX(key) - MININDEX;
}

EndGameKey EndGameKeyDefinition3Men1Pawn::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;
  EndGameKey result;
  SETPIT(         result, index   );
  SETPAWN1POS(    result, index, 2);
  SETKK_WITH_PAWN(result, index   );
  return result;
}
