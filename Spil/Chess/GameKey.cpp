#include "stdafx.h"

int pieceTypeCmp(const PieceType &p1, const PieceType &p2) {
  return p1 - p2;
}

class PieceWithPosition {
public:
  PieceKey      m_pieceKey;
  unsigned char m_position;
  PieceWithPosition() {
  }
  PieceWithPosition(PieceKey pieceKey, unsigned char position);
  String toString() const;
};

PieceWithPosition::PieceWithPosition(PieceKey pieceKey, unsigned char position) {
  m_pieceKey = pieceKey;
  m_position = position;
}

String PieceWithPosition::toString() const {
  return format(_T("%s%s"),getPieceTypeShortNameEnglish(GET_TYPE_FROMKEY(m_pieceKey)), getFieldName(m_position));
}

static int pieceCmp(const PieceWithPosition &p1, const PieceWithPosition &p2) {
  int c = pieceTypeCmp(GET_TYPE_FROMKEY(p1.m_pieceKey), GET_TYPE_FROMKEY(p2.m_pieceKey));
  if(c) {
    return c;
  }
  return (int)p1.m_position - (int)p2.m_position;
}

GameKey GameKey::startupPosition;

const GameKey &GameKey ::getStartUpPosition() {
  static int initDone = false;
  if(!initDone) {
    startupPosition = Game().getKey();
    initDone = true;
  }
  return startupPosition;
}


void GameKey::clear(Player playerInTurn) {
  memset(this,0,sizeof(GameKey));
  d.m_playerInTurn = playerInTurn;
  d.m_EPSquare     = -1;
}

void GameKey::clearCastleState() {
  d.m_castleState[WHITEPLAYER] = d.m_castleState[BLACKPLAYER] = 0;
}

void GameKey::initCastleState() {
  d.m_castleState[WHITEPLAYER] = findCastleState(WHITEPLAYER);
  d.m_castleState[BLACKPLAYER] = findCastleState(BLACKPLAYER);
}

CastleState GameKey::findCastleState(Player player) const {
  switch(player) {
  case WHITEPLAYER: return (m_pieceKey[E1] != WHITEKING) 
                         ? 0 
                         : (((m_pieceKey[A1] == WHITEROOK) ? LONGCASTLE_ALLOWED  : 0)
                          | ((m_pieceKey[H1] == WHITEROOK) ? SHORTCASTLE_ALLOWED : 0));

  case BLACKPLAYER: return (m_pieceKey[E8] != BLACKKING) 
                         ? 0 
                         : (((m_pieceKey[A8] == BLACKROOK) ? LONGCASTLE_ALLOWED  : 0)
                          | ((m_pieceKey[H8] == BLACKROOK) ? SHORTCASTLE_ALLOWED : 0));

  default         : INVALIDPLAYERERROR(player);
  }
  return 0;
}

PositionSignature GameKey::getPositionSignature() const {
  PositionSignature result;
  for(int pos = 0; pos < 64; pos++) {
    const PieceKey pk = m_pieceKey[pos];
    if(pk != EMPTYPIECEKEY) {
      result.add(pk);
    }
  }
  return result;
}

String GameKey::toString() const {
  CompactArray<PieceWithPosition> playerPieces[2];
  for(int pos = 0; pos < 64; pos++) {
    const PieceKey pk = m_pieceKey[pos];
    if(pk != EMPTYPIECEKEY) {
      playerPieces[GET_PLAYER_FROMKEY(pk)].add(PieceWithPosition(pk, pos));
    }
  }
  String result;
  forEachPlayer(p) {
    CompactArray<PieceWithPosition> &a = playerPieces[p];
    a.sort(pieceCmp);
    result += getPlayerNameEnglish(p);
    const TCHAR *delimiter = _T(":");
    for(size_t i = 0; i < a.size(); i++, delimiter = _T(",")) {
      result += delimiter;
      result += a[i].toString();
    }
    result += _T(";\n");
  }
  result += format(_T("PlayerInTurn:%s;\n"), getPlayerNameEnglish(getPlayerInTurn()));
  return result;
}

TCHAR *GameKey::getFENBoardString(TCHAR *dst) const {
#define FLUSHEMPTY() if(emptyCount) { *(cp++) = '0' + emptyCount; emptyCount = 0; }
  TCHAR *cp = dst;
  for(int rank = 7; rank >= 0; rank--) {
    if(rank < 7) {
      *(cp++) = '/';
    }
    int emptyCount = 0;
    for(int file = 0; file < 8; file++) {
      TCHAR ch;
      switch(m_pieceKey[MAKE_POSITION(rank, file)]) {
      case NoPiece     : emptyCount++; continue;
      case WHITEKING   : ch = 'K'; break;
      case WHITEQUEEN  : ch = 'Q'; break;
      case WHITEROOK   : ch = 'R'; break;
      case WHITEBISHOP : ch = 'B'; break;
      case WHITEKNIGHT : ch = 'N'; break;
      case WHITEPAWN   : ch = 'P'; break;
      case BLACKKING   : ch = 'k'; break;
      case BLACKQUEEN  : ch = 'q'; break;
      case BLACKROOK   : ch = 'r'; break;
      case BLACKBISHOP : ch = 'b'; break;
      case BLACKKNIGHT : ch = 'n'; break;
      case BLACKPAWN   : ch = 'p'; break;
      }
      FLUSHEMPTY();
      *(cp++) = ch;
    }
    FLUSHEMPTY();
  }
  *(cp++) = 0;
  return dst;
}

TCHAR *GameKey::getFENCastleString(TCHAR *dst) const {
  TCHAR *cp = dst;
  if(d.m_castleState[0] == 0 && d.m_castleState[1] == 0) {
    *(cp++) = '-';
  } else {
    if(d.m_castleState[0] & SHORTCASTLE_ALLOWED) *(cp++) = 'K';
    if(d.m_castleState[0] & LONGCASTLE_ALLOWED ) *(cp++) = 'Q';
    if(d.m_castleState[1] & SHORTCASTLE_ALLOWED) *(cp++) = 'k';
    if(d.m_castleState[1] & LONGCASTLE_ALLOWED ) *(cp++) = 'q';
  }
  *(cp++) = 0;
  return dst;
}

TCHAR *GameKey::getFENEpString(TCHAR *dst) const {
  if(d.m_EPSquare < 0) {
    return _tcscpy(dst, _T("-"));
  } else {
    return _tcscpy(dst, getFieldName(d.m_EPSquare ^ 8));
  }
}

String GameKey::toFENString(int plyCountWithoutCaptureOrPawnMove, int moveNumber) const { // Forsyth-Edwards Notation
  TCHAR boardStr[300], castleStr[20], EpStr[10];
  return format(_T("%s %c %s %s %d %d")
               ,getFENBoardString(boardStr)
               ,(getPlayerInTurn() == WHITEPLAYER) ? 'w' : 'b'
               ,getFENCastleString(castleStr)
               ,getFENEpString(EpStr)
               ,plyCountWithoutCaptureOrPawnMove
               ,moveNumber
               );
}

#define HASHCOUNT (sizeof(GameKey)/sizeof(long))

unsigned long GameKey::hashCode() const {
  unsigned long result = 0;
  for(unsigned long *l = (unsigned long*)this, count=HASHCOUNT; count--;) {
    result = result * 13 + *(l++);
  }
  return result;
}

bool operator==(const GameKey &key1, const GameKey &key2) {
  return memcmp(&key1, &key2, sizeof(GameKey)) == 0;
}

bool operator!=(const GameKey &key1, const GameKey &key2) {
  return memcmp(&key1, &key2, sizeof(GameKey)) != 0;
}

void GameKey::validateSize() {
  const int sz = sizeof(GameKey);
  if((sizeof(GameKey) % sizeof(long)) != 0) {
    throwException(_T("sizeof(GameKey) is not n * sizeof(long). sizeof(GameKey) = %d"), sizeof(GameKey));
  }
}

#define TRANSFORM_EPSQUARE(dst,tr) dst.d.m_EPSquare = (d.m_EPSquare < 0) ? -1 : tr(d.m_EPSquare)

GameKey GameKey::swapPlayers() const {
  GameKey result;
  result.clear(GETENEMY(getPlayerInTurn()));
  result.d.m_castleState[WHITEPLAYER] = d.m_castleState[BLACKPLAYER];
  result.d.m_castleState[BLACKPLAYER] = d.m_castleState[WHITEPLAYER];
  TRANSFORM_EPSQUARE(result, MIRRORROW);
  for(int pos = 0; pos < ARRAYSIZE(m_pieceKey); pos++) {
    PieceKey pk = m_pieceKey[pos];
    if(pk != EMPTYPIECEKEY) {
      pk = SWAPPLAYER(pk);
      result.m_pieceKey[MIRRORROW(pos)] = pk;
    }
  }
  return result;
}

#define _TRANSFORMALL(dst, trEP, tr)                     \
{ memset(dst.m_pieceKey, 0, sizeof(dst.m_pieceKey));     \
  dst.m_u = m_u;                                         \
  trEP(dst, tr);                                         \
  for(int pos = 0; pos < ARRAYSIZE(m_pieceKey); pos++) { \
    const PieceKey pk = m_pieceKey[pos];                 \
    if(pk != EMPTYPIECEKEY) {                            \
      dst.m_pieceKey[tr(pos)] = pk;                      \
    }                                                    \
  }                                                      \
}

#define NO_EPTRANSFORM(dst,tr)

#define TRANSFORMKEY(dst, tr)                            _TRANSFORMALL(dst, TRANSFORM_EPSQUARE, tr)
#define TRANSFORMFIELDS(dst, tr)                         _TRANSFORMALL(dst, NO_EPTRANSFORM    , tr)


GameKey GameKey::rotate180() const {
  GameKey result;
  TRANSFORMKEY(   result, ROTATE180);
  return result;
}

GameKey GameKey::rotateRight() const {
  GameKey result;
  TRANSFORMFIELDS(result, ROTATERIGHT);
  return result;
}

GameKey GameKey::rotateLeft() const {
  GameKey result;
  TRANSFORMFIELDS(result, ROTATELEFT);
  return result;
}

GameKey GameKey::mirrorRows() const {
  GameKey result;
  TRANSFORMKEY(   result, MIRRORROW);
  return result;
}

GameKey GameKey::mirrorColumns() const {
  GameKey result;
  TRANSFORMKEY(   result, MIRRORCOLUMN);
  return result;
}

GameKey GameKey::mirrorDiag1() const {
  GameKey result;
  TRANSFORMFIELDS(result, MIRRORDIAG1);
  return result;
}

GameKey GameKey::mirrorDiag2() const {
  GameKey result;
  TRANSFORMFIELDS(result, MIRRORDIAG2);
  return result;
}

GameKey GameKey::transform(SymmetricTransformation st) const {
  switch(st) {
  case 0                    : return *this;
  case TRANSFORM_SWAPPLAYERS: return swapPlayers();
  case TRANSFORM_ROTATE180  : return rotate180();
  case TRANSFORM_ROTATERIGHT: return rotateRight();
  case TRANSFORM_ROTATELEFT : return rotateLeft();
  case TRANSFORM_MIRRORROW  : return mirrorRows();
  case TRANSFORM_MIRRORCOL  : return mirrorColumns();
  case TRANSFORM_MIRRORDIAG1: return mirrorDiag1();
  case TRANSFORM_MIRRORDIAG2: return mirrorDiag2();
  default                   : throwException(_T("Unknown symmetric transformation:%d"), st);
                              return *this;
  }
}

int GameKey::transform(int pos, SymmetricTransformation st) { // static 
  switch(st) {
  case 0                    : return pos;
  case TRANSFORM_SWAPPLAYERS: return MIRRORROW(   pos);
  case TRANSFORM_ROTATE180  : return ROTATE180(   pos);
  case TRANSFORM_ROTATERIGHT: return ROTATERIGHT( pos);
  case TRANSFORM_ROTATELEFT : return ROTATELEFT(  pos);
  case TRANSFORM_MIRRORROW  : return MIRRORROW(   pos);
  case TRANSFORM_MIRRORCOL  : return MIRRORCOLUMN(pos);
  case TRANSFORM_MIRRORDIAG1: return MIRRORDIAG1( pos);
  case TRANSFORM_MIRRORDIAG2: return MIRRORDIAG2( pos);
  default                   : throwException(_T("Unknown symmetric transformation:%d"), st);
                              return pos;
  }
}

#define SHORTCASTLE_ALLOWED          0x01
#define LONGCASTLE_ALLOWED           0x02
#define SHORTCASTLE_DONE             0x04
#define LONGCASTLE_DONE              0x08

const char GameKey::castleScore[16] = { // static
  -20 //  0 no castling done or allowed
 , -5 //  1 only short castle_allowed
 , -8 //  2 only long  castle_allowed
 ,  0 //  3 both short and long castle allowed
 , 30 //  4 short castle done
 , 30 //  5 short castle done
 , 30 //  6 short castle done
 , 30 //  7 short castle done
 , 16 //  8 long  castle done
 , 16 //  9 long  castle done
 , 16 // 10 long  castle done
 , 16 // 11 long  castle done
 ,  0 // 12 impossible. both short and long castle done
 ,  0 // 13 impossible. both short and long castle done
 ,  0 // 14 impossible. both short and long castle done
 ,  0 // 15 impossible. both short and long castle done
};
