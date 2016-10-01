#include "stdafx.h"
#include "EndGame3Men.h"

const EndGameKeyDefinition3Men1Pawn EndGameKPK::keyDefinition(
  WHITEPAWN
);



#ifdef TABLEBASE_BUILDER

bool EndGameKPK::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
    return true;

  case PROMOTION :
    return (m.getPromoteTo() == Queen) || (m.getPromoteTo() == Rook);

  default        :
    return false;
  }
}

#endif
