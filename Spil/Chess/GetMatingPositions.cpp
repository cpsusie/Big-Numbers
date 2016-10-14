#include "stdafx.h"

FieldSet Game::getMatingPositions() const {
  FieldSet result;
  if(!isCheckmate()) {
    return result;
  }
  const int    kingPos = getKingPosition(PLAYERINTURN);
  const Player winner  = CURRENTENEMY;
  if(isDoubleCheck()) {
    result = findAttackingPiecePositions(winner, kingPos);
  } else {
    result.add(findAttackingPiece(winner, kingPos, &result)->getPosition());
  }
  PositionArray pa = fieldInfo[kingPos].m_kingAttacks;
  for(int count = *(pa++); count--;) {
    const int    squareNextToKing = *(pa++);
    const Piece *p = m_board[squareNextToKing];
    if(p && p->getPlayer() == PLAYERINTURN) {
      continue; // king cannot go to this field, because one of his own pieces occupy the field
    }
    if((p = findAttackingPiece(winner, squareNextToKing, NULL)) != NULL) {
      result.add(p->getPosition());
    }
  }
  return result;
}

const Piece *Game::findAttackingPiece(Player player, int pos, FieldSet *result) const {
  for(const Piece *piece = m_playerState[player].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    if(pieceAttacksPosition(piece, pos)) {
      if(result) {
        FieldSet pinningPiecePositions;
        for(const Piece *defender = m_playerState[GETENEMY(player)].m_first; defender; defender = defender->m_next) {
          if(!defender->m_onBoard || (defender->getType() == King) || !pieceAttacksPosition(defender, piece->getPosition())) {
            continue;
          }
          const Piece *pinningPiece = findPinningPiece(defender, piece->m_position);
          if(pinningPiece == NULL) {
            goto NextPiece; // found a not pinned defender
          }
          pinningPiecePositions.add(pinningPiece->m_position);
        }
        if(piece->m_attackAttribute & ATTACKS_LONGDISTANCE) { // Checks from Queen, Rook and Bishop can be defended 
                                                              // by blocking one of the squares between king and the attacking piece
          const FieldSet fieldsBetween = getFieldsBetween(pos, piece->m_position);
          for(Iterator<int> it = fieldsBetween.getIterator(); it.hasNext();) {
            const int pos1 = it.next();
            for(const Piece *defender = m_playerState[GETENEMY(player)].m_first; defender; defender = defender->m_next) {
              if(!defender->m_onBoard || (defender->getType() == King) || !pieceCanGotoPosition(defender, pos1)) {
                continue;
              }
              const Piece *pinningPiece = findPinningPiece(defender, pos1);
              if(pinningPiece == NULL) {
                goto NextPiece;
              }
              pinningPiecePositions.add(pinningPiece->m_position);
            }
          }
        }
        *result += pinningPiecePositions;
      }
      return piece;
    }
NextPiece:;
  }
  if(result) {
    throwException(_T("findAttackingPiece:No %s piece is attacking %s"), getPlayerNameEnglish(player), getFieldName(pos));
  }
  return NULL;
}

const Piece *Game::findPinningPiece(const Piece *piece, int wantedDestination) const {
  if(piece->m_pinnedState == NOT_PINNED) {
    return NULL;
  }
  switch(piece->getType()) { // cannot be King
  case Queen :
  case Rook  :
  case Bishop:
  case Pawn  :
    if(!MoveTable::uncoversKing[piece->m_pinnedState][getMoveDirection(piece->m_position, wantedDestination)]) {
      return NULL;
    }
    // continue case
  case Knight:
    return findFirstPieceInDirection(piece->m_position
                                    ,getOppositeDirection(KING_DIRECTION(piece->m_playerState, piece->m_position)));
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("piece.type=%d"), piece->getType());
    return NULL;
  }
}

FieldSet Game::findAttackingPiecePositions(Player player, int pos) const {
  FieldSet result;
  for(const Piece *piece = m_playerState[player].m_first; piece; piece = piece->m_next) {
    if(!piece->m_onBoard) {
      continue;
    }
    if(pieceAttacksPosition(piece, pos)) {
      result.add(piece->m_position);
    }
  }
  return result;
}

bool Game::pieceCanGotoPosition(const Piece *piece, int pos) const {
  FieldSet fieldSet;
  fieldSet.add(pos);
  return pieceCanGotoAnyPosition(piece, fieldSet);
}

bool Game::pieceAttacksPosition(const Piece *piece, int pos) const {
  FieldSet fieldSet;
  fieldSet.add(pos);
  return pieceAttacksAnyPosition(piece, fieldSet);
}

