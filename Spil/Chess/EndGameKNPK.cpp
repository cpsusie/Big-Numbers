#include "stdafx.h"
#include "EndGame4MenWithPawns.h"

const EndGameKeyDefinition4Men1Pawn EndGameKNPK::keyDefinition(
  WHITEPAWN
 ,WHITEKNIGHT
);


#ifdef TABLEBASE_BUILDER

bool EndGameKNPK::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
    return true;

  case PROMOTION  :
    return (m.getPromoteTo() != Knight);

  default:
    return false;
  }
}

#endif
