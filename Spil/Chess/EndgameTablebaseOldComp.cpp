#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

#ifndef NEWCOMPRESSION

#ifndef TABLEBASE_BUILDER

void EndGameTablebase::decompress(ByteInputStream &s) const {
  TablebaseInfo info;
  info.load(s);
  m_positionIndex.decompress(s, info);
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
#ifndef TABLEBASE_BUILDER
        if(m.getType() == PROMOTION) {
          const EndGameResult keyResult = getKeyResult(transformGameKey(gameKey));
          if(  (succResult.getStatus()         == keyResult.getStatus())
            && (succResult.getPliesToEnd() + 1 == keyResult.getPliesToEnd())) {
            skipNextPromotion = m.m_piece; // got the best promotion, no need to load the others, same trick as getBestResult
          }
        }
#endif // TABLEBASE_BUILDER
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
        const EndGameResult egr     = m_positionIndex.get(succKey);
        if(!egr.isDefined()) {
          a.add(MoveWithResult(m, EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, egr));
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

#ifndef TABLEBASE_BUILDER
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
        const EndGameKey    succKey = transformGameKey(game.getKey());
        const EndGameResult egr     = m_positionIndex.get(succKey);
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

