#include "stdafx.h"
#include <HashMap.h>
#include <Date.h>

/* #define DEBUG_LEX */

Scanner::Scanner(const String &fname) {
  m_inputfname     = fname;
  m_yypos.m_line   = 0;
  m_yypos.m_column = 0;
  m_errorcount     = 0;
  m_warningcount   = 0;
  m_allok          = true;
  m_lex_trace      = false;
  m_traceon        = false;
  m_traceinput     = false;
  m_errorfile      = stdout;
  m_listfile       = stdout;
  memset(m_line,0,sizeof(m_line));
  m_next           = m_line;

  m_input = fopen(m_inputfname,_T("r"));
  if(m_input == NULL) {
    error(SQL_FILE_OPEN_ERROR,_T("cannot open %s"),m_inputfname.cstr());
    return;
  }
  m_absolutfname = FileNameSplitter(m_inputfname).getAbsolutePath();
  m_absolutfname.replace(_T('\\'),_T('/'));
}

Scanner::~Scanner() {
  if(m_input != NULL)
    fclose(m_input);
}

void Scanner::readLine() {
  if(_fgetts(m_line, ARRAYSIZE(m_line), m_input) == NULL) {
    m_line[0] = '\0';
  } else {
    m_yypos.m_column = 0;
    m_yypos.m_line++;
  }
  m_next = m_line;
}

void Scanner::advance() {
  m_next++;
  m_yypos.m_column++;
  if(currentChar() == 0)
    readLine();
}

void Scanner::markLine() {
  _ftprintf(m_errorfile, _T("%s"), m_line );
  const int len = (int)(m_yytext - m_line);
  _ftprintf(m_errorfile, _T("%*.*s^\n"), len, len, _T(" "));
}

void Scanner::warning(long sqlcode, TCHAR *format, ...) {
  va_list argptr;

//  markLine();
  _ftprintf(  m_errorfile,_T("%s(%d,%d) : warning %ld:"),
    m_absolutfname.cstr(),
    m_yypos.m_line,
    m_yypos.m_column - yyleng(),
    sqlcode );
  va_start( argptr, format );
  _vftprintf( m_errorfile, format, argptr );
  _ftprintf(  m_errorfile, _T("\n") );
  va_end(argptr);
  m_warningcount++;
}

void Scanner::error(long sqlcode, TCHAR *format, ...) {
  va_list argptr;

//  markLine();
//  _ftprintf( errorfile, _T("Error in line %ld:"), m_yylineno );
  _ftprintf(m_errorfile,_T("%s(%d,%d) : error %d:")
                       ,m_absolutfname.cstr()
                       ,m_yypos.m_line
                       ,m_yypos.m_column - yyleng()
                       ,sqlcode );
  va_start( argptr, format );
  _vftprintf( m_errorfile, format, argptr );
  _ftprintf(  m_errorfile, _T("\n") );
  va_end(argptr);
  m_allok = false;
  m_errorcount++;
}

void Scanner::getStringLit() {
  int i = 0;

  advance();
  while(currentChar() && currentChar() != '"' && currentChar() != '\n' && i < ARRAYSIZE(the_string)) {
    if(currentChar() == '\\') {
      the_string[i++] = '\\';
      advance();
      if(i == ARRAYSIZE(the_string) || currentChar() == '\n') {
        break;
      }
    }
    the_string[i++] = currentChar();
    advance();
  }
  if(currentChar() == '\n') 
    error(SQL_SYNTAX_ERROR,_T("Missing String terminator"));

  if(i == ARRAYSIZE(the_string)) {
    error(SQL_STRING_TOO_LONG,_T("String must not exceed %d characters. truncated"), ARRAYSIZE(the_string) - 1);
    while(currentChar() && currentChar() != '\n' && currentChar() != '"' ) {
      advance();
    }
    i--;
  }
  the_string[i++] = '\0';
  the_string_len = i;
  if(currentChar() == '"' ) {
    advance();
  }

  token = SYM_STRING;
}

void Scanner::getHostvar() { /* getting C-names */
  int i = 0;

  if(currentChar() == '*') { /* It is a pointer */
    the_name[i++] = currentChar();
    advance();
  }

  while((isalnum(currentChar()) || currentChar() == '_' ) && i < sizeof(the_name)) {
    the_name[i++] = currentChar();
    advance();
  }
  if(i == sizeof(the_name)) {
    error(SQL_NAME_TOO_LONG, _T("Name must not exceed %d characters."), sizeof(the_name) - 1);
    while(isalnum(currentChar()) || currentChar() == '_' ) {
      advance();
    }
    i--;
  }
  the_name[i] = '\0';
  the_name_len = i;
  token = SYM_HOSTVAR;
}

typedef struct {
  TCHAR  *name;
  Symbol token;
} keyword;

static Symbol nameorkeyword(TCHAR *name);

void Scanner::getName() { 
  /* getting SQL-names with _ (EXEC SQL and END-EXEC is a problem!) */
  int i = 0;
  TCHAR uppername[MAX_NAME_LEN + 1];

  while((_istalnum(currentChar()) || currentChar() == '_' ) && i < ARRAYSIZE(the_name)) {
    the_name[i++] = currentChar();
    advance();
  }
  if(i == ARRAYSIZE(the_name)) {
    error(SQL_NAME_TOO_LONG, _T("Name must not exceed %d characters."), ARRAYSIZE(the_name) - 1);
    while(isalnum(currentChar()) || currentChar() == '_' ) {
      advance();
    }
    i--;
  }
  the_name[i] = '\0';
  the_name_len = i;

  _tcsupr(_tcscpy(uppername, the_name));
  token = nameorkeyword(uppername);
  switch(token) {
  case SYM_END: /* maybe END-EXEC */
    if(_tcsnicmp(m_next, _T("-EXEC"), 5) == 0) {
      for(i=0;i<5;i++) {
        advance();
      }
      token = SYM_END_EXEC;
    }
	break;
  case SYM_NAME:
    if(_tcsicmp( the_name, _T("EXEC")) == 0) { /* maybe EXEC SQL */
      const TCHAR *s;
      for(s = m_next; _istspace(*s); s++);
      if(_tcsnicmp(s, _T("SQL"), 3) == 0 && (_istspace(s[3]) || s[3] == 0)) {
        while(_istspace(currentChar())) {
          the_name[i++] = currentChar();
          advance();
        }
        while(_istalpha(currentChar())) {
          the_name[i++] = currentChar();
          advance();
        }
        the_name[i]  = '\0';
        the_name_len = i;
        token        = SYM_EXEC_SQL;
      }
    }

	break;
  default:;
  }
}

void Scanner::getNumber() {
  for(the_number = 0; isdigit( currentChar() ); advance()) {
    the_number = 10 * the_number + (currentChar() - '0');
    if(the_number < 0) 
      error(SQL_ARITHMETIC_OVERFLOW,_T("number too big"));
  }
  token = SYM_NUMBER;
}

void Scanner::getComment() {
  for(;;) {
    while(currentChar() != '*') {
      advance();
    }
    advance();
    if(currentChar() == '/') {
      advance(); 
      break;
    }
  }
}

void Scanner::skipSpace() {
  while(_istspace(currentChar())) {
    advance();
  }
}

void Scanner::nextToken() {
  skipSpace();

  m_yytext = m_next;

  if(_istalpha( currentChar())) {
    getName(); 
  } else if(isdigit(currentChar())) {
    getNumber(); 
  } else {
    switch( currentChar() ) {
    case ':':
      advance(); /* skip the : */
      getHostvar();
      break;

    case '"':
      getStringLit();
      break;

    case '/':
      advance();
      if( currentChar() == '*' ) {
        advance();
        getComment();
        token = SYM_COMMENT;
      }
      else if( currentChar() == '/' ) {
        while(currentChar() && currentChar() != '\n' ) advance();
        token = SYM_COMMENT;
      }
      else
        token = SYM_UNKNOWN;
      break;
    case '[':    token = SYM_LBRACK;        advance(); break;
    case ']':    token = SYM_RBRACK;        advance(); break;
    case '(':    token = SYM_LPAR;          advance(); break;
    case ')':    token = SYM_RPAR;          advance(); break;
    case ',':    token = SYM_COMMA;         advance(); break;
    case ';':    token = SYM_SEMI;          advance(); break;
    case '\0':   token = SYM_EOF;                      break;
    default  :   token = SYM_UNKNOWN;       advance(); break;
    }
  }
  if(m_lex_trace) {
    markLine();
    _tprintf(_T("%s\n"),tokstring(token));
  }
}

static keyword keywordlist[] = {
   _T("BEGIN")     , SYM_BEGIN     ,
   _T("CHAR")      , SYM_CHAR      ,
   _T("CLOSE")     , SYM_CLOSE     ,
   _T("COMMIT")    , SYM_COMMIT    ,
   _T("CONNECT")   , SYM_CONNECT   ,
   _T("CONTROL")   , SYM_CONTROL   ,
   _T("CREATE")    , SYM_CREATE    ,
   _T("CURSOR")    , SYM_CURSOR    ,
   _T("DATE")      , SYM_DATE      ,
   _T("DECLARE")   , SYM_DECLARE   ,
   _T("DESCRIBE")  , SYM_DESCRIBE  ,
   _T("DELETE")    , SYM_DELETE    ,
   _T("DESCRIPTOR"), SYM_DESCRIPTOR,
   _T("DOUBLE")    , SYM_DOUBLE    ,
   _T("DROP")      , SYM_DROP      ,
   _T("END")       , SYM_END       ,
   _T("EXECUTE")   , SYM_EXECUTE   ,
   _T("EXTERN")    , SYM_EXTERN    ,
   _T("FETCH")     , SYM_FETCH     ,
   _T("FLOAT")     , SYM_FLOAT     ,
   _T("FOR")       , SYM_FOR       ,
   _T("FROM")      , SYM_FROM      ,
   _T("IMMEDIATE") , SYM_IMMEDIATE ,
   _T("INCLUDE")   , SYM_INCLUDE   ,
   _T("INDICATOR") , SYM_INDICATOR ,
   _T("INSERT")    , SYM_INSERT    ,
   _T("INT")       , SYM_INT       ,
   _T("INTO")      , SYM_INTO      ,
   _T("LONG")      , SYM_LONG      ,
   _T("OPEN")      , SYM_OPEN      ,
   _T("PREPARE")   , SYM_PREPARE   ,
   _T("RESET")     , SYM_RESET     ,
   _T("ROLLBACK")  , SYM_ROLLBACK  ,
   _T("SQLCA")     , SYM_SQLCA     ,
   _T("SECTION")   , SYM_SECTION   ,
   _T("SELECT")    , SYM_SELECT    ,
   _T("SHORT")     , SYM_SHORT     ,
   _T("SIGNED")    , SYM_SIGNED    ,
   _T("STATIC")    , SYM_STATIC    ,
   _T("TIMET")     , SYM_TIME      ,
   _T("TIMESTAMP") , SYM_TIMESTAMP ,
   _T("TO")        , SYM_TO        ,
   _T("UNSIGNED")  , SYM_UNSIGNED  ,
   _T("UPDATE")    , SYM_UPDATE    ,
   _T("USER")      , SYM_USER      ,
   _T("USING")     , SYM_USING     ,
   _T("VARCHAR")   , SYM_VARCHAR   ,
   _T("WCHAR_T")   , SYM_WCHAR     ,
   _T("WORK")      , SYM_WORK      
};                           

typedef StrHashMap<Symbol> KeyWordMap;

class KeyWordTable : public KeyWordMap {
public:
  KeyWordTable(int size);
};

KeyWordTable::KeyWordTable(int size) : KeyWordMap(size) {
  for(int i = 0; i < ARRAYSIZE(keywordlist); i++) {
    put(keywordlist[i].name,keywordlist[i].token);
  }
}    

static KeyWordTable keywords(101);

static Symbol nameorkeyword(TCHAR *name) {
  Symbol *s = keywords.get(name);
  return s ? *s : SYM_NAME;
}

TCHAR *Scanner::tokstring(int token) {
  for(int i = 0; i < ARRAYSIZE(keywordlist); i++) {
    if(keywordlist[i].token == token) {
      return keywordlist[i].name;
    }
  }
  switch(token) {
  case SYM_COMMA   : return _T(",");
  case SYM_NUMBER  : return _T("number");
  case SYM_STRING  : return _T("String");
  case SYM_NAME    : return _T("name");
  case SYM_HOSTVAR : return _T("hostvar");
  case SYM_LBRACK  : return _T("[");
  case SYM_RBRACK  : return _T("]");
  case SYM_LPAR    : return _T("(");
  case SYM_RPAR    : return _T(")");
  case SYM_SEMI    : return _T(";");
  case SYM_EOF     : return _T("eof");
  case SYM_EXEC_SQL: return _T("exec sql");
  case SYM_END_EXEC: return _T("end-exec");
  case SYM_UNKNOWN : return _T("unknown");
  case SYM_COMMENT : return _T("comment");
  default: return _T("??");
  }
};
