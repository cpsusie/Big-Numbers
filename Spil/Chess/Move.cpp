#include "stdafx.h"

static const String nomoveString = _T("Nomove");

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
  return (m_type == PROMOTION) ? Game::legalPromotions[m_promoteIndex] : NoPiece;
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

#ifdef TABLEBASE_BUILDER
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

String MoveBase::toString() const {
  return toStringFileFormat();
}

String MoveBase::toSimpleFormat(bool uciFormat) const {
  switch(m_type) {
  case NOMOVE       :
    return uciFormat ? _T("0000") : nomoveString;

  case PROMOTION    :
    return format(_T("%s%s%s%s")
                 ,getFieldName(m_from)
                 ,uciFormat?_T(""):_T("-")
                 ,getFieldName(m_to)
                 ,getPieceTypeShortNameEnglish(Game::legalPromotions[m_promoteIndex]));

  default:
    return format(_T("%s%s%s"), getFieldName(m_from), uciFormat?_T(""):_T("-"), getFieldName(m_to));
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



// -------------------------------------- Move --------------------------------

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
                          return _T("");
  }
}

String Move::toStringLongFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return nomoveString;

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
    return _T("");
  }
}

String Move::toStringDebugFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return nomoveString;

  case NORMALMOVE    :
    return format(_T("Type:%s, %s%s%c%s, dirIndex:%d, moveIndex:%d, dir:%s") // maxLength 52 (=5+10+2+1+2+1+3+12+1+6+9)
                  ,getMoveTypeName(m_type).cstr()
                  ,m_piece->getShortName().cstr()
                  ,getFieldName(m_from)
                  ,m_capturedPiece ? 'x' : '-'
                  ,format(_T("%s%s"),(m_capturedPiece?m_capturedPiece->getShortName().cstr():_T("")),getFieldName(m_to)).cstr()
                  ,m_dirIndex
                  ,m_moveIndex
                  ,getMoveDirectionName(m_direction).cstr());

  case PROMOTION     :
    return format(_T("Type:%s, %s%c%s%s, dirIndex:%d, moveIndex:%d, dir:%s") // maxLength 59 (=5+9+2+2+1+3+1+11+1+12+1+2+9)
                  ,getMoveTypeName(m_type).cstr()
                  ,getFieldName(m_from)
                  ,m_capturedPiece ? 'x' : '-'
                  ,format(_T("%s%s"),(m_capturedPiece?m_capturedPiece->getShortName().cstr():_T("")),getFieldName(m_to)).cstr()
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
    return _T("");
  }
}

// ------------------------ ExecutableMove --------------------------------------

ExecutableMove::ExecutableMove(const Game &game, const Move &m) : MoveBase(m) {
  if(m_type == NOMOVE) {
    setNoMove();
  } else {
    m_player            = game.getPlayerInTurn();
    m_pieceType         = m.m_piece->getType();
    m_capturedPieceType = m.m_capturedPiece ? m.m_capturedPiece->getType() : NoPiece;
    findUniqueString(game);
    findKingAttackState((Game&)game, m);
  }
}

ExecutableMove &ExecutableMove::setNoMove() {
  MoveBase::setNoMove();
  m_player            = WHITEPLAYER;
  m_pieceType         = NoPiece;
  m_capturedPieceType = NoPiece;
  m_uniqueString      = _T("");
  m_kingAttackState   = KING_NOT_ATTACKED;
  m_checkMate         = false;
  return *this;
}

void ExecutableMove::findUniqueString(const Game &game) {
  switch(m_pieceType) {
  case King:
  case Pawn:
    break;

  default  :
    { MoveGenerator &mg = game.getMoveGenerator();
      Move move;
      UINT sameRowCount     = 0;
      UINT sameColCount     = 0;
      UINT alternativeCount = 0;
      for(bool b = mg.firstMove(move); b; b = mg.nextMove(move)) {
        if(move.m_to   == m_to
        && move.m_from != m_from
        && move.m_piece->getType() == m_pieceType) {
          alternativeCount++;
          if(GETROW(move.m_from) == GETROW(m_from)) {
            sameRowCount++;
          } else if(GETCOL(move.m_from) == GETCOL(m_from)) {
            sameColCount++;
          }
        }
      }
      switch(alternativeCount) {
      case 0:
        return;
      case 1:
        m_uniqueString = format(_T("%c"), sameColCount ? getRowName(m_from) : getColumnName(m_from));
        break;
      default:
        if(sameColCount == 0) {
          m_uniqueString = format(_T("%c"), getColumnName(m_from));
        } else if(sameRowCount == 0) {
          m_uniqueString = format(_T("%c"), getRowName(m_from));
        } else {
          m_uniqueString = getFieldName(m_from);
        }
        break;
      }
    }
    break;
  }
}

void ExecutableMove::findKingAttackState(Game &game, const Move &m) {
  game.doMove(m);
  m_kingAttackState = game.getKingAttackState();
  switch(game.findGameResult()) {
  case WHITE_CHECKMATE:
  case BLACK_CHECKMATE:
    m_checkMate = true;
    break;

  default             :
    m_checkMate = false;
    break;
  }
  game.undoMove();
}

bool ExecutableMove::operator==(const ExecutableMove &m) const {
  return MoveBase::operator ==(m)
      && m_annotation        == m.m_annotation
      && m_player            == m.m_player
      && m_pieceType         == m.m_pieceType
      && m_capturedPieceType == m.m_capturedPieceType
      && m_uniqueString      == m.m_uniqueString
      && m_kingAttackState   == m.m_kingAttackState
      && m_checkMate         == m.m_checkMate;
}


String ExecutableMove::toString(MoveStringFormat mf) const {
  switch(mf) {
  case MOVE_SHORTFORMAT : return toStringShortFormat()   + getAnnotationToString(m_annotation);
  case MOVE_LONGFORMAT  : return toStringLongFormat()    + getAnnotationToString(m_annotation);
  case MOVE_FILEFORMAT  : return toStringFileFormat()    + getAnnotationToString(m_annotation);
  case MOVE_UCIFORMAT   : return toStringUCIFormat();
  case MOVE_DEBUGFORMAT : return toStringDebugFormat()   + getAnnotationToString(m_annotation);
  default               : throwInvalidArgumentException(__TFUNCTION__, _T("mf=%d"), mf);
                          return _T("");
  }
}

#define CAPTURE_DELIMITER_STRING _T("x")

String ExecutableMove::toStringShortFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return nomoveString;

  case NORMALMOVE   :
    if(m_pieceType == Pawn) {
      if(m_capturedPieceType != NoPiece) {
        return format(_T("%cx%s%s")
                     ,getColumnName(m_from)
                     ,getFieldName(m_to)
                     ,getCheckString()
                     );
      } else {
        return format(_T("%s%s")
                     ,getFieldName(m_to)
                     ,getCheckString()
                     );
      }
    } else {
      if(m_capturedPieceType != NoPiece) {
        return format(_T("%s%sx%s%s")
                     ,getPieceTypeShortName(m_pieceType).cstr()
                     ,m_uniqueString.cstr()
                     ,getFieldName(m_to)
                     ,getCheckString()
                     );
      } else {
        return format(_T("%s%s%s%s")
                     ,getPieceTypeShortName(m_pieceType).cstr()
                     ,m_uniqueString.cstr()
                     ,getFieldName(m_to)
                     ,getCheckString()
                     );
      }
    }

  case PROMOTION    :
    if(m_capturedPieceType != NoPiece) {
      return format(_T("%cx%s%s%s")
                   ,getColumnName(m_from)
                   ,getFieldName(m_to)
                   ,getPieceTypeShortName(getPromoteTo()).cstr()
                   ,getCheckString()
                   );
    } else {
      return format(_T("%s%s%s")
                   ,getFieldName(m_to)
                   ,getPieceTypeShortName(getPromoteTo()).cstr()
                   ,getCheckString()
                   );
    }

  case ENPASSANT    :
    return format(_T("%cx%s e.p.%s")
                 ,getColumnName(m_from)
                 ,getFieldName(m_to)
                 ,getCheckString()
                 );

  case SHORTCASTLING:
  case LONGCASTLING :
    return format(_T("%s%s"), getCastleString(m_type).cstr(), getCheckString());

  default:
    return format(_T("Unknown moveType (=%d)"),m_type);
  }
}

String ExecutableMove::toStringLongFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return nomoveString;

  case NORMALMOVE   :
  case PROMOTION    :
    { String result = format(_T("%s%s%c%s")
                            ,getPieceTypeShortName(m_pieceType).cstr()
                            ,getFieldName(m_from)
                            ,(m_capturedPieceType != NoPiece) ? 'x' : '-'
                            ,getFieldName(m_to));
      if(m_type == PROMOTION) {
        result += getPieceTypeShortName(Game::legalPromotions[m_promoteIndex]);
      }
      result += getCheckString();
      return result;
    }

  case ENPASSANT    :
    return format(_T("%s%s%s e.p.%s")
                  ,getFieldName(m_from)
                  ,CAPTURE_DELIMITER_STRING
                  ,getFieldName(m_to)
                  ,getCheckString()
                 );

  case SHORTCASTLING:
  case LONGCASTLING :
    return format(_T("%s%s"), getCastleString(m_type).cstr(), getCheckString());

  default:
    return format(_T("Unknown moveType (=%d)"),m_type);
  }
}

String ExecutableMove::toStringDebugFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return nomoveString;

  case NORMALMOVE    :
    return format(_T("Type:%s, %s%s%s%s%s, dirIndex:%d, moveIndex:%d, dir:%s")
                  ,getMoveTypeName(m_type).cstr()
                  ,getPieceTypeShortName(m_pieceType).cstr()
                  ,getFieldName(m_from)
                  ,(m_capturedPieceType != NoPiece) ? CAPTURE_DELIMITER_STRING : _T("-")
                  ,format(_T("%s%s"),((m_capturedPieceType != NoPiece)?getPieceTypeShortName(m_capturedPieceType).cstr():_T(""))
                                    ,getFieldName(m_to)).cstr()
                  ,getCheckString()
                  ,m_dirIndex
                  ,m_moveIndex
                  ,getMoveDirectionName(m_direction).cstr());

  case PROMOTION     :
    return format(_T("Type:%s, %s%s%s%s%s, dirIndex:%d, moveIndex:%d, dir:%s")
                  ,getMoveTypeName(m_type).cstr()
                  ,getFieldName(m_from)
                  ,(m_capturedPieceType != NoPiece) ? CAPTURE_DELIMITER_STRING : _T("-")
                  ,format(_T("%s%s"),((m_capturedPieceType != NoPiece)?getPieceTypeShortName(m_capturedPieceType).cstr():_T(""))
                                    ,getFieldName(m_to)).cstr()
                  ,getPieceTypeShortName(Game::legalPromotions[m_promoteIndex]).cstr()
                  ,getCheckString()
                  ,m_dirIndex
                  ,m_moveIndex
                  ,getMoveDirectionName(m_direction).cstr());

  case ENPASSANT     :
    return format(_T("Type:%s, %s%s%s e.p.%s dir:%s")
                  ,getMoveTypeName(m_type).cstr()
                  ,getFieldName(m_from)
                  ,CAPTURE_DELIMITER_STRING
                  ,getFieldName(m_to)
                  ,getCheckString()
                  ,getMoveDirectionName(m_direction).cstr());

  case SHORTCASTLING :
  case LONGCASTLING  :
    return format(_T("Type:%s, %s%s %s%s-%s dir:%s")
                  ,getMoveTypeName(m_type).cstr()
                  ,getCastleString(m_type).cstr()
                  ,getCheckString()
                  ,getPieceTypeShortName(m_pieceType).cstr()
                  ,getFieldName(m_from)
                  ,getFieldName(m_to)
                  ,getMoveDirectionName(m_direction).cstr());

  default:
    return format(_T("Unknown movetype:%d"),m_type);
  }
}

const TCHAR *ExecutableMove::getCheckString() const {
  if(m_checkMate) {
    return _T("#");
  }
  switch(m_kingAttackState) {
  case KING_NOT_ATTACKED           : return _T("");
  case KING_LD_ATTACKED_FROM_ROW   :
  case KING_LD_ATTACKED_FROM_COL   :
  case KING_LD_ATTACKED_FROM_DIAG1 :
  case KING_LD_ATTACKED_FROM_DIAG2 :
  case KING_SD_ATTACKED            : return _T("+");
  default                          : return _T("++");
  }
}
