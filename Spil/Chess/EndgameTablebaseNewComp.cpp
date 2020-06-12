#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

#if defined(NEWCOMPRESSION)

#if !defined(TABLEBASE_BUILDER)

void EndGameTablebase::decompress(ByteInputStream &s) const {
  BigEndianInputStream bes(s);
  TablebaseInfo info;
  info.load(bes);
  m_positionIndex.decompress(bes, info);
}

class GameResultMap : public CompactHashMap<EndGameKey, EndGameResult> { // map->result are all !isPrunedWinnerResult
private:
  const EndGameTablebase    &m_tb;
  const IndexedMap          &m_positionIndex;
  Game                      &m_game;
  CompactHashSet<EndGameKey> m_constructingSet; // contains positions under construction
  inline EndGameKey transformGameKey(const GameKey &key) const {
    return m_positionIndex.getKeyDef().transformGameKey(key);
  }
  inline EndGameKey transformGameKey(const Game &game) const {
    return transformGameKey(game.getKey());
  }
  inline void saveResult(const EndGameKey &key , const EndGameResult &result) {
    put(key, result);
  }
  inline void saveResult(const Game &game , const EndGameResult &result) {
    saveResult(transformGameKey(game), result);
  }
  EndGameResult lookupGamePosition(EndGameKey *egkp = NULL) const;
public:
  GameResultMap(const EndGameTablebase *tb, Game &game)
    : m_tb(*tb)
    , m_positionIndex(tb->m_positionIndex)
    , m_game(game)
  {
  }
  EndGameResult findResult(UINT level, const MoveBase &lastMove);
};

EndGameResult GameResultMap::lookupGamePosition(EndGameKey *egkp) const {
  EndGameKey egkTmp, &egk(egkp?*egkp:egkTmp);
  egk = transformGameKey(m_game);
  const EndGameResult *rp = get(egk);
  return rp ? *rp : m_positionIndex.get(egk);
}

#define RETURN(egr) { saveResult(m_game, egr); debugLog(_T("%*.*s(%d):%s\n"), level,level,EMPTYSTRING, level, (egr).toString().cstr()); return egr; }

EndGameResult GameResultMap::findResult(UINT level, const MoveBase &lastMove) {
  debugLog(_T("%*.*s(%d):%6s:findResult(%s,%s)\n"), level, level, EMPTYSTRING, level, lastMove.toString().cstr(), m_tb.getName().cstr(), m_game.toFENString().cstr());
  switch(m_game.findGameResult()) {
  case NORESULT                  : break;
  case WHITE_CHECKMATE           : RETURN(EGR_WHITEISMATE);
  case BLACK_CHECKMATE           : RETURN(EGR_BLACKISMATE);
  default                        : RETURN(EGR_DRAW);
  }
  EndGameKey    key;
  EndGameResult result = lookupGamePosition(&key);
  EndGameResult mateResult; // = EG_UNDEFINED
  if(!result.isPrunedWinnerResult()) {
    RETURN(result);
  }
  m_constructingSet.add(key);
  MoveResultArray ma;
  ma.clear(m_game.getPlayerInTurn());
  int             prunedKeyCount = 0;
  Move            m;
  m.clearAnnotation();
  MoveGenerator &mg = m_game.getMoveGenerator();
  m_game.pushState();
  for(bool more = mg.firstMove(m); more && (mateResult.getStatus() == EG_UNDEFINED); m_game.unTryMove(), more = mg.nextMove(m)) {
    m_game.tryMove(m);
    UINT                        pliesToEnd;
    const EndGamePositionStatus status = m_tb.isTerminalMove(m_game, m, &pliesToEnd);
    switch(status) {
    case EG_DRAW     :
      result = EGR_DRAW;
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      result = EndGameResult(status, pliesToEnd);
      break;
    case EG_UNDEFINED:
      result = lookupGamePosition(&key);
      if(result.isPrunedWinnerResult()) {
        switch(m_game.findGameResult()) {
        case NORESULT                  : break;
        case WHITE_CHECKMATE           : mateResult = result = EGR_WHITEISMATE; break;
        case BLACK_CHECKMATE           : mateResult = result = EGR_BLACKISMATE; break;
        default                        :              result = EGR_DRAW;        break;
        }
      }
      if(!result.isPrunedWinnerResult()) {
        saveResult(transformGameKey(m_game), result);
      } else if(!m_constructingSet.contains(key)) {
        prunedKeyCount++;
      } else {
        continue;
      }
      ma.add(MoveWithResult(m, result));
    }
  }
  m_game.popState();

  if(mateResult.getStatus() != EG_UNDEFINED) {
    RETURN(mateResult);
  } else if(prunedKeyCount==0) {
    result = ma.selectBestMove(100).getResult();
    RETURN(result);
  }

  debugLog(_T("%s"), indentString(ma.toString(m_game, MOVE_SHORTFORMAT, true), level).cstr());

  m_game.pushState();
  for(size_t i = 0; i < ma.size(); i++) {
    MoveWithResult &mr = ma[i];
    if(mr.getResult().isPrunedWinnerResult()) {
      m_game.tryMove(mr);
      const EndGameResult egr = findResult(level+1, mr);
      mr.setResult(egr.isWinner() ? EndGameResult(egr.getStatus(),egr.getPliesToEnd()+1):EGR_DRAW);
      m_game.unTryMove();
    }
  }
  m_game.popState();
  debugLog(_T("%s"), indentString(ma.toString(m_game, MOVE_SHORTFORMAT, true), level).cstr());

  result = ma.selectBestMove(100).getResult();
  RETURN(result);
}

#endif // TABLEBASE_BUILDER

MoveResultArray &EndGameTablebase::getAllMoves(const GameKey &gameKey, MoveResultArray &a) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());

  const Player playerInTurn = gameKey.getPlayerInTurn();

  a.clear(playerInTurn);
  Game            game = gameKey;
  Move            m;
  m.clearAnnotation();
  const Piece    *skipNextPromotion = NULL;

  MoveGenerator &mg = game.getMoveGenerator();
  game.pushState();
  for(bool more = mg.firstMove(m); more; game.unTryMove(), more = mg.nextMove(m)) {
    game.tryMove(m);
    if(skipNextPromotion) {
      if(m.m_piece == skipNextPromotion) {
        continue;
      } else {
        skipNextPromotion = NULL;
      }
    }
    UINT pliesToEnd;
    const EndGamePositionStatus status = isTerminalMove(game, m, &pliesToEnd);
    switch(status) {
    case EG_DRAW     :
      a.add(MoveWithResult(m, EGR_TERMINALDRAW));
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { const EndGameResult succResult(status, pliesToEnd);
        a.add(MoveWithResult(m, succResult));
#if !defined(TABLEBASE_BUILDER)
        if(m.getType() == PROMOTION) {
          const EndGameResult keyResult = getKeyResult(transformGameKey(gameKey));
          if(  (succResult.getStatus()         == keyResult.getStatus())
            && (succResult.getPliesToEnd() + 1 == keyResult.getPliesToEnd())) {
            skipNextPromotion = m.m_piece; // got the best promotion, no need to load the others, same trick as getBestResult
          }
        }
#endif
      }
      break;
    case EG_UNDEFINED:
      if(game.getKey().getEPSquare() > 0) {
        MoveResultArray allReplies;
        const EndGameResult egr = getAllMoves(game.getKey(), allReplies).selectBestMove(100).getResult();
        if(!egr.isWinner()) {
          a.add(MoveWithResult(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, EndGameResult(egr.getStatus(), egr.getPliesToEnd()+1)));
        }
      } else {
        const EndGameKey    succKey = transformGameKey(game.getKey());
#if defined(TABLEBASE_BUILDER)
        const EndGameResult egr     = m_positionIndex.get(succKey);
        if(!egr.isDefined()) {
          a.add(MoveWithResult(m, EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, egr));
        }
#else // !TABLEBASE_BUILDER
        EndGameResult       egr     = m_positionIndex.get(succKey);
        if(egr.isPrunedWinnerResult()) {
          egr = GameResultMap(this, game).findResult(0,m);
        }
        if(!egr.isWinner()) {
          a.add(MoveWithResult(m, EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, egr));
        }
#endif // !TABLEBASE_BUILDER
      }
      break;
    default:
      statusError(status, game, m);
    }
  }
  game.popState();
  return a;
}


#if !defined(TABLEBASE_BUILDER)
MoveResult2Array &EndGameTablebase::getAllMoves(const GameKey &gameKey, MoveResult2Array &a) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());

  const Player playerInTurn = gameKey.getPlayerInTurn();

  a.clear(playerInTurn);
  Game            game = gameKey;
  Move            m;
  m.clearAnnotation();
  const Piece    *skipNextPromotion = NULL;

  MoveGenerator &mg = game.getMoveGenerator();
  game.pushState();
  for(bool more = mg.firstMove(m); more; game.unTryMove(), more = mg.nextMove(m)) {
    game.tryMove(m);
    if(skipNextPromotion) {
      if(m.m_piece == skipNextPromotion) {
        continue;
      } else {
        skipNextPromotion = NULL;
      }
    }
    UINT            pliesToEnd;
    MoveResultArray allReplies;
    const EndGamePositionStatus status = isTerminalMove(game, m, &pliesToEnd, &allReplies);
    switch(status) {
    case EG_DRAW     :
      // cannot use pliesToEnd...it's always zero. check for allReplies.isEmpty() instead
      a.add(MoveWithResult2(m, allReplies.isEmpty()?EGR_STALEMATE:EGR_DRAW, allReplies));
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { const EndGameResult succResult(status, pliesToEnd);
        a.add(MoveWithResult2(m, succResult));
        if(m.getType() == PROMOTION) {
          const EndGameResult keyResult = getKeyResult(transformGameKey(gameKey));
          if(  (succResult.getStatus()         == keyResult.getStatus())
            && (succResult.getPliesToEnd() + 1 == keyResult.getPliesToEnd())) {
            skipNextPromotion = m.m_piece; // got the best promotion, no need to load the others, same trick as getBestResult
          }
        }
      }
      break;
    case EG_UNDEFINED:
      if(game.getKey().getEPSquare() > 0) {
        const EndGameResult egr = getAllMoves(game.getKey(), allReplies).selectBestMove(100).getResult();
        if(!egr.isWinner()) {
          a.add(MoveWithResult2(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW, allReplies));
        } else {
          a.add(MoveWithResult2(m, EndGameResult(egr.getStatus(), egr.getPliesToEnd()+1)));
        }
      } else {
        const EndGameKey succKey = transformGameKey(game.getKey());
        EndGameResult    egr     = m_positionIndex.get(succKey);
        if(egr.isPrunedWinnerResult()) {
          egr = GameResultMap(this, game).findResult(0,m);
        }
        if(!egr.isWinner()) {
          getAllMoves(game.getKey(), allReplies);
          a.add(MoveWithResult2(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW, allReplies));
        } else {
          a.add(MoveWithResult2(m, egr));
        }
      }
      break;
    default:
      statusError(status, game, m);
    }
  }
  game.popState();
  return a;
}
#endif // TABLEBASE_BUILDER

#endif // NEWCOMPRESSION
