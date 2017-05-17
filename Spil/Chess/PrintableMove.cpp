#include "stdafx.h"

PrintableMove::PrintableMove(const Game &game, const MoveBase &m) : MoveBase(m) {
  if(m_type == NOMOVE) {
    setNoMove();
  } else {
    const Move tmpMove  = game.generateMove(m);
    m_player            = game.getPlayerInTurn();
    m_pieceType         = tmpMove.getPieceType();
    m_capturedPieceType = tmpMove.getCapturedPieceType();

    findUniqueString(game);
    findKingAttackState((Game&)game, tmpMove);
  }
}
/*
PrintableMove::PrintableMove(const Game &game, const String &uciString) {
  if (uciString == _T("0000") || uciString.length() == 0) {
    setNoMove();
    return;
  }
  try {
    m_annotation = NOANNOTATION;
    m_from       = decodePosition(uciString.cstr());
    m_to         = decodePosition(uciString.cstr()+2);
    m_player     = game.getPlayerInTurn();
    m_pieceType  = game.getPieceAtPosition(m_from)->getType();
    switch (m_pieceType) {
    case King:
      if(Game::getWalkDistance(m_from, m_to) == 2) {
        m_type = (GETCOL(m_to) == 6) ? SHORTCASTLING : LONGCASTLING;
      } else {
        m_type = NORMALMOVE;
      }
      break;
    case Pawn:
      if(GETROW(m_to) == 0 || GETROW(m_to) == 7) {
        m_type = PROMOTION;
        switch (uciString[4]) {
        case 'q': m_promoteIndex = Game::getPromoteIndex(Queen ); break;
        case 'n': m_promoteIndex = Game::getPromoteIndex(Knight); break;
        case 'r': m_promoteIndex = Game::getPromoteIndex(Rook  ); break;
        case 'b': m_promoteIndex = Game::getPromoteIndex(Bishop); break;
        default : throwException(_T("invalid promotion:\"%s\""), uciString.cstr());
        }
        if (GETCOL(m_to) == GETCOL(m_from)) {
          m_capturedPieceType = NoPiece;
        } else {
          m_capturedPieceType = game.getPieceAtPosition(m_to)->getType();
        }
      } else if (GETCOL(m_to) == GETCOL(m_from)) {
        m_type              = NORMALMOVE;
        m_capturedPieceType = NoPiece;
      } else {
        const Piece *cp = game.getPieceAtPosition(m_to);
        if (cp) {
          m_type              = NORMALMOVE;
          m_capturedPieceType = cp->getType();
        } else {
          m_type              = ENPASSANT;
          m_capturedPieceType = Pawn;
        }
      }
      break;
    default:
      m_type = NORMALMOVE;
      const Piece *cp = game.getPieceAtPosition(m_to);
      m_capturedPieceType = cp ? cp->getType() : NoPiece;
      break;
    }
  } catch (Exception e) {
    *this = game.generateMove(uciString, MOVE_UCIFORMAT);
  }
}
*/

PrintableMove &PrintableMove::setNoMove() {
  MoveBase::setNoMove();
  m_player            = WHITEPLAYER;
  m_pieceType         = NoPiece;
  m_capturedPieceType = NoPiece;
  m_uniqueString      = EMPTYSTRING;
  m_kingAttackState   = KING_NOT_ATTACKED;
  m_checkMate         = false;
  return *this;
}

void PrintableMove::findUniqueString(const Game &game) {
  switch(m_pieceType) {
  case King:
  case Pawn:
    break;

  default  :
    { Move move;
      UINT sameRowCount     = 0;
      UINT sameColCount     = 0;
      UINT alternativeCount = 0;
      MoveGenerator &mg = game.getMoveGenerator();
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

void PrintableMove::findKingAttackState(Game &game, const Move &m) {
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

bool PrintableMove::operator==(const PrintableMove &m) const {
  return MoveBase::operator ==(m)
      && m_annotation        == m.m_annotation
      && m_player            == m.m_player
      && m_pieceType         == m.m_pieceType
      && m_capturedPieceType == m.m_capturedPieceType
      && m_uniqueString      == m.m_uniqueString
      && m_kingAttackState   == m.m_kingAttackState
      && m_checkMate         == m.m_checkMate;
}


String PrintableMove::toString(MoveStringFormat mf) const {
  switch(mf) {
  case MOVE_SHORTFORMAT : return toStringShortFormat()   + getAnnotationToString(m_annotation);
  case MOVE_LONGFORMAT  : return toStringLongFormat()    + getAnnotationToString(m_annotation);
  case MOVE_FILEFORMAT  : return toStringFileFormat()    + getAnnotationToString(m_annotation);
  case MOVE_UCIFORMAT   : return toStringUCIFormat();
  case MOVE_DEBUGFORMAT : return toStringDebugFormat()   + getAnnotationToString(m_annotation);
  default               : throwInvalidArgumentException(__TFUNCTION__, _T("mf=%d"), mf);
                          return EMPTYSTRING;
  }
}

#define CAPTURE_DELIMITER_STRING _T("x")

String PrintableMove::toStringShortFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return s_nomoveString;

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
                     ,getPieceTypeShortName(m_pieceType)
                     ,m_uniqueString.cstr()
                     ,getFieldName(m_to)
                     ,getCheckString()
                     );
      } else {
        return format(_T("%s%s%s%s")
                     ,getPieceTypeShortName(m_pieceType)
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
                   ,getPieceTypeShortName(getPromoteTo())
                   ,getCheckString()
                   );
    } else {
      return format(_T("%s%s%s")
                   ,getFieldName(m_to)
                   ,getPieceTypeShortName(getPromoteTo())
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

String PrintableMove::toStringLongFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return s_nomoveString;

  case NORMALMOVE   :
  case PROMOTION    :
    { String result = format(_T("%s%s%c%s")
                            ,getPieceTypeShortName(m_pieceType)
                            ,getFieldName(m_from)
                            ,(m_capturedPieceType != NoPiece) ? 'x' : '-'
                            ,getFieldName(m_to));
      if(m_type == PROMOTION) {
        result += getPieceTypeShortName(Game::s_legalPromotions[m_promoteIndex]);
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

String PrintableMove::toStringDebugFormat() const {
  switch(m_type) {
  case NOMOVE       :
    return s_nomoveString;

  case NORMALMOVE    :
    return format(_T("Type:%s, %s%s%s%s%s, dirIndex:%d, moveIndex:%d, dir:%s")
                 ,getMoveTypeName(m_type).cstr()
                 ,getPieceTypeShortName(m_pieceType)
                 ,getFieldName(m_from)
                 ,(m_capturedPieceType != NoPiece) ? CAPTURE_DELIMITER_STRING : _T("-")
                 ,format(_T("%s%s"),((m_capturedPieceType != NoPiece)?getPieceTypeShortName(m_capturedPieceType):EMPTYSTRING)
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
                 ,format(_T("%s%s"),((m_capturedPieceType != NoPiece)?getPieceTypeShortName(m_capturedPieceType):EMPTYSTRING)
                                   ,getFieldName(m_to)).cstr()
                 ,getPieceTypeShortName(Game::s_legalPromotions[m_promoteIndex])
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
                 ,getPieceTypeShortName(m_pieceType)
                 ,getFieldName(m_from)
                 ,getFieldName(m_to)
                 ,getMoveDirectionName(m_direction).cstr());

  default:
    return format(_T("Unknown movetype:%d"), m_type);
  }
}

const TCHAR *PrintableMove::getCheckString() const {
  if(m_checkMate) {
    return _T("#");
  }
  switch(m_kingAttackState) {
  case KING_NOT_ATTACKED           : return EMPTYSTRING;
  case KING_LD_ATTACKED_FROM_ROW   :
  case KING_LD_ATTACKED_FROM_COL   :
  case KING_LD_ATTACKED_FROM_DIAG1 :
  case KING_LD_ATTACKED_FROM_DIAG2 :
  case KING_SD_ATTACKED            : return _T("+");
  default                          : return _T("++");
  }
}
