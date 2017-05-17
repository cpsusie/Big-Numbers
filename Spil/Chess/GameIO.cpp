#include "stdafx.h"

const TCHAR *getPlayerShortNameEnglish(Player player) {
  switch(player) {
  case WHITEPLAYER: return _T("W");
  case BLACKPLAYER: return _T("B");
  default         : INVALIDPLAYERERROR(player);
                    return _T("?");
  }
}

const TCHAR *getPieceTypeShortNameEnglish(PieceType type) {
  switch(type) {
  case King  : return _T("K");
  case Queen : return _T("Q");
  case Rook  : return _T("R");
  case Bishop: return _T("B");
  case Knight: return _T("N");
  case Pawn  : return EMPTYSTRING;
  default    : throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
               return _T("?");
  }
}

const TCHAR *getPieceTypeNameEnglish(PieceType type, bool plur) {
  DEFINEMETHODNAME;
  if(plur) {
    switch(type) {
    case King  : return _T("kings");
    case Queen : return _T("queens");
    case Rook  : return _T("rooks");
    case Bishop: return _T("bishops");
    case Knight: return _T("knights");
    case Pawn  : return _T("pawns");
    default    : throwInvalidArgumentException(method, _T("type=%d"), type);
                 return _T("?");
    }
  } else {
    switch(type) {
    case King  : return _T("king");
    case Queen : return _T("queen");
    case Rook  : return _T("rook");
    case Bishop: return _T("bishop");
    case Knight: return _T("knight");
    case Pawn  : return _T("pawn");
    default    : throwInvalidArgumentException(method, _T("type=%d"), type);
                 return _T("?");
    }
  }
}

static PieceType decodeShortcut(const String &s) {
  DEFINEMETHODNAME;
  String t = trim(s);
  if(t.length() < 2) {
    throwInvalidArgumentException(method, _T("s=\"%s\""), s.cstr());
  }
  if(t.length() == 2 && t[0] >= 'a' && t[0] <= 'h') {
    return Pawn;
  } else {
    switch(t[0]) {
    case 'K': return King;
    case 'Q': return Queen;
    case 'R': return Rook;
    case 'B': return Bishop;
    case 'N': return Knight;
    default : throwInvalidArgumentException(method, _T("s=\"%s\""), s.cstr());
              return Pawn;
    }
  }
}

static PieceType decodePromoteChar(TCHAR ch) {
  DEFINEMETHODNAME;
  switch(ch) {
  case _T('Q'): return Queen;
  case _T('R'): return Rook;
  case _T('B'): return Bishop;
  case _T('N'): return Knight;
  default : throwInvalidArgumentException(method, _T("ch='%c'"), ch);
            return Pawn;
  }
}

static bool isLegalPromoteChar(TCHAR ch) {
  return ch != 0 && _tcschr(_T("QRBN"),ch) != NULL;
}

typedef struct {
  const TCHAR   *m_str;
  MoveAnnotation m_annotation;
} AnnotationString;

static const AnnotationString annotationStrings[] = {
  EMPTYSTRING  , NOANNOTATION
 ,_T("?") , BAD_MOVE
 ,_T("??"), VERYBAD_MOVE
 ,_T("!") , GOOD_MOVE
 ,_T("!!"), EXCELLENT_MOVE
 ,_T("!?"), INTERESTING_MOVE
 ,_T("?!"), DOUBIOUS_MOVE
};

MoveAnnotation parseAnnotation(const TCHAR *str) {
  if(str[0] == '\0' || isspace(str[0])) {
    return NOANNOTATION;
  }
  for(int i = 0; i < ARRAYSIZE(annotationStrings); i++) {
    const AnnotationString &as = annotationStrings[i];
    if(_tcscmp(str, as.m_str) == 0) {
      return as.m_annotation;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("str=\"%s\""), str);
  return NOANNOTATION;
}

int decodePosition(const TCHAR *str) {
  DEFINEMETHODNAME;
  int row,col;
  switch(str[0]) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
    col = str[0] - 'a';
    break;
  default:
    throwInvalidArgumentException(method, _T("str=\"%s\". Column must be ['a'..'h']"), str);
  }
  switch(str[1]) {
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
    row = str[1] - '1';
    break;
  default:
    throwInvalidArgumentException(method, _T("str=\"%s\". Row must be ['1'..'8']"), str);
  }
  return MAKE_POSITION(row,col);
}

int decodePosition(const String &s) {
  DEFINEMETHODNAME;
  String t = trim(s);
  if(t.length() < 2 || t.length() > 3) {
    throwInvalidArgumentException(method, _T("s=\"%s\". Length must be 2 or 3"), s.cstr());
  }
  if(t.length() == 3) {
    t = substr(t,1,2);
  }
  return decodePosition(t.cstr());
}

const TCHAR *getPlayerNameEnglish(Player player) {
  switch(player) {
  case WHITEPLAYER: return _T("white");
  case BLACKPLAYER: return _T("black");
  default         : INVALIDPLAYERERROR(player);
                    return _T("?");
  }
}

Game &Game::save(FILE *f) {
  _ftprintf(f,_T("%s"), toString().cstr());
  return *this;
}

Game &Game::load(FILE *f) {
  beginSetup();
  String s = readTextFile(f);
  Tokenizer tok(s, _T(" "));
  parse(s);
  return endSetup();
}

String Game::toString() const {
  String result = format(_T("%s"), getStartPosition().toString().cstr());
  return result + format(_T("History\n%s\nEndHistory\n"), getHistory().toString(MOVE_FILEFORMAT).replace('\r',EMPTYSTRING).cstr());
}

Game &Game::operator=(const String &src) {
  beginSetup();
  parse(src);
  return endSetup();
}


void Game::parse(const String &s) {
  clearBoard();
  for(Tokenizer t1(s,_T("\r\n")); t1.hasNext();) {
    String line = t1.next();
    Tokenizer t2(line,_T(":,; "));
    if(t2.hasNext()) {
      String p = t2.next();
      if(p.equalsIgnoreCase(_T("white"))) {
        parsePiecePositions(WHITEPLAYER,t2);
      } else if(p.equalsIgnoreCase(_T("black"))) {
        parsePiecePositions(BLACKPLAYER,t2);
      } else if(p.equalsIgnoreCase(_T("playerinturn"))) {
        if(t2.hasNext()) {
          p = t2.next();
        } else {
          throwException(_T("Unexpected end of line. Expected white/black"));
        }
        if(p.equalsIgnoreCase(_T("white"))) {
          m_gameKey.d.m_playerInTurn = WHITEPLAYER;
        } else if(p.equalsIgnoreCase(_T("black"))) {
          m_gameKey.d.m_playerInTurn = BLACKPLAYER;
        } else {
          throwException(_T("Unexpected symbol:\"%s\". Expected white/black"), p.cstr());
        }
        resetGameHistory();
        validateBoard(false);
      } else if(p.equalsIgnoreCase(_T("history"))) {
        try {
          endSetup();
          parseHistory(t1);
          beginSetup();
        } catch(Exception e) {
          beginSetup();
          throw;
        }
      }
    }
  }
}

void Game::parsePiecePositions(Player player, Tokenizer &tok) {
  while(tok.hasNext()) {
    String s = tok.next();
    if(s.length() < 2) {
      throwException(_T("Unrecognized piece/position:\"%s\""), s.cstr());
    }
    setPieceAtPosition(decodeShortcut(s),player,decodePosition(s));
  }
}

void Game::parseHistory(Tokenizer &tok) {
  while(tok.hasNext()) {
    String line = tok.next();
    if(line.equalsIgnoreCase(_T("EndHistory"))) {
      break;
    }
    for(Tokenizer t(line,_T(" .,")); t.hasNext();) {
      String n = t.next();
      String wStr = t.hasNext() ? t.next() : _T("-");
      String bStr = t.hasNext() ? t.next() : _T("-");
      if(wStr != _T("-")) {
        parseMove(wStr);
      }
      if(bStr != _T("-")) {
        parseMove(bStr);
      }
    }
  }
}

void Game::parseMove(const String &str) {
  TCHAR fromStr[10],toStr[10],attr[10];
  if(_stscanf(str.cstr(),_T("%2s-%2s%s"),fromStr,toStr,attr) != 3) {
    attr[0] = '\0';
    if(_stscanf(str.cstr(),_T("%2s-%2s"),fromStr,toStr) != 2) {
      throwException(_T("Unrecognized move:<%s>"),str.cstr());
    }
  }
  int       from      = decodePosition(fromStr);
  int       to        = decodePosition(toStr);
  PieceType promoteTo = NoPiece;

  int chIndex = 0;
  if(isLegalPromoteChar(attr[chIndex])) {
    promoteTo = decodePromoteChar(attr[chIndex++]);
  }
  MoveAnnotation annotation = parseAnnotation(attr+chIndex);
  executeMove(generateMove(from, to, promoteTo, annotation));
}

#ifndef TABLEBASE_BUILDER

String Game::toUCIString() const {
  String result = _T("position");
  const GameKey &startKey = getStartPosition();
  result += (startKey == GameKey::getStartUpPosition()) ? _T(" startpos") : format(_T(" fen %s"), startKey.toFENString().cstr());
  const int plyCount = getPlyCount();
  if(plyCount > 0) {
    result += _T(" moves");
    for(int ply = 0; ply < plyCount; ply++) {
      result += _T(" ") + toLowerCase(getMove(ply).toString(MOVE_UCIFORMAT));
    }
  }
  return result;
}

String Game::toFENString() const {
  return m_gameKey.toFENString(getPlyCountWithoutCaptureOrPawnMove(), getPlyCount()/2+1);
}

Game &Game::fromFENString(const String &s) {
  beginSetup();
  try {
    parseFEN(Tokenizer(s, _T(" ")));
    return endSetup();
  } catch(...) {
    newGame();
    endSetup();
    throw;
  }
}

void Game::parseUCI(const String &line) {
  for(Tokenizer tok(line, _T(" ")); tok.hasNext();) {
    String command = tok.next();
    if(command == _T("fen")) {
      parseFEN(tok);
    } else if(command == _T("startpos")) {
      newGame();
    }
    if(tok.hasNext() && (tok.next() == _T("moves"))) {
      endSetup(&m_gameKey);
      while(tok.hasNext()) {
        executeMove(generateMove(tok.next(), MOVE_UCIFORMAT));
      }
      beginSetup();
    }
  }
}

void Game::parseFEN(Tokenizer &tok) { // tok should contain a valid FEN-string
  clearBoard();
  try {
    const String pieces = tok.next();
    Tokenizer tok1(pieces,_T("/"));
    for(int rank = 8; rank--;) {
      const String line = tok1.next();
      int file = 0;
      for(const TCHAR *cp = line.cstr(); *cp; cp++) {
        if(!isValidPosition(rank, file)) {
          throwException(_T("non-existing position:(%d,%d)"), rank, file);
        }
        const int pos = MAKE_POSITION(rank, file);
        PieceKey piece;
        switch(*cp) {
        case 'K': piece = WHITEKING;  ; break;
        case 'Q': piece = WHITEQUEEN  ; break;
        case 'R': piece = WHITEROOK   ; break;
        case 'B': piece = WHITEBISHOP ; break;
        case 'N': piece = WHITEKNIGHT ; break;
        case 'P': piece = WHITEPAWN   ; break;
        case 'k': piece = BLACKKING   ; break;
        case 'q': piece = BLACKQUEEN  ; break;
        case 'r': piece = BLACKROOK   ; break;
        case 'b': piece = BLACKBISHOP ; break;
        case 'n': piece = BLACKKNIGHT ; break;
        case 'p': piece = BLACKPAWN   ; break;
        default :
          if(('1' <= *cp)  && (*cp <= '8')) {
            file += *cp - '0';
            continue;
          } else {
            throwException(_T("unexpected char:%c"), *cp);
          }
        }
        setPieceAtPosition(piece, pos); file++;
      }
    }
    endSetup();
    beginSetup();
    if(!tok.hasNext()) throwException(_T("expected [wb]"));
    const String s = tok.next();
    switch(s[0]) {
    case 'w': setPlayerInTurn(WHITEPLAYER); break;
    case 'b': setPlayerInTurn(BLACKPLAYER); break;
    default : throwException(_T("expected [wb]"));
    }

    m_gameKey.clearCastleState();
    m_gameKey.clearEPSquare();

    if(!tok.hasNext()) throwException(_T("expected [-KQkq]"));
    const String castling = tok.next();
    if(castling == _T("-")) {
      // do nothing
    } else {
      for(const TCHAR *cp = castling.cstr(); *cp; cp++) {
        switch(*cp) {
        case 'K': m_gameKey.d.m_castleState[WHITEPLAYER] |= SHORTCASTLE_ALLOWED; break;
        case 'Q': m_gameKey.d.m_castleState[WHITEPLAYER] |= LONGCASTLE_ALLOWED;  break;
        case 'k': m_gameKey.d.m_castleState[BLACKPLAYER] |= SHORTCASTLE_ALLOWED; break;
        case 'q': m_gameKey.d.m_castleState[BLACKPLAYER] |= LONGCASTLE_ALLOWED;  break;
        default : throwException(_T("expected [KQkq]"));
        }
      }
    }
    if(!tok.hasNext()) throwException(_T("expected EP-position"));
    const String ep = tok.next();
    if(ep == _T("-")) {
      // do nothing
    } else {
      m_gameKey.d.m_EPSquare = decodePosition(ep) ^ 8;
    }
    int pc = tok.getInt();
    int n  = tok.getInt();
  } catch(Exception e) {
    throwException(_T("Invalid FEN:%s"), e.what());
  }
}

#endif
