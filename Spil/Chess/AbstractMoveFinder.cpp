#include "stdafx.h"
#include "AbstractMoveFinder.h"
#include "TraceDlgThread.h"

AbstractMoveFinder::AbstractMoveFinder(Player player, ChessPlayerRequestQueue &msgQueue)
: m_player(  player  )
, m_msgQueue(msgQueue)
{
  m_verbose = false;
}

void AbstractMoveFinder::initSearch(const RequestParamFindMove &param) {
  m_game = param.getGame();
#if !defined(TABLEBASE_BUILDER)
  m_game.setMaxPositionRepeat(1);
#endif
  m_hint = param.isHint();
  setVerbose(param.isVerbose());
}

void AbstractMoveFinder::putMove(const MoveBase &m) {
  m_msgQueue.put(ChessPlayerRequest(m, m_hint));
}

void AbstractMoveFinder::putError(const TCHAR *fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);
  const String msg = vformat(fmt, argptr);
  va_end(argptr);
  m_msgQueue.put(ChessPlayerRequest(msg, true));
}

PrintableMove AbstractMoveFinder::checkForSingleMove() {
  if(m_game.isSingleMovePosition()) { // dont waste time to think. just return the only possible move
    const PrintableMove result(m_game, m_game.getRandomMove());
    if(isVerbose()) {
      verbose(_T("%s:%s\n"), loadString(IDS_MSG_ONLY_1_MOVE).cstr(), result.toString().cstr());
    }
    return result;
  }
  return PrintableMove();
}
