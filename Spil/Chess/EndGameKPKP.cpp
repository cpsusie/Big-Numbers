#include "stdafx.h"
#include "EndGame4MenWithPawns.h"

const EndGameKeyDefinition4Men2Pawns EndGameKPKP::keyDefinition(
  WHITEPAWN
 ,BLACKPAWN
);


#if defined(TABLEBASE_BUILDER)

bool EndGameKPKP::isUsableMove(const Move &m) const {
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
