#include "stdafx.h"
#include <FileNameSplitter.h>
#include "GrammarScanner.h"

GrammarScanner::GrammarScanner(const String &fileName)
: m_fileName(fileName)
, m_absoluteFileName(FileNameSplitter(fileName).getAbsolutePath())
, m_tabSize(Options::getInstance().m_tabSize)
{
  m_ok               = true;
  m_debug            = false;
  m_input            = FOPEN(m_fileName, _T("r"));
  m_currentPos       = SourcePosition(0, 0);
  m_text             = m_lineBuffer;
  m_length           = 0;
  m_collecting       = false;
  nextLine();
}

GrammarScanner::~GrammarScanner() {
  fclose(m_input);
}

void GrammarScanner::nextLine() {
  if(_fgetts(m_lineBuffer, ARRAYSIZE(m_lineBuffer), m_input) == nullptr) {
    _tcscpy(m_lineBuffer, EMPTYSTRING);
  } else {
    m_currentPos.incrLineNumber();
  }
  m_next = m_lineBuffer;
}

void GrammarScanner::advance() {
  if(*m_next == '\n') {
    if(m_collecting) {
      collectChar();
    }
    nextLine();
  } else {
    if(m_collecting) {
      collectChar();
    }
    if(*m_next != '\t') {
      m_currentPos.incrColumn();
    } else {
      m_currentPos.incrColumn(m_tabSize - m_currentPos.getColumn() % m_tabSize);
    }
    m_next++;
  }
}

void GrammarScanner::parseString() {
  int i = 0;
  while(*m_next && *m_next != '"' && *m_next != '\n' && i < ARRAYSIZE(m_string)) {
    if(*m_next == '\\') {
      advance();
      switch(*m_next) {
        case '\0':
        case '\n': goto done;
        case 'n' : m_string[i++] = '\n'; advance(); continue;
        case 'r' : m_string[i++] = '\r'; advance(); continue;
        case 't' : m_string[i++] = '\t'; advance(); continue;
        case 'f' : m_string[i++] = '\f'; advance(); continue;
        case '\\': m_string[i++] = '\\'; advance(); continue;
        case '"' : m_string[i++] = '"';  advance(); continue;
        default  : advance(); continue; // ignore all other
      }
    }
    m_string[i++] = *m_next;
    advance();
  }

done:
  if(i == ARRAYSIZE(m_string)) {
    error(_T("String too long. max:%d"), MAXSTRINGLEN);
    while(*m_next && *m_next != '\n' && *m_next != '"') {
      advance();
    }
    i--;
  }
  m_string[i] = '\0';
  if(*m_next == '"') {
    advance();
  }
}

Token GrammarScanner::parseName() {
  int i = 0;
  TCHAR name[MAXNAMELEN + 1];

  while((isalnum(*m_next) || *m_next == '_') && i < ARRAYSIZE(name)) {
    name[i++] = *m_next;
    advance();
  }
  if(i == ARRAYSIZE(name) ) {
    error(_T("Name too long. max:%d"), MAXNAMELEN);
    while(isalnum(*m_next) || (*m_next == '_')) {
      advance();
    }
    i--;
  }
  name[i] = '\0';

  return nameOrKeyWord(name);
}

void GrammarScanner::parseNumber() {
  bool isHex = false;

  m_number = 0;

  if(*m_next == '0') {
    advance();
    if((*m_next == 'x') || (*m_next == 'X')) {
      isHex = true;
      advance();
      while(isxdigit(*m_next)) {
        m_number = 16 * m_number;
        if(isdigit(*m_next) ) {
          m_number += (*m_next - '0');
        } else if(islower(*m_next)) {
          m_number += ((UINT)*m_next - 'a' + 10);
        } else {
          m_number += ((UINT)*m_next - 'A' + 10);
        }
        advance();
      }
    }
  }
  for(;isdigit(*m_next); advance()) {
    m_number = 10 * m_number + (*m_next - '0');
  }
  if(isHex) {
    return;
  }
  if((*m_next == '.') && isdigit(m_next[1])) {
    advance();
    for(double f = 0.1; isdigit(*m_next); advance(), f *= 0.1) {
      m_number += f * (*m_next - '0');
    }
  }
  int exponent = 0;
  if((*m_next == 'e') || (*m_next == 'E')) {
    advance();
    int exponentSign = 1;
    if(*m_next == '-') {
      exponentSign = -1;
      advance();
    } else if(*m_next == '+') {
      advance();
    }
    for(exponent = 0; isdigit(*m_next); advance()) {
      exponent = 10 * exponent + (*m_next - '0');
    }
    exponent *= exponentSign;
  }

  if(exponent != 0) {
    m_number *= pow(10, (double)exponent);
  }
}


Token GrammarScanner::next() {
start: // restart here if we meet a comment /*...*/ of //
  while(isspace(*m_next)) {
    advance();
  }

  m_text      = m_next;
  m_sourcePos = m_currentPos;

  if(m_debug) {
    _tprintf(_T("%s\n"), m_lineBuffer);
    _tprintf(_T("%*.*s^\n"), (int)(m_next - m_lineBuffer), (int)(m_next - m_lineBuffer), _T(" "));
    pause();
  }

  Token tok;
  if(isalpha(*m_next) || *m_next == '_') {
    parseName();
    tok = NAME;
  } else if(isdigit(*m_next)) {
    parseNumber();
    tok = NUMBER;
  } else {
    switch(*m_next) {
    case '"':
      advance();
      parseString();
      tok = STRINGLIT;
      break;
    case '|':
      tok = BAR;   advance();
      break;
    case ':':
      tok = COLON; advance();
      break;
    case ';':
      tok = SEMI;  advance();
      break;
    case '{':
      tok = LCURL; advance();
      break;
    case '}':
      tok = RCURL; advance();
      break;

    case '?':
      tok = QUEST; advance();
      break;
    case '+':
      tok = PLUS;  advance();
      break;
    case '*':
      tok = STAR;  advance();
     break;

    case '$':
      advance();
      if(*m_next == '$') {
        tok = DOLLARDOLLAR;
        advance();
      } else {
        tok = DOLLAR;
      }
      break;
    case '%':
      advance();
      switch(*m_next) {
      case '%':
        tok = PERCENTPERCENT;
        advance();
        break;
      case '{':
        tok = PERCENTLCURL;
        advance();
        break;
      case '}':
        tok = PERCENTRCURL;
        advance();
        break;
      default:
        if(isalpha(*m_next)) {
          tok = parseName();
          if(tok == NAME) {
            tok = UNKNOWN;
          }
        } else {
          tok = UNKNOWN;
        }
        break;
      }

      break;
    case '/':
      advance();
      switch(*m_next) {
      case '/': // comment until endofline
        advance();
        while(*m_next && *m_next != '\n') {
          advance();
        }
        goto start;
      case '*':
        advance();
        while(*m_next) {
          while(*m_next && *m_next != '*') {
            advance();
          }
          if(*m_next == '*') {
            advance();
            if(*m_next == '/') {
              advance();
              goto start;
            }
          }
        }
        tok = EOI;
        break;

      default:
        tok = UNKNOWN;
        break;
      }
      break;
    case '\0':
      tok = EOI;
      break;
    default:
      tok = UNKNOWN;
      advance();
      break;
    }

  }
  m_length = (int)(m_next - m_text);
/*
    yycomment("lex:lineno:%2d. Token:%2d yytext:[%*.*s] yyleng:%d\n",
      Lineno, Token, yyleng, yyleng, yytext, yyleng );
*/
    return tok;
}

String GrammarScanner::getText() {
  const TCHAR save = m_text[m_length];
  m_text[m_length] = '\0';
  String result = m_text;
  m_text[m_length] = save;
  return result;
}

void GrammarScanner::collectBegin() {
  m_collecting = true;
  m_collector.begin(m_text, m_length, SourcePositionWithName(m_absoluteFileName, m_sourcePos));
}

void GrammarScanner::collectChar() {
  m_collector.addChar(*m_next);
}

void GrammarScanner::getCollected(SourceText &sourceText) {
  sourceText = m_collector.getSourceText(m_length);
}

static void verrorwarning(const TCHAR *prefix, const SourcePositionWithName &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  _tprintf(_T("%s(%d,%d) : %s %s\n")
          ,pos.getName().cstr(), pos.getLineNumber(), pos.getColumn()+1
          ,prefix
          ,vformat(format, argptr).replace('\n', ' ').cstr());
}

static void verror(const SourcePositionWithName &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  verrorwarning(_T("error"), pos, format, argptr);
}

static void vwarning(const SourcePositionWithName &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  verrorwarning(_T("warning"), pos, format, argptr);
}

void GrammarScanner::error(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  ::verror(SourcePositionWithName(m_absoluteFileName, m_sourcePos), format, argptr);
  va_end(argptr);
  m_ok = false;
}

void GrammarScanner::error(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  ::verror(SourcePositionWithName(m_absoluteFileName, pos), format, argptr);
  va_end(argptr);
  m_ok = false;
}

void GrammarScanner::warning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  ::vwarning(SourcePositionWithName(m_absoluteFileName, m_sourcePos), format, argptr);
  va_end(argptr);
}

void GrammarScanner::warning(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  ::vwarning(SourcePositionWithName(m_absoluteFileName, pos), format, argptr);
  va_end(argptr);
}
