#include "stdafx.h"
#include "EndGameUtil.h"

#define GAMERESULTTOPOSITIONSTATUS(r)  (((r) == STALEMATE)       ? EG_DRAW      \
                                     :  ((r) == BLACK_CHECKMATE) ? EG_WHITEWIN  \
                                     :  ((r) == WHITE_CHECKMATE) ? EG_BLACKWIN  \
                                     :  EG_UNDEFINED                            \
                                       )

const TCHAR *positionStatusToString(EndGamePositionStatus state) {
  switch(state) {
  case EG_UNDEFINED: return _T("undefined");
  case EG_DRAW     : return _T("draw");
  case EG_WHITEWIN : return _T("white win");
  case EG_BLACKWIN : return _T("black win");
  default          : return _T("?");
  }
}

EndGameResult &EndGameResult::setResult(EndGamePositionStatus status, UINT pliesToEnd) {
  assert(status >= 0 && status <= 3);
  assert(pliesToEnd <= 0x1ff);
  assert(exists() && !isDefined());
  m_data |= (status << 9) | pliesToEnd;
  return *this;
}

EndGameResult &EndGameResult::changePliesToEnd(UINT pliesToEnd) {
  assert(exists() && isDefined());
  assert(pliesToEnd <= 0x1ff);
  m_data = (m_data & ~0x1ff) | pliesToEnd;
  return *this;
}

#ifdef TABLEBASE_BUILDER

#define STALEMATE_STR      _T("Stalemate")
#define DRAW_STR           _T("Draw")
#define TERMINALDRAW_STR   _T("Terminal draw")
#define BLACKCHECKMATE_STR _T("Black checkmate")
#define WHITECHECKMATE_STR _T("White checkmate")
#define WININPLIES_STR     _T("Win in ")
#define LOOSEINPLIES_STR   _T("Loose in ")
#define PLY                _T("ply")
#define PLIES              _T("plies")
#define MOVE               _T("move")
#define MOVES              _T("moves")

#else

#define STALEMATE_STR      loadString(IDS_STALEMATE     , _T("Stalemate")      ).cstr()
#define DRAW_STR           loadString(IDS_DRAW          , _T("Draw")           ).cstr()
#define TERMINALDRAW_STR   loadString(IDS_TERMINALDRAW  , _T("Terminal draw")  ).cstr()
#define BLACKCHECKMATE_STR loadString(IDS_BLACKCHECKMATE, _T("Black checkmate")).cstr()
#define WHITECHECKMATE_STR loadString(IDS_WHITECHECKMATE, _T("White checkmate")).cstr()
#define WININPLIES_STR     loadString(IDS_WININPLIES    , _T("Win in ")        ).cstr()
#define LOOSEINPLIES_STR   loadString(IDS_LOOSEINPLIES  , _T("Loose in ")      ).cstr()
#define PLY                loadString(IDS_PLY           , _T("ply")            ).cstr()
#define PLIES              loadString(IDS_PLIES         , _T("plies")          ).cstr()
#define MOVE               loadString(IDS_MOVE          , _T("move")           ).cstr()
#define MOVES              loadString(IDS_MOVES         , _T("moves")          ).cstr()

#endif

EndGameResult::EndGameResult(GameResult gr) {
  const EndGamePositionStatus status = GAMERESULTTOPOSITIONSTATUS(gr);
  m_data = (status << 9);
}

static TCHAR *pliesToStr(TCHAR *dst, int pliesToEnd, bool ply) {
  if(ply) {
    _stprintf(dst, _T("%3d %s"), pliesToEnd, (pliesToEnd==1)? PLY : PLIES);
  } else {
#ifdef TABLEBASE_BUILDER
    _stprintf(dst, _T("%3d")    , PLIESTOMOVES(pliesToEnd));
#else
    const int moves = PLIESTOMOVES(pliesToEnd);
    _stprintf(dst, _T("%3d %s"), moves, (moves==1)?MOVE:MOVES);
#endif
  }
  return dst;
}

TCHAR *EndGameResult::toStr(TCHAR *dst, bool ply) const {
  const int pliesToEnd = getPliesToEnd();
  TCHAR plyStr[40];
  switch(getStatus()) {
  case EG_UNDEFINED:
    return _tcscpy(dst, _T("(?)"));

  case EG_DRAW     :
    switch(pliesToEnd) {
    case 0 : return _tcscpy(dst, STALEMATE_STR   );
    case 1 : return _tcscpy(dst, TERMINALDRAW_STR);
    default: return _tcscpy(dst, _T("(/)")       );
    }

  case EG_WHITEWIN :
    if(pliesToEnd) {
      return _tcscat(_tcscpy(dst, _T("(W) ")), pliesToStr(plyStr, pliesToEnd, ply));
    } else {
      return _tcscpy(dst, BLACKCHECKMATE_STR);
    }

  case EG_BLACKWIN :
    if(pliesToEnd) {
      return _tcscat(_tcscpy(dst, _T("(B) ")), pliesToStr(plyStr, pliesToEnd, ply));
    } else {
      return _tcscpy(dst, WHITECHECKMATE_STR);
    }

  default          :
    throwException(_T("%s:status=%d"), __TFUNCTION__, getStatus());
    return EMPTYSTRING;
  }
}

TCHAR *EndGameResult::toStr(TCHAR *dst, Player playerInTurn, bool ply) const {
  switch(getStatus()) {
  case EG_UNDEFINED:
  case EG_DRAW     :
    return _tcscpy(dst, DRAW_STR);
  case EG_WHITEWIN:
  case EG_BLACKWIN:
    if(getPliesToEnd() == 0) {
      return _tcscpy(dst, (getStatus() == EG_WHITEWIN) ? BLACKCHECKMATE_STR : WHITECHECKMATE_STR);
    } else {
      TCHAR plyStr[40];
      if(STATUSTOWINNER(getStatus()) == playerInTurn) {
        return _tcscat(_tcscpy(dst, WININPLIES_STR  ), pliesToStr(plyStr, getPliesToEnd(), ply));
      } else {
        return _tcscat(_tcscpy(dst, LOOSEINPLIES_STR), pliesToStr(plyStr, getPliesToEnd(), ply));
      }
    }
  default:
    throwException(_T("%s:Unknown status:%d"), __TFUNCTION__, getStatus());
    return EMPTYSTRING;
  }
}

String EndGameResult::toString(bool ply) const {
  TCHAR tmp[300];
  return toStr(tmp, ply);
}

String EndGameResult::toString(Player playerInTurn, bool ply) const {
  TCHAR tmp[300];
  return toStr(tmp, playerInTurn, ply);
}
