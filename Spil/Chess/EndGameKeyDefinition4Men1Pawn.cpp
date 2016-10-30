#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGamePosIndex EndGameKeyDefinition4Men1Pawn::keyToIndex(const EndGameKey &key) const {
  return ONE_PAWN_4MEN_INDEX(key);
}

EndGameKey EndGameKeyDefinition4Men1Pawn::indexToKey(EndGamePosIndex index) const {
  EndGameKey result;
  SETPIT(         result, index   );
  SETP3_INDEX(    result, index   );
  SETPAWN1POS(    result, index, 2);
  SETKK_WITH_PAWN(result, index   );
  result.p3IndexToOffDiagPos();
  return result;
}
