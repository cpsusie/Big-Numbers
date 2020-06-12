#include "stdafx.h"
#include "EndGame5MenWithPawns.h"

const EndGameKeyDefinition5Men1Pawn2Equal EndGameKNNKP::keyDefinition(
  BLACKPAWN
 ,WHITEKNIGHT
);


#if defined(TABLEBASE_BUILDER)

bool EndGameKNNKP::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
    return true;

  case PROMOTION  :
    return (m.getPromoteTo() == Queen);

  default:
    return false;
  }
}

#endif
