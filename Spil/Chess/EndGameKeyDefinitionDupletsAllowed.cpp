#include "stdafx.h"
#include "EndGameKeyDefinition.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinitionDupletsAllowed::EndGameKeyDefinitionDupletsAllowed(PieceKey pk23) : EndGameKeyDefinition(pk23, pk23) {
  initIndexMap();
}

EndGameKeyDefinitionDupletsAllowed::EndGameKeyDefinitionDupletsAllowed(PieceKey pk2, PieceKey pk34) : EndGameKeyDefinition(pk2, pk34, pk34) {
  initIndexMap();
}

#define STATEBIT(  state, index) (1<<((state)*MAX_ENDGAME_PIECECOUNT+(index)))
#define STATESHIFT(state, index) ((m_stateShift & STATEBIT(state, index))?true:false)

void EndGameKeyDefinitionDupletsAllowed::initIndexMap() {
  for(int i = 0; i < ARRAYSIZE(m_pieceKeyIndexMap); i++) {
    for(int j = 0; j < ARRAYSIZE(m_pieceKeyIndexMap[0]); j++) {
      m_pieceKeyIndexMap[i][j] = -1;
    }
  }
  m_stateShift = 0;

  UINT keyTypeCount[MAX_PIECEKEY_VALUE+1];
  for(int i = 0; i < ARRAYSIZE(keyTypeCount); i++) {
    keyTypeCount[i] = 0;
  }
  for(int i = 0; i < getPieceCount(); i++) {
    keyTypeCount[m_pieceKey[i]]++;
  }
  int stateCount = 1;
  for(int i = 0; i < ARRAYSIZE(keyTypeCount); i++) {
    const UINT count = keyTypeCount[i];
    if(count > 1) {
      stateCount *= count;
    }
  }

  assert(stateCount <= MAX_STATECOUNT);
// This works only because we have max. 5 pieces on the board, and always 2 kings (with different color=>different PieceKey),
// so we'll never have more than 3 states, which occurs only when we have 3 (non-king-pieces) of the same type and color.
// In this case there can be no more pieces of duplicate types => no more states.
  for(int state = 0; state < stateCount; state++) {
    for(int i = 0; i < getPieceCount(); i++) {
      const PieceKey pk = m_pieceKey[i];
      assert(state < ARRAYSIZE(m_pieceKeyIndexMap));
      assert(pk    < ARRAYSIZE(m_pieceKeyIndexMap[state]));
      if(keyTypeCount[pk] == 1) {
        m_pieceKeyIndexMap[state][pk] = i;
      } else {
        const int index = m_pieceKeyIndexMap[state][pk] = findKeyIndexByCount(pk, state+1);
        if(state < stateCount-1) {
          setStateShift(state, index);
        }
      }
    }
  }

#ifdef TABLEBASE_BUILDER
  m_equal[0] = m_equal[1] = m_equal[2] = -1;
  for(int i = 2; i < getPieceCount(); i++) {
    if(m_equal[0] == -1) {
      switch(getPieceCount(getPieceOwner(i), getPieceType(i))) {
      case 1:
        break;
      case 2:
      case 3:
        { m_equal[0] = i;
          for(int j = i+1, e = 1; j < getPieceCount(); j++) {
            if(getPieceKey(j) == getPieceKey(i)) {
              m_equal[e++] = j;
            }
          }
        }
        break;
      }
    }
  }
#endif

//#define PRINT_INFO
#ifdef PRINT_INFO
  _tprintf(_T("State count:%d\n"), stateCount);
  _tprintf(_T("State shift:"));
  for(int i = 0; i < m_totalPieceCount; i++) {
    _tprintf(_T("%-6d "), i);
  }
  _tprintf(_T("\n"));
  for(int state = 0; state < stateCount; state++) {
    _tprintf(_T("%11d:"), state);
    for(int i = 0 ; i < m_totalPieceCount; i++) {
      _tprintf(_T("%-6s "), boolToStr(STATESHIFT(state, i)));
    }
    _tprintf(_T("\n"));
  }
  _tprintf(_T("\n"));

  _tprintf(_T("%-16s"), _T("State:"));
  for(int state = 0; state < stateCount; state++) {
    _tprintf(_T("%-11d"), state);
  }
  _tprintf(_T("\n%s\n"), spaceString(16+11*stateCount,'_').cstr());
  forEachPlayer(p) {
    for(PieceType type = King; type <= Pawn; ((int&)type)++) {
      _tprintf(_T("%s %-9s:"), getPlayerNameEnglish(p), getPieceTypeNameEnglish(type));
      const PieceKey pk = MAKE_PIECEKEY(p, type);
      for(state = 0; state < stateCount; state++) {
        const int index = m_pieceKeyIndexMap[state][pk];
        const TCHAR *shiftString = (index < 0) ? _T("") : boolToStr(STATESHIFT(state, index));
        _tprintf(_T("(%+d %-6s)"), index, shiftString);
      }
      _tprintf(_T("\n"));
    }
  }
  _tprintf(_T("\n"));
#endif
}

void EndGameKeyDefinitionDupletsAllowed::setStateShift(int state, int pieceIndex) {
  assert(state      < MAX_STATECOUNT        );
  assert(pieceIndex < MAX_ENDGAME_PIECECOUNT);
  m_stateShift |= STATEBIT(state, pieceIndex);
}

int EndGameKeyDefinitionDupletsAllowed::getPieceIndex(PieceKey pk, int &state) const {
  const int result = m_pieceKeyIndexMap[state][pk];

#ifdef _DEBUG
  if(result < 0) {
    throwException(_T("Keydef %s:getPieceIndex(pk=(%s,%s), state=%d. result=%d")
                  ,toString().cstr()
                  ,getPlayerNameEnglish(GET_PLAYER_FROMKEY(pk))
                  ,getPieceTypeNameEnglish(GET_TYPE_FROMKEY(pk))
                  ,state
                  ,result);
  }
#endif
  if(STATESHIFT(state, result)) {
    state++;
  }
  return result;
}

EndGameKey EndGameKeyDefinitionDupletsAllowed::getEndGameKey(const GameKey &gameKey) const {
  EndGameKey result;
  result.setPlayerInTurn(gameKey.getPlayerInTurn());
#ifdef IS32BIT
#define ASM_OPTIMIZED
#endif

#ifndef ASM_OPTIMIZED

  PieceKey pk;
  int pieceCount = 0;
  for(int pos = 0, state = 0; pos < ARRAYSIZE(gameKey.m_pieceKey); pos++) {
    if((pk = gameKey.m_pieceKey[pos]) == EMPTYPIECEKEY) {
      continue;
    }
    result.setPosition(getPieceIndex(pk, state), pos);
    pieceCount++;
  }
  assert(pieceCount == getPieceCount());

#else

  const PieceKey *ppk = gameKey.m_pieceKey;

  __asm {
    pushf
    cld
  }
  for(int i = getPieceCount(), state = 0; i--; ppk++) {
    __asm {
      mov ecx, 64
      mov edi, ppk
      xor eax, eax
      repe scasb
      dec edi
      mov ppk, edi
    }
    result.setPosition(getPieceIndex(*ppk, state), ppk - gameKey.m_pieceKey);
  }
  __asm {
    popf
  }

#endif

  return result;
}

#ifdef TABLEBASE_BUILDER

bool EndGameKeyDefinitionDupletsAllowed::match(const EndGameKey &key1, const EndGameKey &key2) const {
  if(key2 == key1) {
    return true;
  }
  if(m_equal[0] == -1) {
    return false;
  }
  EndGameKey tmp = key2;
  tmp.setPosition(m_equal[0],key2.getPosition(m_equal[1]));
  tmp.setPosition(m_equal[1],key2.getPosition(m_equal[0]));
  if(tmp == key1) {
    return true;
  }
  if(m_equal[2] == -1) {
    return false;
  }
  tmp = key2;
  tmp.setPosition(m_equal[0],key2.getPosition(m_equal[2]));
  tmp.setPosition(m_equal[1],key2.getPosition(m_equal[0]));
  tmp.setPosition(m_equal[2],key2.getPosition(m_equal[1]));
  if(tmp == key1) {
    return true;
  }
  tmp.setPosition(m_equal[1],key2.getPosition(m_equal[1]));
  tmp.setPosition(m_equal[2],key2.getPosition(m_equal[0]));
  if(tmp == key1) {
    return true;
  }
  tmp.setPosition(m_equal[1],key2.getPosition(m_equal[2]));
  tmp.setPosition(m_equal[0],key2.getPosition(m_equal[0]));
  tmp.setPosition(m_equal[2],key2.getPosition(m_equal[1]));
  if(tmp == key1) {
    return true;
  }
  tmp.setPosition(m_equal[0],key2.getPosition(m_equal[1]));
  tmp.setPosition(m_equal[2],key2.getPosition(m_equal[0]));
  if(tmp == key1) {
    return true;
  }
  return false;
}

#endif
