#include "stdafx.h"

BoardConfiguration::BoardConfiguration() {
  clear();
}

void BoardConfiguration::clear() {
  memset(m_field, 0, sizeof(m_field));
  m_side            = WHITE;
  m_EP              = S;
  m_fiftyMoves      = 0;
  m_startMoveNumber = 1;
  initOccupationMap();
}

static const char startPosition[8]   = { 
  ROOK
 ,KNIGHT|HASMOVED
 ,BISHOP|HASMOVED
 ,QUEEN |HASMOVED
 ,KING
 ,BISHOP|HASMOVED
 ,KNIGHT|HASMOVED
 ,ROOK
}; // initial piece setup

void BoardConfiguration::setupStartBoard() {
  clear();
  for(int col = 8; col--;) {
    m_field[MAKEPOS(7,col)] = (m_field[MAKEPOS(0,col)] = startPosition[col]|WHITE)^COLORBITS;
    m_field[MAKEPOS(1,col)] = WHITEPAWN;  // initial board setup
    m_field[MAKEPOS(6,col)] = BLACKPAWN;
  }
  m_hashCode.clear();
  m_R          = 0;
  m_score      = 0;
  initOccupationMap();
}

void BoardConfiguration::setupBoard(Tokenizer &tok) { // tok should contain a valid FEN-string
  clear();
  const String pieces = tok.next();
  Tokenizer tok1(pieces,_T("/"));
  for(int rank = 8; rank--;) {
    const String line = tok1.next();
    int file = 0;
    for(const TCHAR *cp = line.cstr(); *cp; cp++) {
      if(file > 7) {
        throwException(_T("Invalid fen"));
      }
      const int fld = MAKEPOS(rank,file);
      char piece;
      switch(*cp) {
      case 'K': piece = WHITEKING   | HASMOVED                   ; break; 
      case 'Q': piece = WHITEQUEEN  | HASMOVED                   ; break; 
      case 'R': piece = WHITEROOK   | HASMOVED                   ; break; 
      case 'B': piece = WHITEBISHOP | HASMOVED                   ; break; 
      case 'N': piece = WHITEKNIGHT | HASMOVED                   ; break; 
      case 'P': piece = WHITEPAWN   | ((rank==1) ? 0 : HASMOVED) ; break; 
      case 'k': piece = BLACKKING   | HASMOVED                   ; break;
      case 'q': piece = BLACKQUEEN  | HASMOVED                   ; break; 
      case 'r': piece = BLACKROOK   | HASMOVED                   ; break; 
      case 'b': piece = BLACKBISHOP | HASMOVED                   ; break; 
      case 'n': piece = BLACKKNIGHT | HASMOVED                   ; break;
      case 'p': piece = BLACKPAWN   | ((rank==6) ? 0 : HASMOVED) ; break;
      default:
        if(('1' <= *cp)  && (*cp <= '8')) {
          file += *cp - '0';
          continue;
        } else {
          return;
        }
      }
      m_field[fld] = piece; file++;
    }
  }
  if(!tok.hasNext()) return;
  const String s = tok.next();
  switch(s[0]) {
  case 'w': m_side = WHITE; break;
  case 'b': m_side = BLACK; break;
  default : return;
  }
  if(!tok.hasNext()) return;
  const String castling = tok.next();
  if(castling == _T("-")) {
    // do nothing
  } else {
    for(const TCHAR *cp = castling.cstr(); *cp; cp++) {
      switch(*cp) {
      case 'K':
        if(((m_field[E1]&~HASMOVED) == (WHITE|KING)) && ((m_field[H1]&~HASMOVED) == (ROOK|WHITE))) {
          m_field[E1] &= ~HASMOVED;
          m_field[H1] &= ~HASMOVED;
        }
        break;
      case 'Q':
        if(((m_field[E1]&~HASMOVED) == (WHITE|KING)) && ((m_field[A1]&~HASMOVED) == (ROOK|WHITE))) {
          m_field[E1] &= ~HASMOVED;
          m_field[A1] &= ~HASMOVED;
        }
        break;
      case 'k':
        if(((m_field[E8]&~HASMOVED) == (BLACK|KING)) && ((m_field[H8]&~HASMOVED) == (ROOK|BLACK))) {
          m_field[E8] &= ~HASMOVED;
          m_field[H8] &= ~HASMOVED;
        }
        break;
      case 'q':
        if(((m_field[E8]&~HASMOVED) == (BLACK|KING)) && ((m_field[A8]&~HASMOVED) == (ROOK|BLACK))) {
          m_field[E8] &= ~HASMOVED;
          m_field[A8] &= ~HASMOVED;
        }
        break;
      default:
        return;
      }
    }
  }
  if(!tok.hasNext()) return;
  const String epStr  = tok.next();
  if(epStr == _T("-")) {
    m_EP = S;
  } else if(ISLEGALFIELD(epStr)) {
    m_EP = DECODEFIELD(epStr);
  }
  if(!tok.hasNext()) {
    // do nothing
  } else {
    m_fiftyMoves = tok.getInt();
  }
  if(!tok.hasNext()) {
    // do nothing
  } else {
    m_startMoveNumber = tok.getInt();
  }
  m_score      = 0;
  m_R          = 40;
  for(int r = 0; r < 8; r++) {
    for(int f = 0; f < 8; f++) {
      const char pos = MAKEPOS(r, f);
      const char sq  = m_field[pos];
      if(sq) {
        char type  = sq&7;
        int value  = max(getPieceValue(type),0); // white is positive, black negative
        m_R -= value >> 7;
        if(SEARCHCENTER(type)) {
          value += getFieldValue(pos);
        }
        if(ISPAWN(type)) {
          value += getPawnPushBonus(sq&COLORBITS, pos);
        }
        if(sq & BLACK) {
          value *= -1;
        }
        m_score += value;
      }
    }
  }
  initOccupationMap();
}

void BoardConfiguration::initOccupationMap() {
  m_occupationMap.clear();

#ifdef USE_KNIGHTBONUS
  m_player[0].clear(WHITE);
  m_player[1].clear(BLACK);
#endif

  for(int row = 8; row--;) {
    for(int col = 8; col--;) {
      const int  pos = MAKEPOS(row,col);
      const char sq  = m_field[pos];
      if(sq) {
        const int pl = (sq>>4)&1;
        m_occupationMap.m_playerMap[pl].add(pos);
#ifdef USE_KNIGHTBONUS
        switch(sq&7) {
        case KING:
          m_player[pl^1].addKing(pos);
          break;
        case KNIGHT:
          m_player[pl].addKnight(pos);
          break;
        }
#endif
      }
    }
  }
#ifdef USE_KNIGHTBONUS
  ajourKnightBonusTable();
#endif
}

#ifdef _DEBUG
String BoardConfiguration::toString(int historySize, int computerSide, bool detailed) const {
  static const char *pl       = ".?pnkbrq?P?NKBRQ";
  static const char *line     = "  +-----------------+\n";
  static const TCHAR *letters  = _T("a b c d e f g h");
  String result;
  String columnLetters;
  char start, delta;

  result += format(_T("Score:%d R:%d Hashcode:%s\nFEN:%s\n")
                  ,m_score,m_R,m_hashCode.toString().cstr()
                  ,getFENString(historySize/2+1).cstr()
                  );
  if(detailed) {
#ifdef USE_KNIGHTBONUS
    result += format("White conf:%s\nBlack conf:%s\n"
                    ,m_player[0].toString().cstr()
                    ,m_player[1].toString().cstr()
                    );
#endif
    result += m_occupationMap.toString();
  }

  if(computerSide == BLACK) {
    columnLetters = format(_T("    %s\n"), letters);
    start =  7;
    delta = -1;
  } else {
    columnLetters = format(_T("    %s\n"), rev(letters).cstr());
    start =  0;
    delta =  1;
  }
  result += columnLetters;
  result += line;
  for(int r = start; !(r&MASK88); r += delta) {
    result += format(_T("%d |"), r+1);
    for(int c = 7-start; !(c&MASK88); c -= delta) {
      result += format(_T(" %c"), pl[m_field[MAKEPOS(r,c)]&0xf]);
    }
    result += format(_T(" | %d\n"), r+1);
  }
  result += line;
  result += columnLetters;
  return result;
}

String BoardConfiguration::getFENString(int historySize) const {
  TCHAR boardStr[300], castleStr[20], EpStr[10];
  return format(_T("%s %c %s %s %d %d")
               ,getFENBoardString(boardStr)
               ,(m_side == WHITE) ? 'w' : 'b'
               ,getFENCastleString(castleStr)
               ,getFENEpString(EpStr)
               ,m_fiftyMoves
               ,m_startMoveNumber + historySize/2
               );
}

TCHAR *BoardConfiguration::getFENBoardString(TCHAR *dst) const {
#define FLUSHEMPTY() if(emptyCount) { *(cp++) = '0' + emptyCount; emptyCount = 0; }
  TCHAR *cp = dst;
  for(int rank = 7; rank >= 0; rank--) {
    if(rank < 7) {
      *(cp++) = '/';
    }
    int emptyCount = 0;
    for(int file = 0; file < 8; file++) {
      char ch;
      int fld = MAKEPOS(rank, file);
      unsigned char p = m_field[fld] & 0x1f;
      switch(p) {
      case 0           : emptyCount++; continue;
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

TCHAR *BoardConfiguration::getFENEpString(TCHAR *dst) const {
  return (m_EP==S) ? _tcscpy(dst, _T("-")) : getFieldName(dst, m_EP);
}

TCHAR *BoardConfiguration::getFENCastleString(TCHAR *dst) const {
  static const char wbKRRPos[][3] = {
    { E1, H1, A1 }
   ,{ E8, H8, A8 }
  };
  TCHAR *cp = dst;
  for(int player = WHITE; player <= BLACK; player <<= 1) { 
    const TCHAR *kqStr  = (player == WHITE) ? _T("KQ") : _T("kq");
    const char  *krrPos = (player == WHITE) ? wbKRRPos[0] : wbKRRPos[1];
    if(m_field[krrPos[0]] == (KING|player)) {
      if(m_field[krrPos[1]] == (ROOK|player)) {
        *(cp++) = kqStr[0];
      }
      if(m_field[krrPos[2]] == (ROOK|player)) {
        *(cp++) = kqStr[1];
      }
    }
  }
  if(cp == dst) {
    *(cp++) = '-';
  }
  *cp = 0;
  return dst;
}

void BoardConfiguration::validate() const {
  int count[2];
  count[0] = count[1] = 0;
  for(int row = 8; row--;) {
    for(int col = 8; col--;) {
      const int pos = MAKEPOS(row,col);
      const char sq = m_field[pos];
      if(sq) {
        char p = (sq & COLORBITS) >> 4;
        count[p]++;
        if(!m_occupationMap.m_playerMap[p].contains(POSTOINDEX(pos))) {
          throwException(_T("validate failed. %s map[%s] empty:\n%s"), getSideStr(sq&COLORBITS), getFieldName(pos).cstr(), toString().cstr());
        }
        if(m_occupationMap.m_playerMap[p^1].contains(POSTOINDEX(pos))) {
          throwException(_T("validate failed. %s map[%s] not empty:\n%s"), getSideStr((sq&COLORBITS)^1), getFieldName(pos).cstr(), toString().cstr());
        }
      } else {
        if(m_occupationMap.m_playerMap[0].contains(POSTOINDEX(pos))) {
          throwException(_T("validate failed. White map[%s] not empty:\n%s"), getFieldName(pos).cstr(), toString().cstr());
        }
        if(m_occupationMap.m_playerMap[1].contains(POSTOINDEX(pos))) {
          throwException(_T("validate failed. Black map[%s] not empty:\n%s"), getFieldName(pos).cstr(), toString().cstr());
        }
      }
    }
  }

  bool ok = true;
  for(int startIndex = 0; startIndex < 64; startIndex++) {
    char startPos = INDEXTOPOS(startIndex);
    for(int i = 0, side=WHITE; i < 2; i++, side=BLACK) {
      int itCount = 0;
      for(PieceIterator it(m_occupationMap.m_playerMap[i], startPos); it.hasNext() && itCount < 32; itCount++) {
        const int posIndex = it.next();
        const int pos = INDEXTOPOS(posIndex);

        if(itCount == 0) {
          char startsq = m_field[startPos];
          if((startsq&side) && (pos != startPos)) {
            _ftprintf(stderr,_T("validate failed. side=%s, startPos:%s. First value returned by iterator (=%s) != startPos, Board:\n%s")
                            ,getSideStr(side)
                            ,getFieldName(startPos).cstr()
                            ,getFieldName(pos).cstr()
                            ,toString().cstr());
            ok = false;
          }
        }
        const char sq = m_field[pos];
        if(!(sq & side)) {
          const String posName = getFieldName(pos);
          if(sq) {
            _ftprintf(stderr,_T("validate failed. startPos:%s, There is a %s on %s\n%s")
                            ,getFieldName(startPos).cstr()
                            ,getPieceName1(sq).cstr()
                            ,posName.cstr()
                            ,toString().cstr());
          } else {
            _ftprintf(stderr,_T("Validate failed. startPos:%s. No piece at %s\n%s\n")
                            ,getFieldName(startPos).cstr()
                            ,posName.cstr()
                            ,toString().cstr());
          }
          ok = false;
        }
      }
      if(itCount != count[i]) {
        _ftprintf(stderr,_T("validate failed. startPos:%s. itCount[%d] (=%d) != count[%d] (=%d).\n%s")
                        ,getFieldName(startPos).cstr()
                        ,i, itCount
                        ,i, count[i]
                        ,toString().cstr());
        ok = false;
      }
    }
  }

  if(ok) {
    if(isatty(stderr)) {
      _ftprintf(stderr, _T("all ok\n"));
    }
  } else if(!isatty(stderr)) {
    throwException(_T("validate failed"));
  }
}

#endif

#ifdef USE_KNIGHTBONUS
char PlayerConfig::knightBonus[7][7];

void PlayerConfig::clear(char side) {
  m_side         = side;
  m_enemyKingPos = S;
  m_knightCount  = 0;
  memset(m_knightPos  , S, sizeof(m_knightPos  ));
  memset(m_knightIndex, S, sizeof(m_knightIndex));
}

void PlayerConfig::removeKnight(unsigned char pos) {
  assert(m_knightCount > 0);
  const unsigned char index = m_knightIndex[pos];
  assert(index < m_knightCount);
  m_knightCount--;
  for(int i = index; i < m_knightCount; i++) {
    m_knightIndex[m_knightPos[i] = m_knightPos[i+1]]--;
  }
  m_knightPos[i]     = S;
  m_knightIndex[pos] = S;
}

void PlayerConfig::ajourKnightBonusTable(int R) { // static
  static const char knightBonus_R_Between6_30[7][7] = {
    0,2,4,1,4,0,0
   ,2,3,5,3,1,0,0
   ,4,5,2,3,4,0,0
   ,1,2,1,2,0,0,0
   ,2,0,1,0,0,0,0
   ,0,0,0,0,0,0,0
   ,0,0,0,0,0,0,0
  };
  if(R >= 6 && R <= 30) {
    memcpy(knightBonus, knightBonus_R_Between6_30 , sizeof(knightBonus));
  } else {
    memset(knightBonus, 0, sizeof(knightBonus));
  }
}

String PlayerConfig::toString() const {
  String result = format(_T("Opponent king:%s. Knights:%d["), (m_enemyKingPos==S)?_T("-"):getFieldName(m_enemyKingPos).cstr(), m_knightCount);
  for(int i = 0; i < m_knightCount; i++) {
    if(i) result += " ";
    result += format(_T("%s(+%d)"), getFieldName(m_knightPos[i]).cstr(), KNIGHTBONUS(m_knightPos[i], m_enemyKingPos));
  }
  result += _T("]");
  return result;
}

void PlayerConfig::checkInvariant(const TCHAR *fileName, int line) const {
  if(m_knightCount > ARRAYSIZE(m_knightPos)) {
    throwException(_T("Invariant(%s(%d):m_knightCount=%d. Must be <= %d"), fileName, line, m_knightCount, ARRAYSIZE(m_knightPos));
  }
  int count = 0;
  for(int i = 0; i < ARRAYSIZE(m_knightIndex); i++) {
    if(m_knightIndex[i] != S) {
      count++;
    }
  }
  if(count != m_knightCount) {
    throwException(_T("Invariant(%s(%d):(#indices != S) = %d, should be m_knightCount=%d"), fileName, line, count, m_knightCount);
  }
  for(i = m_knightCount; i < ARRAYSIZE(m_knightPos); i++) {
    if(m_knightPos[i] != S) {
      throwException(_T("Invariant(%s(%d):knightPos[%d] = %d. should be %d. knightCount=%d"), fileName, line, i, m_knightPos[i], S, m_knightCount);
    }
  }
  for(i = 0; i < m_knightCount; i++) {
    const unsigned int pos = m_knightPos[i];
    if(pos >= ARRAYSIZE(m_knightIndex)) {
      throwException(_T("Invariant(%s(%d):knightPos[%d]=%d. Must be < %d"), fileName, line, i, pos, ARRAYSIZE(m_knightIndex));
    }
    const unsigned int index = m_knightIndex[pos];
    if(index != i) {
      throwException(_T("Invariant(%s(%d):knightIndex[%d]=%d. Should be %d"), fileName, line, pos, index, i);
    }
  }
}

#endif
