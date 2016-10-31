#include "stdafx.h"
#include "EndGameKeyDefinition.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinitionDupletsNotAllowed::EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2) : EndGameKeyDefinition(pk2) {
  initIndexMap();
}

EndGameKeyDefinitionDupletsNotAllowed::EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2, PieceKey pk3) : EndGameKeyDefinition(pk2, pk3) {
  assert(pk2 != pk3);
  initIndexMap();
}

EndGameKeyDefinitionDupletsNotAllowed::EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2, PieceKey pk3, PieceKey pk4) : EndGameKeyDefinition(pk2, pk3, pk4) {
  assert((pk2 != pk3) && (pk2 != pk4) && (pk3 != pk4));
  initIndexMap();
}

void EndGameKeyDefinitionDupletsNotAllowed::initIndexMap() {
  for(int i = 0; i < ARRAYSIZE(m_pieceKeyIndexMap); i++) {
    m_pieceKeyIndexMap[i] = -1;
  }

  for(int i = 0; i < getPieceCount(); i++) {
    m_pieceKeyIndexMap[m_pieceKey[i]] = i;
  }

//#define PRINT_INFO
#ifdef PRINT_INFO
  _tprintf(_T("IndexMap:\nPiece  White Black\n"));
  for(PieceType type = King; type <= Pawn; ((int&)type)++) {
    _tprintf(_T("%-6s"), getPieceTypeNameEnglish(type));
    forEachPlayer(p) {
      _tprintf(_T("%+6d"), m_pieceKeyIndexMap[MAKE_PIECEKEY(p, type)]);
    }
    _tprintf(_T("\n"));
  }
  _tprintf(_T("\n"));

#endif

}

EndGameKey EndGameKeyDefinitionDupletsNotAllowed::getEndGameKey(const GameKey &gameKey) const {
  EndGameKey result;
  result.setPlayerInTurn(gameKey.getPlayerInTurn());

#ifdef IS32BIT
#define ASM_OPTIMIZED
#endif

#ifndef ASM_OPTIMIZED

  PieceKey pk;
  int pieceCount = 0;
  for(int pos = 0; pos < ARRAYSIZE(gameKey.m_pieceKey); pos++) {
    if((pk = gameKey.m_pieceKey[pos]) == EMPTYPIECEKEY) {
      continue;
    }
    result.setPosition(m_pieceKeyIndexMap[pk], pos);
    pieceCount++;
  }
  assert(pieceCount == getPieceCount());

#else

  const PieceKey *ppk = gameKey.m_pieceKey;

  __asm {
    pushf
    cld
  }
  for(int i = getPieceCount(); i--; ppk++) {
    __asm {
      mov ecx, 64
      mov edi, ppk
      xor eax, eax
      repe scasb
      dec edi
      mov ppk, edi
    }
    result.setPosition(m_pieceKeyIndexMap[*ppk], ppk - gameKey.m_pieceKey);
  }
  __asm {
    popf
  }

#endif

  return result;
}
