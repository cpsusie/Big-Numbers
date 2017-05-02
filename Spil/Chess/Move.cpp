#include "stdafx.h"

const bool MoveTable::isDiagonalMove[9] = { // Indexed by MoveDirection;
  false, false, false, false, false, true, true, true, true
};

const bool MoveTable::uncoversKing[5][9] = { // Indexed by PinnedStatus,MoveDirection
  /* NOT_PINNED          */  { false, false, false, false, false, false, false, false, false }
  /* PINNED_TO_ROW       */ ,{ true , false, false, true , true , true , true , true , true  }
  /* PINNED_TO_COL       */ ,{ true , true , true , false, false, true , true , true , true  }
  /* PINNED_TO_DIAG1     */ ,{ true , true , true , true , true , false, false, true , true  }
  /* PINNED_TO_DIAG2     */ ,{ true , true , true , true , true , true , true , false, false }
};

Move &Move::setNoMove() {
  MoveBase::setNoMove();
  m_piece         = NULL;
  m_capturedPiece = NULL;
  return *this;
}

#ifdef TABLEBASE_BUILDER
Move Move::makeBackMove() const {
  Move result(*this);
  result.swapFromTo();
  return result;
}
#endif

String Move::toString(MoveStringFormat mf) const {
  switch(mf) {
  case MOVE_SHORTFORMAT :
  case MOVE_LONGFORMAT  : return toStringLongFormat();
  case MOVE_FILEFORMAT  : return toStringFileFormat();
  case MOVE_UCIFORMAT   : return toStringUCIFormat();
  case MOVE_DEBUGFORMAT : return toStringDebugFormat();
  default               : throwInvalidArgumentException(__TFUNCTION__, _T("mf=%d"), mf);
                          return EMPTYSTRING;
  }
}

String Move::toStringLongFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return s_nomoveString;

  case NORMALMOVE   :
  case PROMOTION    :
    { String result = format(_T("%s%s%c%s")
                            ,m_piece->getShortName().cstr()
                            ,getFieldName(m_from)
                            ,m_capturedPiece ? 'x' : '-'
                            ,getFieldName(m_to));
      if(m_type == PROMOTION) {
        result += getPieceTypeShortName(Game::legalPromotions[m_promoteIndex]);
      }
      return result;
    }

  case ENPASSANT    :
    return format(_T("%sx%s e.p."),getFieldName(m_from), getFieldName(m_to));

  case SHORTCASTLING:
  case LONGCASTLING :
    return getCastleString(m_type);

  default:
    throwException(_T("Unknown moveType:%d"),m_type);
    return EMPTYSTRING;
  }
}

String Move::toStringDebugFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return s_nomoveString;

  case NORMALMOVE    :
    return format(_T("Type:%s, %s%s%c%s, dirIndex:%d, moveIndex:%d, dir:%s") // maxLength 52 (=5+10+2+1+2+1+3+12+1+6+9)
                  ,getMoveTypeName(m_type).cstr()
                  ,m_piece->getShortName().cstr()
                  ,getFieldName(m_from)
                  ,m_capturedPiece ? 'x' : '-'
                  ,format(_T("%s%s"),(m_capturedPiece?m_capturedPiece->getShortName().cstr():EMPTYSTRING),getFieldName(m_to)).cstr()
                  ,m_dirIndex
                  ,m_moveIndex
                  ,getMoveDirectionName(m_direction).cstr());

  case PROMOTION     :
    return format(_T("Type:%s, %s%c%s%s, dirIndex:%d, moveIndex:%d, dir:%s") // maxLength 59 (=5+9+2+2+1+3+1+11+1+12+1+2+9)
                  ,getMoveTypeName(m_type).cstr()
                  ,getFieldName(m_from)
                  ,m_capturedPiece ? 'x' : '-'
                  ,format(_T("%s%s"),(m_capturedPiece?m_capturedPiece->getShortName().cstr():EMPTYSTRING),getFieldName(m_to)).cstr()
                  ,getPieceTypeShortName(Game::legalPromotions[m_promoteIndex]).cstr()
                  ,m_dirIndex
                  ,m_moveIndex
                  ,getMoveDirectionName(m_direction).cstr());

  case ENPASSANT     :
    return format(_T("Type:%s, %sx%s e.p. dir:%s")                           // maxLength 31 (=5+9+2+2+1+2+10+0)
                  ,getMoveTypeName(m_type).cstr()
                  ,getFieldName(m_from)
                  ,getFieldName(m_to)
                  ,getMoveDirectionName(m_direction).cstr());

  case SHORTCASTLING :
  case LONGCASTLING  :
    return format(_T("Type:%s, %s %s%s-%s dir:%s")                           // maxLength 42 (=5+13+2+5+1+1+2+1+2+5+5)
                  ,getMoveTypeName(m_type).cstr()
                  ,getCastleString(m_type).cstr()
                  ,m_piece->getShortName().cstr()
                  ,getFieldName(m_from)
                  ,getFieldName(m_to)
                  ,getMoveDirectionName(m_direction).cstr());

  default:
    throwException(_T("Unknown moveType:%d"),m_type);
    return EMPTYSTRING;
  }
}
