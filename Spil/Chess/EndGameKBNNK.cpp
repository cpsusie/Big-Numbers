#include "stdafx.h"
#include "EndGame5MenNoPawns.h"

#if defined(ENDGAME_NOKING)

const EndGameKeyDefinitionKBNNKNoKing EndGameKBNNK::keyDefinition;

#else

const EndGameKeyDefinition5Men2Equal EndGameKBNNK::keyDefinition(
  WHITEBISHOP
 ,WHITEKNIGHT
);

#endif
