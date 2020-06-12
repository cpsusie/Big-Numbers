#include "stdafx.h"
#include "EndGameKeyDefinition.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinitionDupletsAllowed::EndGameKeyDefinitionDupletsAllowed(PieceKey pk23) : EndGameKeyDefinition(pk23, pk23) {
  initIndexMap();
}

EndGameKeyDefinitionDupletsAllowed::EndGameKeyDefinitionDupletsAllowed(PieceKey pk2, PieceKey pk34) : EndGameKeyDefinition(pk2, pk34, pk34) {
  initIndexMap();
}

EndGameKeyDefinitionDupletsAllowed::EndGameKeyDefinitionDupletsAllowed(PieceKey pk2, PieceKey pk3, PieceKey pk45) : EndGameKeyDefinition(pk2, pk3, pk45, pk45) {
  initIndexMap();
}

#define STATEBIT(  state, index) (1<<((state)*MAX_ENDGAME_PIECECOUNT+(index)))
#define STATESHIFT(state, index) ((m_stateShift & STATEBIT(state, index))?true:false)

class PieceTypeCounters {
public:
  union {
    BYTE   m_counter[MAX_PIECEKEY_VALUE+1];
    UINT64 m_i64[2];
  };
  inline PieceTypeCounters() {
    m_i64[0] = m_i64[1] = 0;
  }
};

void EndGameKeyDefinitionDupletsAllowed::initIndexMap() {
  for(int i = 0; i < ARRAYSIZE(m_pieceKeyIndexMap); i++) {
    for(int j = 0; j < ARRAYSIZE(m_pieceKeyIndexMap[0]); j++) {
      m_pieceKeyIndexMap[i][j] = -1;
    }
  }

  PieceTypeCounters ptc;
  for(int i = 0; i < getPieceCount(); i++) {
    const PieceKey pk = m_pieceKey[i];
    m_pieceKeyIndexMap[pk][ptc.m_counter[pk]++] = i;
  }
}

EndGameKey EndGameKeyDefinitionDupletsAllowed::getEndGameKey(const GameKey &gameKey) const {
  EndGameKey result;
  result.setPlayerInTurn(gameKey.getPlayerInTurn());

  PieceKey pk;
  PieceTypeCounters ptc;
#if defined(_DEBUG)
  int pieceCount = 0;
#endif
  for(int pos = 0; pos < ARRAYSIZE(gameKey.m_pieceKey); pos++) {
    if((pk = gameKey.m_pieceKey[pos]) == EMPTYPIECEKEY) {
      continue;
    }
    result.setPosition(m_pieceKeyIndexMap[pk][ptc.m_counter[pk]++], pos);
#if defined(_DEBUG)
    pieceCount++;
#endif
  }
  assert(pieceCount == getPieceCount());

  return result;
}

#if defined(TABLEBASE_BUILDER)
bool EndGameKeyDefinitionDupletsAllowed::keysMatch23Equal(EndGameKey k1, EndGameKey k2) {
  if(k2 == k1) return true;
  k2.swapPos(2,3);
  return k2 == k1;
}

bool EndGameKeyDefinitionDupletsAllowed::keysMatch34Equal(EndGameKey k1, EndGameKey k2) {
  if(k2 == k1) return true;
  k2.swapPos(3,4);
  return k2 == k1;
}

bool EndGameKeyDefinitionDupletsAllowed::keysMatch45Equal(EndGameKey k1, EndGameKey k2) {
  if(k2 == k1) return true;
  k2.swapPos(4,5);
  return k2 == k1;
}

bool EndGameKeyDefinitionDupletsAllowed::keysMatch2Pairs(EndGameKey k1, EndGameKey k2) {
  if(k2 == k1) return true;
  k2.swapPos(2,3); if(k2 == k1) return true;
  k2.swapPos(4,5); if(k2 == k1) return true;
  k2.swapPos(2,3); if(k2 == k1) return true;
  return false;
}

bool EndGameKeyDefinitionDupletsAllowed::keysMatch234Equal(EndGameKey k1, EndGameKey k2) {
                   if(k2 == k1) return true;
  k2.swapPos(2,3); if(k2 == k1) return true;
  k2.swapPos(3,4); if(k2 == k1) return true;
  k2.swapPos(2,3); if(k2 == k1) return true;
  k2.swapPos(3,4); if(k2 == k1) return true;
  k2.swapPos(2,3); if(k2 == k1) return true;
  return false;
}

bool EndGameKeyDefinitionDupletsAllowed::keysMatch345Equal(EndGameKey k1, EndGameKey k2) {
                   if(k2 == k1) return true;
  k2.swapPos(3,4); if(k2 == k1) return true;
  k2.swapPos(4,5); if(k2 == k1) return true;
  k2.swapPos(3,4); if(k2 == k1) return true;
  k2.swapPos(4,5); if(k2 == k1) return true;
  k2.swapPos(3,4); if(k2 == k1) return true;
  return false;
}
#endif // TABLEBASE_BUILDER
