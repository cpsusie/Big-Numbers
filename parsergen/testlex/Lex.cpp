#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <String.h>
#include <ctype.h>
#include <HashMap.h>
#include <sqlutil.h>
#include <sqlapi.h>
#include "lex.h"

/* #define DEBUG_LEX */

scanner::scanner( char *fname ) {
  strcpy(m_inputfname,fname);

  m_errorcount   = 0;
  m_warningcount = 0;
  m_allok        = true;
  m_lex_trace    = false;
  m_traceon      = false;
  m_traceinput   = false;
  m_errorfile    = stdout;
  m_listfile     = stdout;
  m_yylineno     = 0;
  memset(m_line,0,sizeof(m_line));
  m_next = m_line;

  input = fopen(m_inputfname,"r");
  if(input == NULL) {
    error(SQL_FILE_OPEN_ERROR,"cannot open %s",m_inputfname);
    return;
  }
}

scanner::~scanner() {
  if(input != NULL)
    fclose(input);
}

void scanner::readline( void ) {
  if( fgets( m_line, sizeof(m_line), input ) == NULL )
    m_line[0] = '\0';
  else
    m_yylineno++;
  m_next = m_line;
}

void scanner::advance( void ) {
  m_next++;
  if( currentchar() == 0 ) readline();
}

void scanner::markline( void ) {
  fprintf(m_errorfile, "%s", m_line );
  fprintf(m_errorfile, "%*.*s^\n", m_yytext - m_line, m_yytext - m_line, " ");
}

void scanner::warning( long sqlcode, char *format, ... ) {
  va_list argptr;

//  markline();
  fprintf(  m_errorfile,"%s(%d) : warning %ld:", m_inputfname,m_yylineno,sqlcode );
  va_start( argptr, format );
  vfprintf( m_errorfile, format, argptr );
  fprintf(  m_errorfile, "\n" );
  va_end(argptr);
  m_warningcount++;
}

void scanner::error( long  sqlcode, char *format, ... ) {
  va_list argptr;

//  markline();
//  fprintf( errorfile, "Error in line %ld:", m_yylineno );
  fprintf(  m_errorfile,"%s(%d) : error %d:",m_inputfname,m_yylineno, sqlcode);
  va_start( argptr, format );
  vfprintf( m_errorfile, format, argptr );
  fprintf(  m_errorfile, "\n" );
  va_end(argptr);
  m_allok = false;
  m_errorcount++;
}

void scanner::getstringlit( void ) {
  int i = 0;

  advance();
  while( currentchar() && currentchar() != '"' && currentchar() != '\n' && i < sizeof(the_string) ) {
    the_string[i++] = currentchar();
    advance();
  }
  if( currentchar() == '\n' )
    error( SQL_SYNTAX_ERROR,"missing String terminator" );

  if( i == sizeof(the_string) ) {
    error( SQL_STRING_TOO_LONG,"String must not exceed %d characters. truncated", sizeof(the_string) - 1 );
    while( currentchar() && currentchar() != '\n' && currentchar() != '"' ) advance();
    i--;
  }
  the_string[i++] = '\0';
  the_string_len = i;
  if( currentchar() == '"' ) advance();

  token = SYM_STRING;
}

void scanner::gethostvar( void ) { /* getting C-names */
  int i = 0;

  if(currentchar() == '*') { /* It is a pointer */
    the_name[i++] = currentchar();
    advance();
  }

  while( ( isalnum( currentchar() ) || currentchar() == '_' ) && i < sizeof(the_name) ) {
    the_name[i++] = currentchar();
    advance();
  }
  if( i == sizeof(the_name) ) {
    error( SQL_NAME_TOO_LONG, "Name must not exceed %d characters.", sizeof(the_name) - 1 );
    while( isalnum(currentchar()) || currentchar() == '_' ) advance();
    i--;
  }
  the_name[i] = '\0';
  the_name_len = i;

  token = SYM_HOSTVAR;
}

typedef struct {
  char  *name;
  symbol token;
} keyword;

static symbol nameorkeyword( char *name );

void scanner::getname( void ) {
  /* getting SQL-names with _ (EXEC SQL and END-EXEC is a problem!) */
  int i = 0;
  char uppername[ MAX_NAME_LEN + 1 ];

  while( ( isalnum( currentchar() ) || currentchar() == '_' ) && i < sizeof(the_name) ) {
    the_name[i++] = currentchar();
    advance();
  }
  if( i == sizeof(the_name) ) {
    error( SQL_NAME_TOO_LONG, "Name must not exceed %d characters.", sizeof(the_name) - 1 );
    while( isalnum(currentchar()) || currentchar() == '_' ) advance();
    i--;
  }
  the_name[i] = '\0';
  the_name_len = i;

  strupr( strcpy( uppername, the_name ) );
  token = nameorkeyword( uppername );
  switch(token) {
  case SYM_END: /* maybe END-EXEC */
    if( strnicmp( m_next, "-EXEC", 5 ) == 0) {
      for(i=0;i<5;i++) advance();
      token = SYM_END_EXEC;
    }
	break;
  case SYM_NAME:
    if( stricmp( the_name, "EXEC" ) == 0) { /* maybe EXEC SQL */
      char *s;
      for( s = m_next; isspace(*s); s++);
      if( strnicmp( s, "SQL", 3 ) == 0 && (isspace( s[3] ) || s[3] == 0 ) ) {
        while( isspace(currentchar()) ) { the_name[i++] = currentchar(); advance(); }
        while( isalpha(currentchar()) ) { the_name[i++] = currentchar(); advance(); }
        the_name[i] = '\0';
        the_name_len = i;
        token = SYM_EXEC_SQL;
      }
    }
	break;
  default:;
  }
}

void scanner::getnumber( void ) {
  for( the_number = 0; isdigit( currentchar() ); advance() ) {
    the_number = 10 * the_number + (currentchar() - '0');
    if( the_number < 0 )
      error(SQL_ARITHMETIC_OVERFLOW,"number too big");
  }
  token = SYM_NUMBER;
}

void scanner::getcomment(void) {
  for(;;) {
    while(currentchar() != '*') advance();
    advance();
    if(currentchar() == '/') {
      advance();
      break;
    }
  }
}

void scanner::skipspace() {
  while( isspace(currentchar()) ) advance();
}

void scanner::nexttoken() {
  skipspace();

  m_yytext = m_next;

  if( isalpha( currentchar() ) )
    getname();
  else
  if( isdigit(currentchar()) )
    getnumber();
  else
    switch( currentchar() ) {
      case ':':
        advance(); /* skip the : */
        gethostvar();
        break;

      case '"':
        getstringlit();
        break;

      case '/':
        advance();
        if( currentchar() == '*' ) {
          advance();
          getcomment();
          token = SYM_COMMENT;
        }
        else if( currentchar() == '/' ) {
          while(currentchar() && currentchar() != '\n' ) advance();
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
  if( m_lex_trace ) {
    markline();
    printf("%s\n",tokstring(token));
  }

  m_yyleng = m_next - m_yytext;
}

static keyword keywordlist[] = {
   "BEGIN"     , SYM_BEGIN     ,
   "CHAR"      , SYM_CHAR      ,
   "CLOSE"     , SYM_CLOSE     ,
   "COMMIT"    , SYM_COMMIT    ,
   "CONNECT"   , SYM_CONNECT   ,
   "CONTROL"   , SYM_CONTROL   ,
   "CREATE"    , SYM_CREATE    ,
   "CURSOR"    , SYM_CURSOR    ,
   "DECLARE"   , SYM_DECLARE   ,
   "DESCRIBE"  , SYM_DESCRIBE  ,
   "DELETE"    , SYM_DELETE    ,
   "DESCRIPTOR", SYM_DESCRIPTOR,
   "DOUBLE"    , SYM_DOUBLE    ,
   "DROP"      , SYM_DROP      ,
   "END"       , SYM_END       ,
   "EXECUTE"   , SYM_EXECUTE   ,
   "EXTERN"    , SYM_EXTERN    ,
   "FETCH"     , SYM_FETCH     ,
   "FLOAT"     , SYM_FLOAT     ,
   "FOR"       , SYM_FOR       ,
   "FROM"      , SYM_FROM      ,
   "IMMEDIATE" , SYM_IMMEDIATE ,
   "INCLUDE"   , SYM_INCLUDE   ,
   "INDICATOR" , SYM_INDICATOR ,
   "INSERT"    , SYM_INSERT    ,
   "INT"       , SYM_INT       ,
   "INTO"      , SYM_INTO      ,
   "LONG"      , SYM_LONG      ,
   "OPEN"      , SYM_OPEN      ,
   "PREPARE"   , SYM_PREPARE   ,
   "RESET"     , SYM_RESET     ,
   "ROLLBACK"  , SYM_ROLLBACK  ,
   "SQLCA"     , SYM_SQLCA     ,
   "SECTION"   , SYM_SECTION   ,
   "SELECT"    , SYM_SELECT    ,
   "SHORT"     , SYM_SHORT     ,
   "SIGNED"    , SYM_SIGNED    ,
   "STATIC"    , SYM_STATIC    ,
   "TO"        , SYM_TO        ,
   "UNSIGNED"  , SYM_UNSIGNED  ,
   "UPDATE"    , SYM_UPDATE    ,
   "USER"      , SYM_USER      ,
   "USING"     , SYM_USING     ,
   "VARCHAR"   , SYM_VARCHAR   ,
   "WORK"      , SYM_WORK
};

typedef HashMap<const char*,symbol> keywordhash;

class keywordtable : public keywordhash {
public:
  keywordtable(int size);
};

keywordtable::keywordtable( int size ) : keywordhash(size,strHash,strHashCmp) {
  for(int i=0;i<ARRAYSIZE(keywordlist);i++)
    insert( keywordlist[i].name,keywordlist[i].token );
}

static keywordtable keywords(101);

static symbol nameorkeyword( char *name ) {
  symbol *s = keywords.find(name);
  return s ? *s : SYM_NAME;
}

char *scanner::tokstring(int token) {
  for(int i = 0; i < ARRAYSIZE(keywordlist); i++)
    if(keywordlist[i].token == token)
      return keywordlist[i].name;
  switch(token) {
  case SYM_COMMA   : return ",";
  case SYM_NUMBER  : return "number";
  case SYM_STRING  : return "String";
  case SYM_NAME    : return "name";
  case SYM_HOSTVAR : return "hostvar";
  case SYM_LBRACK  : return "[";
  case SYM_RBRACK  : return "]";
  case SYM_LPAR    : return "(";
  case SYM_RPAR    : return ")";
  case SYM_SEMI    : return ";";
  case SYM_EOF     : return "eof";
  case SYM_EXEC_SQL: return "exec sql";
  case SYM_END_EXEC: return "end-exec";
  case SYM_UNKNOWN : return "unknown";
  case SYM_COMMENT : return "comment";
  default: return "??";
  }
};
