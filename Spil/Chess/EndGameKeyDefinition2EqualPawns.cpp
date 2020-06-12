#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition2EqualPawns::EndGameKeyDefinition2EqualPawns(PieceKey pk23)
: EndGameKeyDefinitionDupletsAllowed(pk23) {
#if defined(TABLEBASE_BUILDER)
  setPawnsOwner();
#endif
}

EndGameKeyDefinition2EqualPawns::EndGameKeyDefinition2EqualPawns(PieceKey pk2, PieceKey pk34)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk34)
{
#if defined(TABLEBASE_BUILDER)
  setPawnsOwner();
#endif
}

#if defined(TABLEBASE_BUILDER)
void EndGameKeyDefinition2EqualPawns::setPawnsOwner() {
  switch(getPieceCount()) {
  case 4 :
    { checkIsPawn(2, true);
      checkIsPawn(3, true);
      checkSameOwner(2, 3, true);
      m_pawnsOwner.setOwner(0, getPieceOwner(2));
    }
    break;
  case 5 :
    { checkIsPawn(2, false);
      checkIsPawn(3, true );
      checkIsPawn(4, true );
      checkSameOwner(3, 4, true);
      m_pawnsOwner.setOwner(0, getPieceOwner(3));
    }
    break;
  default:
    invalidPieceCountError(__TFUNCTION__);
  }
}
#endif

