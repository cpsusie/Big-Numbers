#include "stdafx.h"
#include "EndGame4MenWithPawns.h"

const EndGameKeyDefinition4Men2EqualPawns EndGameKPPK::keyDefinition(
  WHITEPAWN
);



#if defined(TABLEBASE_BUILDER)

bool EndGameKPPK::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE:
    return true;

  case PROMOTION :
    return (m.getPromoteTo() == Queen) || (m.getPromoteTo() == Rook);

  case ENPASSANT :
    return true;

  default        :
    moveTypeError(m);
    return false;
  }
}

#endif
