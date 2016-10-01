#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 120

unsigned long EndGameKeyDefinition5Men2Pawns::keyToIndex(const EndGameKey &key) const {
  return TWO_PAWNS_5MEN_INDEX(key) - MININDEX;
}

EndGameKey EndGameKeyDefinition5Men2Pawns::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;
  SETPIT(         result, index       );
  SETPOS_INDEX(   result, index, 60, 4);
  SETPAWN2POS(    result, index,     3);
  SETPAWN1POS(    result, index,     2);
  SETKK_WITH_PAWN(result, index       );
  result.p3IndexToPawn2Pos();
  result.p4IndexToOffDiagPos();
  return result;
}
