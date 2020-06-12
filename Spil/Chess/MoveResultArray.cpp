#include "stdafx.h"
#include "EndGameUtil.h"

//----------------------------------------------------------------------------------------------------
static inline bool isWinnerMove(const MoveWithResult &m, Player playerInTurn) {
  return (m.getResult().getStatus() == EG_WHITEWIN) && (playerInTurn == WHITEPLAYER)
      || (m.getResult().getStatus() == EG_BLACKWIN) && (playerInTurn == BLACKPLAYER);
}

static inline bool isDrawMove(const MoveWithResult &m) {
  return (m.getResult().getStatus() == EG_DRAW) || (m.getResult().getStatus() == EG_UNDEFINED);
}

static int positionCompare(const MoveWithResult &m1, const MoveWithResult &m2) {
  int c = m1.getFrom() - m2.getFrom();
  if(c) return c;
  c = m1.getTo() - m2.getTo();
  if(c) return c;
  return pieceTypeCmp(m1.getPromoteTo(), m2.getPromoteTo());
}

class MoveWithResultComparator : public Comparator<MoveWithResult> {
private:
  const Player m_playerInTurn;

  inline bool isWinnerMove(const MoveWithResult &m) const {
    return ::isWinnerMove(m, m_playerInTurn);
  }
public:
  MoveWithResultComparator(Player playerInTurn) : m_playerInTurn(playerInTurn) {
  }
  int compare(const MoveWithResult &m1, const MoveWithResult &m2);
  AbstractComparator *clone() const {
    return new MoveWithResultComparator(m_playerInTurn);
  }
};

int MoveWithResultComparator::compare(const MoveWithResult &m1, const MoveWithResult &m2) {
  int c;
  switch(FBOOL2MASK(isWinnerMove, m1, m2)) {
  case 3:
    c = m1.getResult().getMovesToEnd() - m2.getResult().getMovesToEnd();
    return c ? c : positionCompare(m1, m2);
  case 2: return  1;
  case 1: return -1;
  case 0:
    switch(FBOOL2MASK(isDrawMove, m1, m2)) {
    case 3: return positionCompare(m1, m2);
    case 2: return  1;
    case 1: return -1;
    case 0:
      c = m2.getResult().getMovesToEnd() - m1.getResult().getMovesToEnd();
      return c ? c : positionCompare(m1,m2);
    }
  }
  throwException(_T("%s(%s,%s) dropped to the end")
                ,__TFUNCTION__
                ,m1.toString().cstr()
                ,m2.toString().cstr()
                );
  return 0;
}

CompactIntArray MoveResultArray::findShortestWinnerMoves() const {
  CompactIntArray tmp;
  int minWinDistance = 9999;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.getResult().getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.getResult().getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd < minWinDistance) {
        minWinDistance = pliesToEnd;
      }
    }
  }
  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    if(tmp[i] == minWinDistance) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findLongestLoosingMoves(int defendStrength) const {
  CompactIntArray tmp;
  int maxLooseDistance = 0;
  const EndGamePositionStatus usableStatus = (m_playerInTurn==WHITEPLAYER) ? EG_BLACKWIN : EG_WHITEWIN;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult &m = (*this)[i];
    if(m.getResult().getStatus() != usableStatus) {
      tmp.add(-1);
    } else {
      const int pliesToEnd = m.getResult().getPliesToEnd();
      tmp.add(pliesToEnd);
      if(pliesToEnd > maxLooseDistance) {
        maxLooseDistance = pliesToEnd;
      }
    }
  }
  const int minLooseDistance = maxLooseDistance * defendStrength / 100;

  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    int p = tmp[i];
    if((p >= 0) && (p >= minLooseDistance)) {
      result.add(i);
    }
  }
  return result;
}

CompactIntArray MoveResultArray::findDrawMoves() const {
  CompactIntArray result;
  for(UINT i = 0; i < size(); i++) {
    if((*this)[i].getResult().getStatus() == EG_DRAW) {
      result.add(i);
    }
  }
  return result;
}

MoveResultArray &MoveResultArray::sort() {
  __super::sort(MoveWithResultComparator(m_playerInTurn));
  return *this;
}

MoveWithResult MoveResultArray::selectBestMove(int defendStrength) const {
  CompactIntArray moves = findShortestWinnerMoves();
  if(!moves.isEmpty()) {
    return (*this)[moves.select()];
  } else if(!(moves = findDrawMoves()).isEmpty()) {
    return (*this)[moves.select()];
  } else {
    return selectLongestLoosingMove(defendStrength);
  }
}

BYTE MoveResultArray::getCountWithStatus(EndGamePositionStatus st) const {
  const size_t n     = size();
  UINT         count = 0;
  switch(st) {
  case EG_UNDEFINED:
  case EG_DRAW     :
    for(size_t i = 0; i < n; i++) {
      if(isDrawMove((*this)[i])) {
        count++;
      }
    }
    break;
  case EG_WHITEWIN :
  case EG_BLACKWIN :
    for(size_t i = 0; i < n; i++) {
      const MoveWithResult &mr = (*this)[i];
      if(mr.getResult().getStatus() == st) {
        count++;
      }
    }
    break;
  }
  assert(count <= 255);
  return (BYTE)count;
}

String MoveResultArray::toString(const Game &game, MoveStringFormat mf, bool depthInPlies) {
  sort();
  StringArray msa;
  for(size_t i = 0; i < size(); i++) {
    msa.add(PrintableMove(game,(*this)[i]).toString(mf));
  }
  const int maxMoveStrLength = (int)msa.maxLength();
  String result;
  for(size_t i = 0; i < size(); i++) {
    result += format(_T("%-*s - %s\n"), maxMoveStrLength, msa[i].cstr(), (*this)[i].getResult().toString(m_playerInTurn, depthInPlies).cstr());
  }
  return result;
}

Packer &operator<<(Packer &p, const MoveResultArray &a) {
  return p << (BYTE)a.m_playerInTurn << (CompactArray<MoveWithResult>&)a;
}

Packer &operator>>(Packer &p, MoveResultArray &a) {
  BYTE player;
  p >> player;
  a.clear((Player)player);
  return p >> (CompactArray<MoveWithResult>&)a;
}

#if !defined(TABLEBASE_BUILDER)

class MoveWithResult2Comparator : public Comparator<MoveWithResult2> {
private:
  const Player m_playerInTurn;

  bool isWinnerMove(const MoveWithResult &m) const {
    return ::isWinnerMove(m, m_playerInTurn);
  }
public:
  MoveWithResult2Comparator(Player playerInTurn) : m_playerInTurn(playerInTurn) {
  }
  int compare(const MoveWithResult2 &m1, const MoveWithResult2 &m2);
  AbstractComparator *clone() const {
    return new MoveWithResult2Comparator(m_playerInTurn);
  }
};

int MoveWithResult2Comparator::compare(const MoveWithResult2 &m1, const MoveWithResult2 &m2) {
  int c;
  switch(FBOOL2MASK(isWinnerMove, m1, m2)) {
  case 3:
    c = m1.getResult().getMovesToEnd() - m2.getResult().getMovesToEnd();
    return c ? c : positionCompare(m1, m2);
  case 2: return  1;
  case 1: return -1;
  case 0:
    switch(FBOOL2MASK(isDrawMove, m1, m2)) {
    case 3:
      c = sign(m1.getReplyCount().getDrawReplyPct() - m2.getReplyCount().getDrawReplyPct());
      if(c) return c;
      c = sign(m2.getReplyCount().getLooseReplyPct() - m1.getReplyCount().getLooseReplyPct());
      return c ? c : positionCompare(m1, m2);
    case 2: return  1;
    case 1: return -1;
    case 0:
      c = m2.getResult().getMovesToEnd() - m1.getResult().getMovesToEnd();
      return c ? c : positionCompare(m1,m2);
    }
  }
  throwException(_T("%s(%s,%s) dropped to the end")
                ,__TFUNCTION__
                ,m1.toString().cstr()
                ,m2.toString().cstr()
                );
  return 0;
}

MoveResult2Array::operator MoveResultArray() const {
  const size_t n = size();
  MoveResultArray result;
  result.clear(getPlayerInTurn(), n);
  for(size_t i = 0; i < n; i++) {
    result.add((*this)[i]);
  }
  return result;
}

MoveResult2Array &MoveResult2Array::sort() {
  __super::sort(MoveWithResult2Comparator(m_playerInTurn));
  return *this;
}

CompactIntArray MoveResult2Array::findBestDrawMoves() const {
  CompactDoubleArray tmp;
  double             minDrawReplyPct = 200;
  for(size_t i = 0; i < size(); i++) {
    const MoveWithResult2 &m = (*this)[i];
    if(!isDrawMove(m)) {
      tmp.add(-1);
    } else {
      const double drawReplyPct = m.getReplyCount().getDrawReplyPct();
      tmp.add(drawReplyPct);
      if(drawReplyPct < minDrawReplyPct) {
        minDrawReplyPct = drawReplyPct;
      }
    }
  }
  CompactIntArray result;
  for(UINT i = 0; i < tmp.size(); i++) {
    if(tmp[i] == minDrawReplyPct) {
      result.add(i);
    }
  }
  return result;
}

MoveWithResult MoveResult2Array::selectBestMove(int defendStrength) const {
  CompactIntArray moves = findShortestWinnerMoves();
  if(!moves.isEmpty()) {
    return (*this)[moves.select()];
  } else if(!(moves = findBestDrawMoves()).isEmpty()) {
    return (*this)[moves.select()];
  } else {
    return selectLongestLoosingMove(defendStrength);
  }
}

String MoveResult2Array::toString(const Game &game, MoveStringFormat mf, bool depthInPlies) {
  sort();
  StringArray msa;
  for(size_t i = 0; i < size(); i++) {
    msa.add(PrintableMove(game,(*this)[i]).toString(mf));
  }
  const int maxMoveStrLength = (int)msa.maxLength();
  String result;
  for(size_t i = 0; i < size(); i++) {
    result += format(_T("%-*s - %s\n"), maxMoveStrLength, msa[i].cstr(), (*this)[i].resultToString(m_playerInTurn, depthInPlies).cstr());
  }
  return result;
}

#endif // TABLEBASE_BUILDER
