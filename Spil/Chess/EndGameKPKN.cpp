#include "stdafx.h"
#include "EndGame4MenWithPawns.h"

const EndGameKeyDefinition4Men1Pawn EndGameKPKN::keyDefinition(
  WHITEPAWN
 ,BLACKKNIGHT
);


#if defined(TABLEBASE_BUILDER)

bool EndGameKPKN::isUsableMove(const Move &m) const {
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
