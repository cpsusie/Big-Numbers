#include "stdafx.h"
#include "EndGame5MenWithPawns.h"

const EndGameKeyDefinition5Men1Pawn EndGameKNPKN::keyDefinition(
  WHITEPAWN
 ,WHITEKNIGHT
 ,BLACKKNIGHT
);

#ifdef TABLEBASE_BUILDER

bool EndGameKNPKN::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
  case PROMOTION :
    return (m.getPromoteTo() != Knight);

  default        :
    return false;
  }
}

#endif
