#include "stdafx.h"
#include "EndGame4MenWithPawns.h"

const EndGameKeyDefinition4Men1Pawn EndGameKPKB::keyDefinition(
  WHITEPAWN
 ,BLACKBISHOP
);


#if defined(TABLEBASE_BUILDER)

bool EndGameKPKB::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
    return true;

  case PROMOTION :
    return (m.getPromoteTo() == Queen);

  default        :
    return false;
  }
}

#endif
