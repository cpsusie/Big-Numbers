#include "stdafx.h"

SearchStatistic::SearchStatistic() {
  m_nodeCount = 0;
}

void SearchStatistic::startSearch(bool verbose) {
  m_verbose         = verbose;
  m_nodeCount       = 0;
  m_searchStartTime = Timestamp();
}

double SearchStatistic::getTimeUsed() const {
  return diff(m_searchStartTime, Timestamp(), TMILLISECOND);
}

TCHAR *HashElement::getVariantStr(TCHAR *dst) const {
  TCHAR *cp = dst;
  if(m_depth > 2) {
    for(const HashElement *hp = this; (hp->m_depth > 2) && (hp->m_score > MINSCORE);) {
      *(cp++) = ' ';
      cp += hp->getMoveStr(cp);
      const HashElement *next = hp->m_next;
      if(next == NULL || (next->m_depth != hp->m_depth-1)) {
        break;
      }
      hp = next;
    }
  }
  *cp = 0;
  return dst;
}

String HashElement::getVariantString() const {
  TCHAR tmp[300];
  return getVariantStr(tmp);
}

static const TCHAR *promoteIndexToShortNameEnglish  = _T(" qnrb");

Move::Move(const String &str) {
  setNoMove();
  if(!ISLEGALFIELD(str.cstr()) || !ISLEGALFIELD(str.cstr()+2)) {
    throwException(_T("Illegal move:<%s>"), str.cstr()); // doesn't have move syntax 
  }

  d.m_from = DECODEFIELD(str.cstr());
  d.m_to   = DECODEFIELD(str.cstr()+2);
  if(str.length() > 4) {
    const TCHAR *cp = _tcschr(promoteIndexToShortNameEnglish, tolower(str[4]));
    if(cp == NULL) {
      throwException(_T("Illegal promotion:<%s>"), str.cstr());
    } else {
      d.m_promoteIndex = cp - promoteIndexToShortNameEnglish;
    }
  }
}

String FastMove::toString() const {
  TCHAR tmp[100];
  toStr(tmp, d.m_from, d.m_to, d.m_promoteIndex);
  return tmp;
}

int FastMove::toStr(TCHAR *dst) const {
  return toStr(dst, d.m_from, d.m_to, d.m_promoteIndex);
}

String FastMove::toString(char from, char to, BYTE promoteIndex) { // static
  TCHAR tmp[100];
  toStr(tmp, from, to, promoteIndex);
  return tmp;
}

int FastMove::toStr(TCHAR *dst, char from, char to, BYTE promoteIndex) { // static 
  TCHAR *cp = dst;
  getFieldName(cp, from); cp += 2;
  getFieldName(cp, to  ); cp += 2;

  if(promoteIndex) {
   *(cp++) = promoteIndexToShortNameEnglish[promoteIndex];
  }
  *cp = 0;
  return cp - dst;
}

TCHAR *getFieldName(TCHAR *dst, char pos) {
  dst[0] = GETCOLNAME(pos);
  dst[1] = GETROWNAME(pos);
  dst[2] = 0;
  return dst;
}

String getFieldName(char pos) {
  TCHAR tmp[10];
  return getFieldName(tmp, pos);
}

String Board::getGameResultString(GameResult result) { // static 
  switch(result) {
  case GR_NORESULT      : return _T("?");
  case GR_WHITECHECKMATE: return _T("0-1 (Black mates)");
  case GR_BLACKCHECKMATE: return _T("1-0 (White mates)");
  case GR_STALEMATE     : return _T("1/2-1/2 (Stalemate)");
  case GR_REPETITION    : return _T("1/2-1/2 (Draw by repetition)");
  case GR_FIFTYMOVES    : return _T("1/2-1/2 (Draw by fifty move rule)");
  default               : return format(_T("Unknown GameResult:%d"), result);
  }
}

#ifdef _DEBUG

String Board::toString(int computerSide, bool detailed) {   // static 
  return bc.toString(getHistorySize(), computerSide, detailed);
}

String Board::getHistoryString() { // static 
  if(historySize == 0) {
    return EMPTYSTRING;
  }
  int moveIndex = 0, moveNumber = bc.m_startMoveNumber;
  String result;
  if(boardHistory[0].m_side == BLACK) {
    result += format(_T("%3d.   -   , %s\n"), moveNumber++, moveHistory[moveIndex++].toString().cstr());
  }
  for(;moveIndex < historySize; moveIndex += 2) {
    result += format(_T("%3d. %s , %s\n")
                    ,moveNumber++
                    ,moveHistory[moveIndex].toString().cstr()
                    ,(moveIndex+1<historySize) ? moveHistory[moveIndex+1].toString().cstr() : EMPTYSTRING);
  }
  return result;
}

String Board::getBoardHistoryString(bool detailed) { // static 
  String result;
  for(int i = 0; i < historySize; i++) {
    result += boardHistory[i].toString(BLACK, detailed);
    result += format(_T("%s\n"), moveHistory[i].toString().cstr());
  }
  result +=  bc.toString(BLACK, detailed);
  return result;
}

const TCHAR *getSideStr(int side) {
  switch(side) {
  case EMPTY: return _T("empty");
  case WHITE: return _T("white");
  case BLACK: return _T("black");
  default   : throwInvalidArgumentException(__TFUNCTION__, _T("side=%d"), side);
              return _T("?");
  }
}

const TCHAR *getPieceName(char pieceType) {
  switch(pieceType) {
  case WPAWN  :
  case BPAWN  : return _T("pawn"  );
  case KNIGHT : return _T("knight");
  case KING   : return _T("king"  );
  case BISHOP : return _T("bishop");
  case ROOK   : return _T("rook"  );
  case QUEEN  : return _T("queen" );
  default     : throwInvalidArgumentException(__TFUNCTION__,_T("piecetype=%d"), pieceType);
                return _T("?");
  }
}

String getPieceName1(char piece) {
  return format(_T("%s %s"), getSideStr(piece&COLORBITS), getPieceName(piece&7));
}

String fieldValuesToString() {
  String result;
  for(int r = 0; r < 8; r++) {
    for(int c = 0; c < 8; c++) {
      result += format(_T("%3d "), getFieldValue(MAKEPOS(r,c)));
    }
    result += _T("\n");
  }
  return result;
}

String pawnPushBonusToString() {
  String result;
  for(int side = WHITE; side <= BLACK; side<<=1) {
    result += format(_T("pawn push bonus(%s):\n"), getSideStr(side));
    for(int r = 8; r--;) {
      for(int c = 0; c < 8; c++) {
        result += format(_T("%3d "), getPawnPushBonus(side, MAKEPOS(r,c)));
      }
      result += _T("\n");
    }
  }
  return result;
}

TCHAR *formatTime(TCHAR *dst, int ms, bool showMS) {
  if(showMS) {
    if(ms < 60 * 1000) {                                                  // less than 1 minute
      _stprintf(dst, _T("%2d.%03d"), ms/1000,ms%1000);                          // ss.SSS
      return dst;
    } else if(ms < 60*60*1000) {                                          // less than 1 hour
      _stprintf(dst, _T("%2d:%02d.%1d"), ms/60000,(ms/1000)%60,(ms%1000)/100);  // mm.ss.S
      return dst;
    }
  }
  // 
  const int sec = ms/1000;
  if(sec < 60*60) {
    _stprintf(dst, _T("%2d:%02d"), sec/60, sec%60);                             // mm:ss
  } else {
    _stprintf(dst, _T("%d:%02d:%02d"), sec/60/60, (sec/60)%60, sec%60);         // h:mm:ss
  }
  return dst;
}

String formatTime(int ms, bool showMS) {
  TCHAR tmp[100];
  return formatTime(tmp, ms, showMS);
}
#endif

static bool isManyLines(const String &s) {
  int lineCount = 0;
  for(const TCHAR *cp = s.cstr(); *cp;) {
    if(*(cp++) == '\n') {
      if(++lineCount > 1) {
        return true;
      }
    }
  }
  return false;
}

void reply(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);

#ifdef _DEBUG
  if(!isatty(stdout)) {
    const String msg = vformat(format, argptr);
    if(isManyLines(msg)) {
      _ftprintf(stderr, _T("reply:\n%s"), msg.cstr());
    } else {
      _ftprintf(stderr, _T("reply:%s"), msg.cstr());
    }
    fflush(stderr);
  }
#endif

  _vtprintf(format, argptr);
  va_end(argptr);
  fflush(stdout);
}

void replyMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  StringArray lines(Tokenizer(msg,_T("\n")));
  for(size_t i = 0; i < lines.size(); i++) {
    reply(_T("info string %s\n"), lines[i].cstr());
  }
}
