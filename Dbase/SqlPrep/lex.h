#pragma once

typedef enum {
  SYM_BEGIN
 ,SYM_CHAR
 ,SYM_CLOSE
 ,SYM_COMMA
 ,SYM_COMMIT
 ,SYM_CONNECT
 ,SYM_CONTROL
 ,SYM_CREATE
 ,SYM_CURSOR
 ,SYM_DATE
 ,SYM_DECLARE
 ,SYM_DESCRIBE
 ,SYM_DELETE
 ,SYM_DESCRIPTOR
 ,SYM_DOUBLE
 ,SYM_DROP
 ,SYM_END
 ,SYM_EXECUTE
 ,SYM_EXTERN
 ,SYM_FETCH
 ,SYM_FLOAT
 ,SYM_FOR
 ,SYM_FROM
 ,SYM_IMMEDIATE
 ,SYM_INCLUDE
 ,SYM_INDICATOR
 ,SYM_INSERT
 ,SYM_INT
 ,SYM_INTO
 ,SYM_LONG
 ,SYM_OPEN
 ,SYM_PREPARE
 ,SYM_RESET
 ,SYM_ROLLBACK
 ,SYM_SQLCA
 ,SYM_SECTION
 ,SYM_SELECT
 ,SYM_SHORT
 ,SYM_SIGNED
 ,SYM_STATIC
 ,SYM_TIME
 ,SYM_TIMESTAMP
 ,SYM_TO
 ,SYM_UNSIGNED
 ,SYM_UPDATE
 ,SYM_USER
 ,SYM_USING
 ,SYM_WCHAR
 ,SYM_VARCHAR
 ,SYM_WORK

 ,SYM_NUMBER
 ,SYM_STRING
 ,SYM_NAME
 ,SYM_HOSTVAR
 ,SYM_LBRACK
 ,SYM_RBRACK
 ,SYM_LPAR
 ,SYM_RPAR
 ,SYM_SEMI
 ,SYM_EOF
 ,SYM_EXEC_SQL
 ,SYM_END_EXEC
 ,SYM_UNKNOWN
 ,SYM_COMMENT

} Symbol;


#define MAX_STRING_LEN 130
#define MAX_NAME_LEN   30

class Scanner {
private:
  UINT                 m_errorcount;
  UINT                 m_warningcount;
  bool                 m_allok;
  SqlApiSourcePosition m_yypos;
  TCHAR                m_line[256];
  TCHAR               *m_next;
  TCHAR               *m_yytext;
  String               m_inputfname;
  String               m_absolutfname;
protected:
  FILE                *m_input;

  void markLine();
  void readLine();
  void getStringLit();
  void getHostvar(); /* getting C-names */
  void getName();
  void getNumber();
  void getComment();
public:

  TCHAR *tokstring(int token);

  Symbol token;
  TCHAR  the_string[MAX_STRING_LEN+1];
  int    the_string_len;
  TCHAR  the_name[MAX_NAME_LEN+1];
  int    the_name_len;
  long   the_number;

  bool   m_lex_trace;
  bool   m_traceon;
  bool   m_traceinput;
  FILE  *m_listfile,*m_errorfile;

  Scanner( const String &fname );
  virtual ~Scanner();

  void skipSpace();
  virtual void advance();
  TCHAR  currentChar() const { return *m_next; }
  TCHAR *currentFileName()   { return m_inputfname.cstr(); }
  TCHAR *absolutFileName()   { return m_absolutfname.cstr(); }
  void   nextToken();
  TCHAR *yytext() const      { return m_yytext; }
  int    yyleng() const      { return (int)(m_next - m_yytext); }
//  void message( TCHAR *format, ... );
  inline SqlApiSourcePosition yypos() const { return m_yypos; }
  void warning( long sqlcode, TCHAR *format, ... );
  void error(   long sqlcode, TCHAR *format, ... );
  bool allok()        const { return m_allok;        }
  UINT errorCount()   const { return m_errorcount;   }
  UINT warningCount() const { return m_warningcount; }
};

class CollectScanner : public Scanner {
  TCHAR *collect_buffer;
  int    collect_buffer_size;
  int    collect_count;
  int    collect_head;
  int    collect_tail;
  int    collect_lastsym;
  int    collect_lastsym_inc_space;

  void collectChar(TCHAR ch);
  inline void incmod(int &p) { p = ( p + 1) % collect_buffer_size; }
public:
  CollectScanner(const String &fname, int size);
  virtual ~CollectScanner();
  void advance();
  void nextToken();
  int  getUntilLastSym(   TCHAR *dst, bool inc_space );
  void flushUntilLastSym( bool inc_space );
  void flushLastSym();
  void skipUntilLastSym(  bool inc_space );
  void skipCollected();
  void flushCollected();
};
