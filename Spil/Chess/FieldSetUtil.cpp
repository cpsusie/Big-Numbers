#include "stdafx.h"

bool Game::pieceCanGotoAnyPosition(const Piece *piece, const FieldSet &positions) const {
  if(piece->getType() != Pawn) {
    return pieceAttacksAnyPosition(piece, positions);
  } else { // Pawn
    const DirectionArray &da        = piece->m_moveTable[piece->m_position];
    const Direction      *direction = da.m_directions;
    const int             pawnCol   = GETCOL(piece->m_position);
    for(int d = da.m_count; d--; direction++) {
      PositionArray pa = direction->m_fields;
      for(int count  = *(pa++); count--;) {
        const int to = *(pa++);
        if((GETCOL(to) == pawnCol) && positions.contains(to)) {
          return true;
        }
        if(m_board[to]) {
          break;
        }
      }
    }
  }
  return false;
}

bool Game::pieceAttacksAnyPosition(const Piece *piece, const FieldSet &positions) const {
  switch(piece->getType()) {
  case King  :
  case Queen :
  case Rook  :
  case Bishop:
  case Knight:
    { const DirectionArray &da        = piece->m_moveTable[piece->m_position];
      const Direction      *direction = da.m_directions;
      for(int d = da.m_count; d--; direction++) {
        PositionArray pa = direction->m_fields;
        for(int count  = *(pa++); count--;) {
          const int to = *(pa++);
          if(positions.contains(to)) {
            return true;
          }
          if(m_board[to]) {
            break;
          }
        }
      }
    }
    break;
  case Pawn:
    { PositionArray pa = (piece->getPlayer()==WHITEPLAYER)
                       ? s_fieldInfo[piece->m_position].m_whitePawnAttacks
                       : s_fieldInfo[piece->m_position].m_blackPawnAttacks;
      for(int count = *(pa++); count--;) {
        if(positions.contains(*(pa++))) {
          return true;
        }
      }
      if(m_gameKey.getEPSquare() > 0
      && positions.contains(m_gameKey.getEPSquare())
      && GETROW(piece->m_position) == GETROW(m_gameKey.getEPSquare())
      && getWalkDistance(piece->m_position,m_gameKey.getEPSquare()) == 1) {
        return true;
      }
    }
    break;
  }
  return false;
}

static FieldSet &followPositions(FieldSet &result, PositionArray pa, int stop) {
  for(int count = *(pa++); count--;) {
    const int pos = *(pa++);
    if(pos == stop) {
      break;
    }
    result.add(pos);
  }
  return result;
}

MoveDirection getMoveDirection(int from, int to) {
  if(from == to) {
    return MD_NONE;
  } else if(GETROW(  from)  == GETROW(  to)) {
    return (GETCOL(  from)  <  GETCOL(  to)) ? MD_RIGHT   : MD_LEFT;
  } else if(GETCOL(  from)  == GETCOL(  to)) {
    return (GETROW(  from)  <  GETROW(  to)) ? MD_UP      : MD_DOWN;
  } else if(GETDIAG1(from)  == GETDIAG1(to)) {
    return (GETDIAG2(from)  <  GETDIAG2(to)) ? MD_UPDIAG1 : MD_DOWNDIAG1;
  } else if(GETDIAG2(from)  == GETDIAG2(to)) {
    return (GETDIAG1(from)  <  GETDIAG1(to)) ? MD_UPDIAG2 : MD_DOWNDIAG2;
  } else {
    return MD_NONE;
  }
}

static FieldSet calculateFieldsBetween(int pos1, int pos2) {
  FieldSet result;
  switch(getMoveDirection(pos1, pos2)) {
  case MD_NONE      : return result;
  case MD_LEFT      : return followPositions(result, Game::s_fieldInfo[pos1].m_rowLine.m_lower  , pos2);
  case MD_RIGHT     : return followPositions(result, Game::s_fieldInfo[pos1].m_rowLine.m_upper  , pos2);
  case MD_DOWN      : return followPositions(result, Game::s_fieldInfo[pos1].m_colLine.m_lower  , pos2);
  case MD_UP        : return followPositions(result, Game::s_fieldInfo[pos1].m_colLine.m_upper  , pos2);
  case MD_DOWNDIAG1 : return followPositions(result, Game::s_fieldInfo[pos1].m_diag1Line.m_lower, pos2);
  case MD_UPDIAG1   : return followPositions(result, Game::s_fieldInfo[pos1].m_diag1Line.m_upper, pos2);
  case MD_DOWNDIAG2 : return followPositions(result, Game::s_fieldInfo[pos1].m_diag2Line.m_lower, pos2);
  case MD_UPDIAG2   : return followPositions(result, Game::s_fieldInfo[pos1].m_diag2Line.m_upper, pos2);
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("pos1=%s, pos2=%s"), getFieldName(pos1), getFieldName(pos2));
                      return result;
  }
}

#if defined(TABLEBASE_BUILDER)

class FieldSetMatrix { // precalculated matrix of FieldSets to speed up things a bit when calling getFieldsBetween
private:
  FieldSet m_emptySet;
public:
  FieldSet *sets[64][64];
  FieldSetMatrix();
 ~FieldSetMatrix();
};

FieldSetMatrix::FieldSetMatrix() {
  memset(sets,0,sizeof(sets));
  for(int p1 = 0; p1 < 64; p1++) {
    for(int p2 = 0; p2 < 64; p2++) {
      if(p1 == p2) {
        sets[p1][p2] = &m_emptySet;
        continue;
      }
      const FieldSet s = calculateFieldsBetween(p1, p2);
      if(s.isEmpty()) {
        sets[p1][p2] = &m_emptySet;
      } else if(sets[p2][p1] != nullptr) {
        sets[p1][p2] = sets[p2][p1];
      } else {
        sets[p1][p2] = new FieldSet(s); TRACE_NEW(sets[p1][p2]);
      }
    }
  }
}

FieldSetMatrix::~FieldSetMatrix() {
  for(int p1 = 0; p1 < 64; p1++) {
    for(int p2 = 0; p2 < 64; p2++) {
      FieldSet *s = sets[p1][p2];
      if(s  && (s != &m_emptySet)) {
        SAFEDELETE(s);
      }
      sets[p1][p2] = sets[p2][p1] = nullptr;
    }
  }
}

const FieldSet &getFieldsBetween(int pos1, int pos2) {
  static FieldSetMatrix matrix;
  return *matrix.sets[pos1][pos2];
}

#else

FieldSet getFieldsBetween(int pos1, int pos2) {
  return calculateFieldsBetween(pos1,pos2);
}

#endif
