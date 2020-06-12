#include "stdafx.h"

const String MoveBase::s_nomoveString = _T("Nomove");

MoveBase &MoveBase::setNoMove() {
  m_type          = NOMOVE;
  m_dirIndex      = 0;
  m_moveIndex     = 0;
  m_promoteIndex  = 0;
  m_from          = 0;
  m_to            = 0;
  m_direction     = MD_NONE;
  m_annotation    = NOANNOTATION;
  return *this;
}

PieceType MoveBase::getPromoteTo() const {
  return (m_type == PROMOTION) ? Game::s_legalPromotions[m_promoteIndex] : NoPiece;
}

bool MoveBase::operator==(const MoveBase &m) const { // compare only from,to, not annotations
  if(m_type != m.m_type) {
    return false;
  } else if(m_type == PROMOTION) {
    return (m_from == m.m_from) && (m_to == m.m_to) && (m_promoteIndex == m.m_promoteIndex);
  } else {
    return (m_from == m.m_from) && (m_to == m.m_to);
  }
}

bool MoveBase::operator!=(const MoveBase &m) const {
  return !(*this == m);
}

bool MoveBase::isGoodMove() const {
  switch(m_annotation) {
  case NOANNOTATION   :
  case INTERESTING_MOVE:
  case GOOD_MOVE       :
  case EXCELLENT_MOVE  :
    return true;

  case BAD_MOVE        :
  case VERYBAD_MOVE    :
  case DOUBIOUS_MOVE   :
    return false;

  default              :
    throwException(_T("%s:Unknown annotation:%d"), __TFUNCTION__, m_annotation);
    return false;
  }
}

#if defined(TABLEBASE_BUILDER)
MoveBase &MoveBase::swapFromTo() {
  if(isMove()) {
    int tmp     = m_from;
    m_from      = m_to;
    m_to        = tmp;
    m_direction = getOppositeDirection(m_direction);
  }
  return *this;
}
#endif

MoveBase MoveBase::transform(const Game &game, bool swapPlayers) const {
  if(!isMove() || !swapPlayers) {
    return *this;
  }
  const int from = GameKey::transform(m_from, TRANSFORM_SWAPPLAYERS);
  const int to   = GameKey::transform(m_to  , TRANSFORM_SWAPPLAYERS);
  return game.generateMove(from, to, getPromoteTo());
}

String MoveBase::toString() const {
  return toStringFileFormat();
}

String MoveBase::toSimpleFormat(bool uciFormat) const {
  switch(m_type) {
  case NOMOVE       :
    return uciFormat ? _T("0000") : s_nomoveString;

  case PROMOTION    :
    return format(_T("%s%s%s%s")
                 ,getFieldName(m_from)
                 ,uciFormat?EMPTYSTRING:_T("-")
                 ,getFieldName(m_to)
                 ,getPieceTypeShortNameEnglish(Game::s_legalPromotions[m_promoteIndex]));

  default:
    return format(_T("%s%s%s"), getFieldName(m_from), uciFormat?EMPTYSTRING:_T("-"), getFieldName(m_to));
  }
}

int MoveBase::getMaxStringLength(MoveStringFormat mf) { // static
  static const int maxLength[] = {
     12           // MOVE_SHORTFORMAT
    ,14           // MOVE_LONGFORMAT
    ,8            // MOVE_FILEFORMAT
    ,6            // MOVE_UCIFORMAT
    ,61           // MOVE_DEBUGFORMAT
  };
  return maxLength[mf];
}

Packer &operator<<(Packer &p, const MoveBase &m) {
  p << (int)m.m_type
    << m.m_from
    << m.m_to
    << m.m_dirIndex
    << m.m_moveIndex
    << m.m_promoteIndex
    << (int)m.m_direction
    << (int)m.m_annotation;
  return p;
}

Packer &operator>>(Packer &p, MoveBase &m) {
  int type, dir, anno;
  p >> type
    >> m.m_from
    >> m.m_to
    >> m.m_dirIndex
    >> m.m_moveIndex
    >> m.m_promoteIndex
    >> dir
    >> anno;
  m.m_type       = (MoveType)type;
  m.m_direction  = (MoveDirection )dir;
  m.m_annotation = (MoveAnnotation)anno;
  return p;
}
