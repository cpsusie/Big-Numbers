#include "stdafx.h"
#include "EndGame5MenNoPawns.h"

#if defined(ENDGAME_NOKING)

const EndGameKeyDefinitionKNNNKNoKing EndGameKNNNK::keyDefinition;

#else

const EndGameKeyDefinition5Men3Equal EndGameKNNNK::keyDefinition(
  WHITEKNIGHT
);

#endif
