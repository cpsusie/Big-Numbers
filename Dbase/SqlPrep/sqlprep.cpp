#include "stdafx.h"
#include <ctype.h>
#include <time.h>

class HostVarIndex {
public:
  int m_data;
  int m_ind;
  inline bool hasIndicator() const { return m_ind >= 0; }
};

class HostVar {
public:
  TCHAR       m_name[MAX_NAME_LEN+1];
  DbFieldType m_type;
  int         m_len;
  void print();
  int type(HostVarIndex &v) const;
};

int HostVar::type(HostVarIndex &v) const {
  return v.hasIndicator() ? isNullAllowed(m_type) : m_type;
}

class Cursor {
public:
  TCHAR m_name[MAX_NAME_LEN+1];
  int   m_nr;
  CompactArray<HostVarIndex> m_hostvarList;
  Cursor() {
    m_name[0] = 0; m_nr = 0;
  }
};

class DynamicStatement {
public:
  TCHAR m_name[MAX_NAME_LEN+1];
  int  m_nr;
};

class StaticStatement {
public:
  int m_nr;
  CompactArray<HostVarIndex> m_inhostvarlist;
  CompactArray<HostVarIndex> m_outhostvarlist;
};

class Parser : public CollectScanner {
public:
  Array<HostVar>          m_hostvarList;
  Array<Cursor>           m_cursorList;
  Array<DynamicStatement> m_dynstatementList;
  Array<StaticStatement>  m_staticstatementlist;
  SqlApiBindProgramId     m_programid;

  int   m_statementCounter;
  SqlApiSourcePosition m_stmtpos;
  TCHAR m_stmtString[SQLAPI_MAXSTMTSIZE];
  int   m_stmtSize;
  bool  m_genLineMarks;
  int   m_endexeclineno;

  FILE *m_bndfile;
  bool  m_dumpStatements;
  bool  m_dumpbnd;
  Parser(const String &fname);
  ~Parser();
  void parseFile();
  void dumpSymbolTable();
private:
  int nextStatementCount();
  void hostvarIndexPrint(HostVarIndex &c);
  int findHostVarIndex(TCHAR *name);
  HostVar *findHostvar(TCHAR *name);
  void cursorPrint(    Cursor &c);
  Cursor *searchCursor(TCHAR *name);
  Cursor *findCursor(  TCHAR *name);
  void staticStatementPrint(StaticStatement &c);
  DynamicStatement *searchDynStatement(TCHAR *name);
  DynamicStatement *findDynStatement(  TCHAR *name);

  void emit(TCHAR *format,...);
  void emitLineMark(int line);
  void emitLineMark();
  void emitBegin();
  void emitEnd();
  void emitStartComment();
  void emitEndComment(int line);
  void emitErrorMark();
  void emitDeclarePgmid();
  void emitChkPgmid();
  void emitDeclareVarlist(int n);
  void emitBindVarlist(int n);
  void emitBindDA(TCHAR *daname);
  void emitSetStr(HostVar *var);
  void emitSetVar(int i, const HostVarIndex  &varindex);
  void emitSetVarCStr(int n, HostVar *var, const char    *str);
  void emitSetVarWStr(int n, HostVar *var, wchar_t       *str);
  void emitCall(int apicall,int apiopt, int ninput, int noutput);
  void emitBndStmt(int nr, CompactArray<HostVarIndex> *input, CompactArray<HostVarIndex> *output);
  void emitBndProgramId();
  void checkToken(Symbol expected);
  void parseName(HostVar *var);
  void parseNameDef();
  void parseEndExec();
  void parseDeclareSection();
  void parseIncludeSection();
  HostVar *parseStringHost();
  void parseIndicator(HostVarIndex &hostvarref);
  void parseHostvar(  HostVarIndex &hostvarref);
  void parseConnectto();
  void parseConnect();
  void parseSelect();
  void parseStaticStatement();
  void parseUpdate();
  void parseInsert();
  void parseDelete();
  void parseCursorSelect(CompactArray<HostVarIndex> &varlist);
  void parseDeclare();
  void parseOpen();
  void parseFetchInto(CompactArray<HostVarIndex> &varlist);
  void parseFetch();
  void parseClose();
  void parsePrepare();
  void parseDescribe();
  void parseImmediate();
  void parse_execute_dynamic();
  void parse_execute();
  void parseCommit();
  void parseRollback();
  void parseCreate();
  void parseDrop();
  void parseSQL();
  void genprogramid();
};

Parser::Parser(const String &fname) : CollectScanner(fname, SQLAPI_MAXSTMTSIZE ) {
  genprogramid();

  m_statementCounter = 0;
  m_stmtSize         = 0;
  _tcscpy(m_stmtString,EMPTYSTRING);
  m_endexeclineno    = 0;
  m_dumpStatements   = false;
  m_dumpbnd          = false;
  m_genLineMarks     = true;
  m_bndfile          = NULL;

  String bndFileName = FileNameSplitter(fname).setExtension(_T("bnd")).getFullPath();
  String cppFileName = FileNameSplitter(fname).setExtension(_T("cpp")).getFullPath();

  if(bndFileName.equalsIgnoreCase(fname)) {
    error(SQL_INVALID_FILENAME,_T("outputfile %s would overwrite inputfile\n"), bndFileName.cstr());
  }
  if(cppFileName.equalsIgnoreCase(fname)) {
    error(SQL_INVALID_FILENAME,_T("outputfile %s would overwrite inputfile\n"), cppFileName.cstr());
  }

  if(allok()) {
    try {
      m_listfile.open(cppFileName);
      m_listfile.setTrimRight(true);
    } catch(Exception e) {
      error(SQL_FILE_OPEN_ERROR,_T("%s:%s\n"),e.what(), cppFileName.cstr());
    }
    m_bndfile  = fopen(bndFileName, _T("wb"));
    if(m_bndfile == NULL) {
      error(SQL_FILE_OPEN_ERROR,_T("Cannot open %s\n"),bndFileName.cstr());
    }

    emitBndProgramId();

    advance();
    nextToken();
  }
}

Parser::~Parser() {
  m_listfile.close();

  if(m_bndfile != NULL) {
    Packer p;
    p.writeEos(ByteFile(m_bndfile, WRITEMODE));
    fclose(m_bndfile);
  }
}

int Parser::nextStatementCount() {
  m_statementCounter++;
  return m_statementCounter;
}

void Parser::hostvarIndexPrint(HostVarIndex &c) {
  _tprintf(_T("  HostVar:[%3d,%3d]=("),c.m_data,c.m_ind);
  HostVar &vard = m_hostvarList[c.m_data];
  _tprintf(_T("%-20s,"),vard.m_name);
  HostVar *vari = (c.m_ind < 0) ? NULL : &m_hostvarList[c.m_ind];
  _tprintf(_T("%-20s)\n"),vari?vari->m_name:_T("-"));
}

void HostVar::print() {
  _tprintf(_T("HostVar name:[%s] type:%d=%s len:%d\n")
          ,m_name,m_type,getTypeString(m_type),m_len);
}

int Parser::findHostVarIndex(TCHAR *name) {
  for(size_t inx = 0; inx < m_hostvarList.size(); inx++) {
    if(_tcscmp(m_hostvarList[inx].m_name,name) == 0) {
      return (int)inx;
    }
  }
  error(SQL_UNDECLARED_HOSTVAR,_T("Undeclared HostVar:%s"),name);
  return -1;
}

HostVar *Parser::findHostvar(TCHAR *name) {
  for(size_t i = 0; i < m_hostvarList.size(); i++) {
    if(_tcscmp(m_hostvarList[i].m_name,name) == 0) {
      return &m_hostvarList[i];
    }
  }
  return NULL;
}

/*  ------------------------------------------------------------------- */

void Parser::cursorPrint(Cursor &c) {
  _tprintf(_T("Cursor:[%s] nr:%d\n"), c.m_name,c.m_nr);
  for(size_t i = 0; i < c.m_hostvarList.size(); i++) {
    hostvarIndexPrint(c.m_hostvarList[i]);
  }
}

Cursor *Parser::searchCursor(TCHAR *name) {
  for(size_t i = 0; i < m_cursorList.size(); i++) {
    if(_tcsicmp(m_cursorList[i].m_name,name) == 0) {
      return &m_cursorList[i];
    }
  }
  return NULL;
}

Cursor *Parser::findCursor(TCHAR *name) {
  Cursor *cursor = searchCursor(name);
  if(cursor == NULL) {
    error(SQL_UNDECLARED_CURSOR,_T("Undeclared Cursor:%s"),name);
  }
  return cursor;
}

/*  ------------------------------------------------------------------- */

static void dynstatementprint(DynamicStatement &c) {
  _tprintf(_T("DynamicStatement:[%s] nr:%d\n"), c.m_name,c.m_nr);
}

void Parser::staticStatementPrint(StaticStatement &c) {
  _tprintf(_T("StaticStatement: nr:%d\n"), c.m_nr );
  _tprintf(_T("inputlist:\n"));
  for(size_t i = 0; i < c.m_inhostvarlist.size(); i++) {
    hostvarIndexPrint(c.m_inhostvarlist[i]);
  }
  _tprintf(_T("ouputlist:\n"));
  for(size_t i = 0; i < c.m_outhostvarlist.size(); i++) {
    hostvarIndexPrint(c.m_outhostvarlist[i]);
  }
}

DynamicStatement *Parser::searchDynStatement(TCHAR *name) {
  for(size_t i = 0; i < m_dynstatementList.size(); i++) {
    if(_tcsicmp(m_dynstatementList[i].m_name,name) == 0) {
      return &m_dynstatementList[i];
    }
  }
  return NULL;
}

DynamicStatement *Parser::findDynStatement(TCHAR *name) {
  DynamicStatement *stmt = searchDynStatement(name);
  if(stmt == NULL) {
    error(SQL_UNDECLARED_STATEMENT,_T("Undeclared statement:%s"),name);
    return NULL;
  } else {
    return stmt;
  }
}

/*  ------------------------------------------------------------------- */

void Parser::emit(TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vfprintf(&m_listfile,format,argptr);
  va_end(argptr);
}

void Parser::emitLineMark(int line) {
  if(m_genLineMarks) {
    if(line==0) line++;
    emit(_T("#line %ld \"%s\"\n"),line,absolutFileName());
  }
}

void Parser::emitLineMark() {
  emitLineMark(yypos().m_line-1);
}


void Parser::emitBegin() {
  emit(_T("{\n"));
}

void Parser::emitEnd() {
  emit(_T("}\n"));
  emitLineMark(yypos().m_line - 2);
}

void Parser::emitStartComment() {
  emit(_T("\n#ifdef __NEVER__\n"));
//  emit(_T("\n/*\n"));
}

void Parser::emitEndComment(int line) {
  emit(_T("\n#endif\n"));
//  emit(_T("\n*/\n"));
  emitLineMark(line);
}

void Parser::emitErrorMark() {
  emit(_T("#error \"sqlprep found %d error(s) in %s\"\n"),errorCount(),currentFileName());
}

void Parser::emitDeclarePgmid() {
  const int nbytes = sizeof(m_programid) * sizeof(TCHAR);
  BYTE *p = (BYTE*)&m_programid;

  emit(_T("static char sqlapi_program_id[%d] = {\n"),nbytes);
  for(int i = 0; i < nbytes; i++, p++) {
    emit(_T("%3d%c"),*p,i<nbytes-1?',':'\n');
    if(i%20==19) emit(_T("\n"));
  }
  emit(_T("};\n"));
  emit(_T("/* %s %s */\n"), m_programid.m_fileName, m_programid.m_timestamp);
  emitLineMark();
}

void Parser::emitChkPgmid() {
/*
  emit(_T("  sqlapi_chkpgmid(sqlapi_program_id,&sqlca);\n"));
  emitLineMark();
*/
}

void Parser::emitDeclareVarlist(int n) {
  emit(_T("  SqlApiVarList sqlapi_varl[%d];\n"),n);
  emitLineMark();
}

void Parser::emitBindVarlist(int n) {
/*
  emit(_T("  sqlapi_bindvarlist(%d,sqlapi_varl);\n"),n);
  emitLineMark();
*/
}

void Parser::emitBindDA(TCHAR *daname) {
  emit(_T("  sqlapi_bindda(&%s);\n"),daname);
  emitLineMark();
}

void Parser::emitSetStr(HostVar *var) {
  emit(_T("  sqlapi_setstr(%s);\n"),var->m_name);
  emitLineMark();
}

void Parser::emitSetVar(int i, const HostVarIndex &varindex) {
  HostVar      &var = m_hostvarList[varindex.m_data];
  HostVar      *ind = (varindex.m_ind) < 0 ? NULL : &m_hostvarList[varindex.m_ind];

  emit(_T("  sqlapi_varl[%d].sqltype = %d;\n"), i, ind?isNullAllowed(var.m_type):var.m_type );
  switch(var.m_type) {
  case DBTYPE_VARCHAR:
    emit(_T("  sqlapi_varl[%d].sqllen  = %s.len();\n")  , i, var.m_name );
    emit(_T("  sqlapi_varl[%d].sqldata = %s.data();\n") , i, var.m_name );
    break;
  case DBTYPE_CSTRING:
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")        , i, var.m_len  );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)%s;\n") , i, var.m_name );
    break;
  default:
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")        , i, var.m_len  );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)&%s;\n"), i, var.m_name );
    break;
  }

  if(ind) {
    emit(_T("  sqlapi_varl[%d].sqlind = &%s;\n"), i, ind->m_name );
  } else {
    emit(_T("  sqlapi_varl[%d].sqlind = 0L;\n"), i );
  }
  emitLineMark();
}

void Parser::emitSetVarCStr(int n, HostVar *var, const char *str) {
  USES_CONVERSION;
  emit(_T("  sqlapi_varl[%d].sqltype = %d;\n"), n, DBTYPE_CSTRING );
  if(var) {
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")           , n, var->m_len    );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)%s;\n")    , n, var->m_name   );
  } else {
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")           , n, strlen(str)+1 );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)\"%s\";\n"), n, A2T(str)      );
  }
  emit(_T("  sqlapi_varl[%d].sqlind = 0L;\n"), n );
  emitLineMark();
}

void Parser::emitSetVarWStr(int n, HostVar *var, wchar_t *str) {
  USES_CONVERSION;
  emit(_T("  sqlapi_varl[%d].sqltype = %d;\n"), n, DBTYPE_WSTRING );
  if(var) {
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")           , n, var->m_len    );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)%s;\n")    , n, var->m_name   );
  } else {
    emit(_T("  sqlapi_varl[%d].sqllen  = %d;\n")           , n, wcslen(str)+1 );
    emit(_T("  sqlapi_varl[%d].sqldata = (void*)L\"%s\";\n"), n, W2T(str)      );
  }
  emit(_T("  sqlapi_varl[%d].sqlind = 0L;\n"), n );
  emitLineMark();
}


#ifndef UNICODE
#define emitSetVarTStr(n, var, str) emitSetVarCStr(n, var, str)
#else
#define emitSetVarTStr(n, var, str) emitSetVarWStr(n, var, str)
#endif

void Parser::emitCall(int apicall,int apiopt, int ninput, int noutput) {
  TCHAR *vars = (ninput > 0 || noutput > 0) ? _T("sqlapi_varl") : _T("NULL");

  emit(_T("  sqlapi_call(%d,%d,sqlapi_program_id,%d,%d,%s,sqlca);\n"),
    apicall,apiopt,ninput,noutput,vars);
/*  emit(_T("  sqlapi_stop();\n"));
*/
  emitLineMark();
}

void Parser::emitBndStmt(int nr, CompactArray<HostVarIndex> *input, CompactArray<HostVarIndex> *output) {
  SqlApiBindStmt bndstmt;

  _tcscpy(bndstmt.m_stmtHead.m_sourceFile,currentFileName());
  bndstmt.m_stmtHead.m_nr       = nr;
  bndstmt.m_stmtHead.m_pos      = m_stmtpos;
  bndstmt.m_stmtHead.m_ninput   = input  ? (UINT)input->size()  : 0;
  bndstmt.m_stmtHead.m_noutput  = output ? (UINT)output->size() : 0;
  bndstmt.m_stmtHead.m_stmtSize = m_stmtSize;

  if(bndstmt.m_stmtHead.m_ninput > SQLAPI_MAXHOSTVAR) {
    error(SQL_TOO_MANY_HOSTVAR,_T("Number of HostVar too big. Max:%d"), SQLAPI_MAXHOSTVAR);
  }

  if(bndstmt.m_stmtHead.m_noutput > SQLAPI_MAXHOSTVAR) {
    error(SQL_TOO_MANY_HOSTVAR,_T("Number of HostVar too big. Max:%d"), SQLAPI_MAXHOSTVAR);
  }

  if(bndstmt.m_stmtHead.m_stmtSize > SQLAPI_MAXSTMTSIZE) {
    error(SQL_STATEMENT_TOO_BIG,_T("Statement too big. Maxsize:%d"), SQLAPI_MAXSTMTSIZE);
  }

  if(allok()) {
    size_t i;
    for(i = 0; i < bndstmt.m_stmtHead.m_ninput; i++) {
      HostVarIndex &inx = (*input)[i];
      HostVar      &var = m_hostvarList[inx.m_data];
      bndstmt.m_inHost[i].sqltype = var.type(inx);
      bndstmt.m_inHost[i].sqllen  = var.m_len;
    }
    for(i = 0; i < bndstmt.m_stmtHead.m_noutput; i++) {
      HostVarIndex &inx = (*output)[i];
      HostVar      &var = m_hostvarList[inx.m_data];
      bndstmt.m_outHost[i].sqltype = var.type(inx);
      bndstmt.m_outHost[i].sqllen  = var.m_len;
    }
    _tcscpy(bndstmt.m_stmt,m_stmtString);

    sqlapi_bndstmtfwrite(m_bndfile,bndstmt);
  }

  if(m_dumpbnd) {
    bndstmt.dump();
  }
}

void Parser::emitBndProgramId() {
  sqlapi_bndprogramidfwrite(m_bndfile,m_programid);
}

/* --------------------------------------------------------- */

void Parser::checkToken(Symbol expected) {
  if(token == expected) {
    nextToken();
  } else {
    error(SQL_SYNTAX_ERROR,_T("Expected %s"),tokstring(expected));
  }
}

void Parser::parseName(HostVar *var) {
  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected name"));
  } else {
    _tcscpy(var->m_name,the_name);
    if(findHostvar(var->m_name))
      error(SQL_HOSTVAR_ALREADY_DECLARED,_T("Hostvar %s already declared"), var->m_name);
    nextToken();
  }
}

void Parser::parseNameDef() {
  HostVar var;
  bool typesigned = true; /* default */

  memset(&var,0,sizeof(HostVar));

  if(token == SYM_STATIC) {
    nextToken();
  } else if(token == SYM_EXTERN) {
    nextToken();
  }

  if(token == SYM_UNSIGNED) {
    nextToken();
    typesigned = false;
  } else if(token == SYM_SIGNED) {
    nextToken();
  }

  if(token == SYM_SHORT) {
    if(typesigned) {
      var.m_type = DBTYPE_SHORT;
      var.m_len  = sizeof(short int);
    } else {
      var.m_type = DBTYPE_USHORT;
      var.m_len  = sizeof(unsigned short);
    }
    nextToken();
    if(token == SYM_INT) {
      nextToken();
    }
    parseName(&var);
  } else if(token == SYM_LONG) {
    if(typesigned) {
      var.m_type = DBTYPE_LONG;
      var.m_len  = sizeof(long int);
    } else {
      var.m_type = DBTYPE_ULONG;
      var.m_len  = sizeof(unsigned long);
    }
    nextToken();
    if(token == SYM_INT) {
      nextToken();
    }
    parseName(&var);
  } else {
    switch(token) {
    case SYM_CHAR  :
      if(typesigned) {
        var.m_type = DBTYPE_CHAR;
        var.m_len  = sizeof(char);
      } else {
        var.m_type = DBTYPE_UCHAR;
        var.m_len  = sizeof(unsigned char);
      }
      nextToken();
      parseName(&var);
      if(token == SYM_LBRACK) {
        nextToken();
        var.m_type = DBTYPE_CSTRING;
        if(token == SYM_NUMBER) {
          var.m_len = the_number;
          nextToken();
        } else {
          error(SQL_SYNTAX_ERROR,_T("Expected number"));
        }

        if(token == SYM_RBRACK) {
          nextToken();
        } else {
          error(SQL_SYNTAX_ERROR,_T("Expected ]"));
        }
      }
      break;

    case SYM_WCHAR  :
      if(typesigned) {
        var.m_type = DBTYPE_SHORT;
        var.m_len  = sizeof(short);
      } else {
        var.m_type = DBTYPE_USHORT;
        var.m_len  = sizeof(unsigned short);
      }
      nextToken();
      parseName(&var);
      if(token == SYM_LBRACK) {
        nextToken();
        var.m_type = DBTYPE_WSTRING;
        if(token == SYM_NUMBER) {
          var.m_len = the_number;
          nextToken();
        } else {
          error(SQL_SYNTAX_ERROR,_T("Expected number"));
        }

        if(token == SYM_RBRACK) {
          nextToken();
        } else {
          error(SQL_SYNTAX_ERROR,_T("Expected ]"));
        }
      }
      break;

    case SYM_INT   :
      if(typesigned) {
        var.m_type = DBTYPE_INT;
        var.m_len  = sizeof(int);
      } else {
        var.m_type = DBTYPE_UINT;
        var.m_len  = sizeof(unsigned int);
      }
      nextToken();
      parseName(&var);
      break;

    case SYM_LONG  :
      if(typesigned) {
        var.m_type = DBTYPE_LONG;
        var.m_len  = sizeof(long);
      } else {
        var.m_type = DBTYPE_ULONG;
        var.m_len  = sizeof(unsigned long);
      }
      nextToken();
      parseName(&var);
      break;

    case SYM_FLOAT :
      var.m_type = DBTYPE_FLOAT;
      var.m_len  = sizeof(float);
      nextToken();
      parseName(&var);
      break;

    case SYM_DOUBLE:
      var.m_type = DBTYPE_DOUBLE;
      var.m_len  = sizeof(double);
      nextToken();
      parseName(&var);
      break;

    case SYM_VARCHAR:
      var.m_type = DBTYPE_VARCHAR;
      var.m_len  = 0;
      nextToken();
      parseName(&var);
      if(token == SYM_LPAR) {
        nextToken();
        if(token == SYM_NUMBER) {
          nextToken();
        }
        if(token == SYM_RPAR) {
          nextToken();
        } else {
          error(SQL_SYNTAX_ERROR,_T("Expected )"));
        }
      }
      break;

    case SYM_DATE:
      var.m_type = DBTYPE_DATE;
      var.m_len  = sizeof(Date);
      nextToken();
      parseName(&var);
      break;

    case SYM_TIME:
      var.m_type = DBTYPE_TIME;
      var.m_len  = sizeof(Time);
      nextToken();
      parseName(&var);
      break;

    case SYM_TIMESTAMP:
      var.m_type = DBTYPE_TIMESTAMP;
      var.m_len  = sizeof(Timestamp);
      nextToken();
      parseName(&var);
      break;

    default:
      error(SQL_INVALID_DATATYPE,_T("Invalid datatype"));
    }
  }

  m_hostvarList.add(var);

  if(token == SYM_SEMI) {
    nextToken();
  } else {
    error(SQL_SYNTAX_ERROR,_T("Expected ;"));
    while(token != SYM_SEMI && token != SYM_EOF && token != SYM_EXEC_SQL) {
      nextToken();
    }
  }
}

void Parser::parseEndExec() {
  if(token == SYM_END_EXEC || token == SYM_SEMI) {
    m_stmtSize = getUntilLastSym(m_stmtString,false);

    if(m_dumpStatements) {
      _tprintf(_T("%s\n"),m_stmtString);
    }

    flushCollected();
    m_endexeclineno = yypos().m_line;
    emitEndComment(yypos().m_line);
    nextToken();
  } else { /* error-recovery */
    error(SQL_SYNTAX_ERROR,_T("Expected END-EXEC or ;"));
    while(token != SYM_END_EXEC && token != SYM_SEMI && token != SYM_EOF) {
      nextToken();
    }
  }
  if(token == SYM_SEMI) {
    nextToken();
  }
}

void Parser::parseDeclareSection() {
  if(token == SYM_DECLARE) {
    nextToken();
  }
  if(token == SYM_SECTION) {
    nextToken();
  }
  parseEndExec();

  while( token != SYM_EXEC_SQL && token != SYM_EOF ) {
    parseNameDef();
  }

  flushUntilLastSym(false);

  emitStartComment();
  emit(_T("  "));

  checkToken(SYM_EXEC_SQL);
  checkToken(SYM_END);
  checkToken(SYM_DECLARE);
  checkToken(SYM_SECTION);
  parseEndExec();
}

void Parser::parseIncludeSection() {
  if(token == SYM_SQLCA) {
    nextToken();
    parseEndExec();
    emit(_T("#include <sqlapi.h>\n"));
    emit(_T("sqlca sqlca;\n"));
    emitLineMark(yypos().m_line - 2);
  }
}

HostVar *Parser::parseStringHost() {
  HostVar *var = findHostvar(the_name);
  if(var == NULL) {
    error(SQL_UNDECLARED_HOSTVAR,_T("Undeclared HostVar:%s"),the_name);
  } else if((var->m_type != DBTYPE_CSTRING) && (var->m_type != DBTYPE_WSTRING)) {
    error(SQL_INVALID_DATATYPE,_T("Invalid type. Must be char or wchar_t"));
  }
  nextToken();
  return(var);
}

void Parser::parseIndicator(HostVarIndex &hostvarref) {
  hostvarref.m_ind = -1;
  if(token == SYM_INDICATOR) {
    nextToken();
    if(token != SYM_HOSTVAR) {
      error(SQL_SYNTAX_ERROR,_T("Expected indicator HostVar"));
      return;
    }
  }
  if(token == SYM_HOSTVAR) {
    int inx = findHostVarIndex(the_name);
    if(inx >= 0) {
      HostVar &ind = m_hostvarList[inx];
      if(ind.m_type != DBTYPE_SHORT)
        error(SQL_INVALID_DATATYPE,_T("Invalid datatype of indicator variable. Must be short int"));
    }
    hostvarref.m_ind = inx;
    nextToken();
  }
}

void Parser::parseHostvar(HostVarIndex &hostvarref) {
  hostvarref.m_data = findHostVarIndex(the_name);
  nextToken();
  parseIndicator(hostvarref);
}

void Parser::parseConnectto() {
  TCHAR dbname[MAX_STRING_LEN+1],
       username[MAX_STRING_LEN+1],
       password[MAX_STRING_LEN+1];
  HostVar *dbvar = NULL,*uservar = NULL,*passwordvar = NULL;
  bool connectUsing = false;

  switch(token) {
  case SYM_STRING:
    _tcscpy(dbname,the_string);
    nextToken();
    break;

  case SYM_NAME:
    _tcscpy(dbname,the_name);
    nextToken();
    break;

  case SYM_HOSTVAR:
    dbvar = parseStringHost();
    break;

  default:
    error(SQL_SYNTAX_ERROR,_T("Expected dbname"));
  }

  if(token == SYM_USER) {

    connectUsing = true;

    nextToken();
    switch(token) {
    case SYM_STRING:
      _tcscpy(username,the_string);
      nextToken();
      break;
    case SYM_NAME:
      _tcscpy(username,the_name);
      nextToken();
      break;
    case SYM_HOSTVAR:
      uservar = parseStringHost();
      break;
    default:
      error(SQL_SYNTAX_ERROR,_T("Expected username"));
    }
    checkToken(SYM_USING);
    switch(token) {
    case SYM_STRING:
      _tcscpy(password,the_string);
      nextToken();
      break;
    case SYM_NAME:
      _tcscpy(password,the_name);
      nextToken();
      break;
    case SYM_HOSTVAR:
      passwordvar = parseStringHost();
      break;
    default:
      error(SQL_SYNTAX_ERROR,_T("Expected password"));
    }
  }
  parseEndExec();

  if(allok()) {

    emitBegin();

    if(connectUsing) {
      emitDeclareVarlist(3);
      emitChkPgmid();
      emitSetVarTStr(0,dbvar      ,dbname  );
      emitSetVarTStr(1,uservar    ,username);
      emitSetVarTStr(2,passwordvar,password);
      emitBindVarlist(3);
      emitCall(SQL_CALL_CONNECT,SQL_CALL_CONNECT_USING,3,0);
    } else {
      emitDeclareVarlist(1);
      emitChkPgmid();
      emitSetVarTStr(0,dbvar      ,dbname  );
      emitBindVarlist(1);
      emitCall(SQL_CALL_CONNECT,SQL_CALL_CONNECT_DB,1,0);
    }

    emitEnd();
    emitLineMark(m_endexeclineno-1);
  }
}

void Parser::parseConnect() {
  switch(token) {
  case SYM_TO:
    nextToken();
    parseConnectto();
    break;

  case SYM_RESET:
    { nextToken();
      parseEndExec();
      if(allok()) {
        emitBegin();
        emitChkPgmid();
        emitCall(SQL_CALL_CONNECT,SQL_CALL_CONNECT_RESET,0,0);
        emitEnd();
        emitLineMark(m_endexeclineno-1);
      }
    }
    break;

  default:
    error(SQL_SYNTAX_ERROR,_T("Invalid syntax"));
    break;
  }
}

void Parser::parseSelect() {
  StaticStatement stmt;
  HostVarIndex    inx;

  stmt.m_nr = nextStatementCount();
  for(;;) {
    switch(token) {
    case SYM_HOSTVAR:
      parseHostvar(inx);
      if(allok()) {
        stmt.m_inhostvarlist.add(inx);
      }
      break;

    case SYM_INTO:
      goto parse_into;
    case SYM_END_EXEC:
    case SYM_SEMI:
    case SYM_EOF:
      error(SQL_SYNTAX_ERROR,_T("Expected INTO"));
      goto parse_end;
    default:
      nextToken();
      break;
    }
  }

parse_into:

  nextToken();
  while(token == SYM_HOSTVAR) {
    parseHostvar(inx);
    if(allok()) {
      stmt.m_outhostvarlist.add(inx);
    }
    if(token == SYM_COMMA) {
      nextToken();
    }
  }
  for(;;) {
    switch(token) {
    case SYM_HOSTVAR:
      parseHostvar(inx);
      if(allok())
        stmt.m_inhostvarlist.add(inx);
      break;
    case SYM_INTO:
      error(SQL_SYNTAX_ERROR,_T("Too many INTO-clauses"));
      nextToken();
      break;

    case SYM_END_EXEC:
    case SYM_SEMI:
      goto parse_end;
    case SYM_EOF:
      error(SQL_SYNTAX_ERROR,_T("Unexpected EOF"));
      goto parse_end;
    default:
      nextToken();
      break;
    }
  }
parse_end:
  parseEndExec();

  if(allok()) {
    m_staticstatementlist.add(stmt);
  }

  if(allok()) {
    const int ninput  = (UINT)stmt.m_inhostvarlist.size();
    const int noutput = (UINT)stmt.m_outhostvarlist.size();
    int i;
    emitBegin();
    emitDeclareVarlist(ninput+noutput);

    emitChkPgmid();

    for(i = 0; i < ninput; i++) {
      emitSetVar(i,stmt.m_inhostvarlist[i]);
    }
    for(i = 0; i < noutput; i++) {
      emitSetVar(ninput + i,stmt.m_outhostvarlist[i]);
    }
    emitBindVarlist(ninput+noutput);

    emitCall(SQL_CALL_EXECUTE,stmt.m_nr,ninput,noutput);
    emitEnd();

    emitBndStmt(stmt.m_nr,&stmt.m_inhostvarlist,&stmt.m_outhostvarlist);
  }
}

void Parser::parseStaticStatement() {
  StaticStatement stmt;
  HostVarIndex inx;

  stmt.m_nr = nextStatementCount();
  for(;;) {
    switch(token) {
    case SYM_HOSTVAR:
      parseHostvar(inx);
      if(allok())
        stmt.m_inhostvarlist.add(inx);
      break;

    case SYM_END_EXEC:
    case SYM_SEMI:
      goto parse_end;
    case SYM_EOF:
      error(SQL_SYNTAX_ERROR,_T("Unexpected EOF"));
      return;
    default:
      nextToken();
      break;
    }
  }
parse_end:
  parseEndExec();

  if(allok()) {
    m_staticstatementlist.add(stmt);
  }

  if(allok()) {
    const int ninput  = (int)stmt.m_inhostvarlist.size();
    emitBegin();
    if(ninput > 0)
      emitDeclareVarlist(ninput);

    emitChkPgmid();

    for(int i = 0; i < ninput; i++) {
      emitSetVar(i,stmt.m_inhostvarlist[i]);
    }
    emitBindVarlist(ninput);

    emitCall(SQL_CALL_EXECUTE,stmt.m_nr,ninput,0);
    emitEnd();

    emitBndStmt(stmt.m_nr,&stmt.m_inhostvarlist,&stmt.m_outhostvarlist);
  }
}

void Parser::parseUpdate() {
  parseStaticStatement();
}

void Parser::parseInsert() {
  parseStaticStatement();
}

void Parser::parseDelete() {
  parseStaticStatement();
}

void Parser::parseCursorSelect(CompactArray<HostVarIndex> &varlist) {
  while(token != SYM_END_EXEC && token != SYM_EOF && token != SYM_SEMI) {
    if(token == SYM_HOSTVAR) {
      HostVarIndex hvinx;
      parseHostvar(hvinx);
      if(hvinx.m_data >= 0) {
        varlist.add(hvinx);
      }
    } else {
      nextToken();
    }
  }
}

void Parser::parseDeclare() {
  Cursor            cursor;
  DynamicStatement *stmt;

  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected cursorname"));
  } else {
    _tcscpy(cursor.m_name,the_name);
    if(searchCursor(cursor.m_name)) {
      error(SQL_CURSOR_ALREADY_DECLARED,_T("Cursor %s already declared"),cursor.m_name);
    }

    nextToken();
  }

  checkToken(SYM_CURSOR);
  checkToken(SYM_FOR);

  switch(token) {
  case SYM_SELECT: /* static Cursor */
    nextToken();
    cursor.m_nr = nextStatementCount();
    parseCursorSelect(cursor.m_hostvarList);
    m_cursorList.add(cursor);
    break;

  case SYM_NAME:   /* dynamic statement */
    stmt = findDynStatement(the_name);
    if(allok()) {
      cursor.m_nr = stmt->m_nr;
    }
    nextToken();
    m_cursorList.add(cursor);
    break;

  default:
    error(SQL_SYNTAX_ERROR,_T("Expected SELECT or NAME"));
    break;
  }
  parseEndExec();

  emitBndStmt(cursor.m_nr,&cursor.m_hostvarList,NULL);
}

void Parser::parseOpen() {
  Cursor *cursor;
  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected cursorname"));
  } else {
    cursor = findCursor(the_name);
    nextToken();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    const int n = (int)cursor->m_hostvarList.size();
    if(n > 0) {
      emitDeclareVarlist(n);
    }
    emitChkPgmid();

    for(int i = 0; i < n; i++) {
      emitSetVar(i,cursor->m_hostvarList[i]);
    }
    if(n > 0) {
      emitBindVarlist(n);
    }
    emitCall(SQL_CALL_OPEN,cursor->m_nr,n,0);
    emitEnd();
  }
}

void Parser::parseFetchInto(CompactArray<HostVarIndex> &varlist) {
  HostVarIndex inx;

  while(token == SYM_HOSTVAR) {
    parseHostvar(inx);

    if(allok()) {
      varlist.add(inx);
    }

    if(token == SYM_COMMA) {
      nextToken();
    }
  }
}

void Parser::parseFetch() {
  Cursor *cursor;
  TCHAR sqldaname[MAX_NAME_LEN+1];
  CompactArray<HostVarIndex> varlist;

  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected cursorname"));
  } else {
    cursor = findCursor(the_name);
    nextToken();
  }

  switch(token) {
  case SYM_USING:
    nextToken();
    checkToken(SYM_DESCRIPTOR);
    if(token != SYM_HOSTVAR)
      error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
    else {
      _tcscpy(sqldaname,the_name);
      nextToken();
    }
    break;

  case SYM_INTO :
    nextToken();
    parseFetchInto(varlist);
    break;

  default:
    error(SQL_SYNTAX_ERROR,_T("Expected INTO or USING"));
  }
  parseEndExec();

  if(allok()) {
    const int n = (int)varlist.size();
    emitBegin();
    if(n > 0) {
      emitDeclareVarlist(n);
    }

    emitChkPgmid();

    if(n > 0) { /* fetch into ... */
      for(int i = 0; i < n; i++) {
        emitSetVar(i,varlist[i]);
      }
      emitBindVarlist(n);
      emitCall(SQL_CALL_FETCH,cursor->m_nr,0,n);
    } else { /* fetch using descriptor :da */
      emitBindDA(sqldaname);
      emitCall(SQL_CALL_FETCH,cursor->m_nr,0,1);
    }
    emitEnd();
    emitBndStmt(cursor->m_nr,NULL,&varlist);
  }
}

void Parser::parseClose() {
  Cursor *cursor;
  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected cursorname"));
  } else {
    cursor = findCursor(the_name);
    nextToken();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitCall(SQL_CALL_CLOSE,cursor->m_nr,0,0);
    emitEnd();
  }
}

void Parser::parsePrepare() {
  DynamicStatement stmt;
  TCHAR sqldaname[MAX_NAME_LEN+1];
  HostVar *var;

  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected name"));
  } else {
    _tcscpy(stmt.m_name,the_name);
    stmt.m_nr = nextStatementCount();
    if(searchDynStatement(stmt.m_name)) {
      error(SQL_STATEMENT_ALREADY_DEFINED,_T("Statement %s already defined"),stmt.m_name);
    } else {
      m_dynstatementList.add(stmt);
    }
    nextToken();
  }

  checkToken(SYM_INTO);

  if(token != SYM_HOSTVAR) {
    error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
  } else {
    _tcscpy(sqldaname,the_name);
    nextToken();
  }

  checkToken(SYM_FROM);

  if(token != SYM_HOSTVAR) {
    error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
  } else {
    var = parseStringHost();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitBindDA(sqldaname);
    emitSetStr(var);
    emitCall(SQL_CALL_PREPARE,stmt.m_nr,0,1);
    emitEnd();
  }
}

void Parser::parseDescribe() {
  DynamicStatement *stmt;
  TCHAR sqldaname[MAX_NAME_LEN+1];

  if(token != SYM_NAME) {
    error(SQL_SYNTAX_ERROR,_T("Expected statementname"));
  } else {
    stmt = findDynStatement(the_name);
    nextToken();
  }

  checkToken(SYM_INTO);

  if(token != SYM_HOSTVAR) {
    error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
  } else {
    _tcscpy(sqldaname,the_name); /* dont check if it is declared */
    nextToken();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitBindDA(sqldaname);
    emitCall(SQL_CALL_DESCRIBE,stmt->m_nr,0,1);
    emitEnd();
  }
}

void Parser::parseImmediate() {
  HostVar *var;

  if(token != SYM_HOSTVAR) {
    error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
  } else {
    var = parseStringHost();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitSetStr(var);
    emitCall(SQL_CALL_IMMEDIATE,0,0,0);
    emitEnd();
  }
}

void Parser::parse_execute_dynamic() {
  DynamicStatement *stmt = findDynStatement(the_name);
  TCHAR sqldaname[MAX_NAME_LEN+1];
  nextToken();
  checkToken(SYM_USING);
  checkToken(SYM_DESCRIPTOR);
  if(token != SYM_HOSTVAR) {
    error(SQL_SYNTAX_ERROR,_T("Expected HostVar"));
  } else {
    _tcscpy(sqldaname,the_name); /* dont check if it is declared */
    nextToken();
  }

  parseEndExec();

  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitBindDA(sqldaname);
    emitCall(SQL_CALL_EXECUTE,stmt->m_nr,1,0);
    emitEnd();
  }
}

void Parser::parse_execute() {
  switch(token) {
  case SYM_IMMEDIATE:
    nextToken();
    parseImmediate();
    break;
  case SYM_NAME:
    parse_execute_dynamic();
    break;
  default:
    error(SQL_SYNTAX_ERROR,_T("Expected IMMEDIATE or name"));
    parseEndExec();
  }
}

void Parser::parseCommit() {
  if(token == SYM_WORK)
    nextToken();
  parseEndExec();
  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitCall(SQL_CALL_COMMIT,0,0,0);
    emitEnd();
  }
}

void Parser::parseRollback() {
  if(token == SYM_WORK) {
    nextToken();
  }
  parseEndExec();
  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitCall(SQL_CALL_ROLLBACK,0,0,0);
    emitEnd();
  }
}

void Parser::parseCreate() {
  StaticStatement stmt;
  stmt.m_nr = nextStatementCount();

  while(token != SYM_END_EXEC && token != SYM_EOF && token != SYM_SEMI) {
    nextToken();
  }

  parseEndExec();
  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitCall(SQL_CALL_EXECUTE,stmt.m_nr,0,0);
    emitEnd();
    emitBndStmt(stmt.m_nr, NULL, NULL );
  }
}

void Parser::parseDrop() {
  StaticStatement stmt;
  stmt.m_nr = nextStatementCount();

  while(token != SYM_END_EXEC && token != SYM_EOF && token != SYM_SEMI) {
    nextToken();
  }

  parseEndExec();
  if(allok()) {
    emitBegin();
    emitChkPgmid();
    emitCall(SQL_CALL_EXECUTE,stmt.m_nr,0,0);
    emitEnd();
    emitBndStmt(stmt.m_nr, 0, 0 );
  }
}

void Parser::parseSQL() {
  switch( token ) {
  case SYM_BEGIN:
    nextToken();
    parseDeclareSection();
    break;

  case SYM_INCLUDE:
    nextToken();
    parseIncludeSection();
    break;

  case SYM_CONNECT :
    nextToken();
    parseConnect();
    break;

  case SYM_SELECT  :
    nextToken();
    parseSelect();
    break;

  case SYM_UPDATE  :
    nextToken();
    parseUpdate();
    break;

  case SYM_INSERT  :
    nextToken();
    parseInsert();
    break;

  case SYM_DELETE  :
    nextToken();
    parseDelete();
    break;

  case SYM_DECLARE :
    nextToken();
    parseDeclare();
    break;

  case SYM_OPEN    :
    nextToken();
    parseOpen();
    break;

  case SYM_FETCH   :
    nextToken();
    parseFetch();
    break;

  case SYM_CLOSE   :
    nextToken();
    parseClose();
    break;

  case SYM_PREPARE :
    nextToken();
    parsePrepare();
    break;

  case SYM_DESCRIBE:
    nextToken();
    parseDescribe();
    break;

  case SYM_EXECUTE :
    nextToken();
    parse_execute();
    break;

  case SYM_COMMIT  :
    nextToken();
    parseCommit();
    break;

  case SYM_ROLLBACK:
    nextToken();
    parseRollback();
    break;

  case SYM_CREATE:
    nextToken();
    parseCreate();
    break;

  case SYM_DROP:
    nextToken();
    parseDrop();
    break;

  default:
    error(SQL_SYNTAX_ERROR,_T("Unknown SQL-statement"));
    while( token != SYM_END_EXEC && token != SYM_SEMI && token != SYM_EOF ) {
      flushUntilLastSym(false);
      nextToken();
    }
    parseEndExec();
  }
}

void Parser::parseFile() {

  emitDeclarePgmid();
  for(;;) {
    switch( token ) {
    case SYM_EXEC_SQL:

      flushUntilLastSym(false);

      emitStartComment();
      emit(_T("  "));
      flushCollected();

      m_stmtpos = yypos();
      nextToken();
      parseSQL();
      break;

    case SYM_EOF:
      goto end;

    default:
      flushUntilLastSym(false);
      nextToken();
      break;
    }
  }
end:
  flushCollected();
  if(errorCount() > 0) {
    emitErrorMark();
  }
}

void Parser::genprogramid() {
  time_t Timestamp;
  TCHAR *nl;
  time(&Timestamp);
  _tcsncpy(m_programid.m_fileName,currentFileName(),ARRAYSIZE(m_programid.m_fileName));
  _tcscpy(m_programid.m_timestamp,_tctime(&Timestamp));
  if(nl = _tcschr(m_programid.m_timestamp,'\n')) {
    *nl = 0;
  }
}

void Parser::dumpSymbolTable() {
  for(size_t i = 0; i < m_hostvarList.size(); i++) {
    m_hostvarList[i].print();
  }
  for(size_t i = 0; i < m_cursorList.size(); i++) {
    cursorPrint(m_cursorList[i]);         ;
  }
  for(size_t i = 0; i < m_dynstatementList.size(); i++) {
    dynstatementprint(m_dynstatementList[i]);
  }
  for(size_t i = 0; i < m_staticstatementlist.size(); i++) {
    staticStatementPrint(m_staticstatementlist[i]);
  }
}

static void usage( void ) {
  _ftprintf(stderr,
            _T("Usage:sqlprep [-d] [-s] [-S] file(s)\n"
               "      -d:Trace lexical scanner\n"
               "      -s:Dump syboltable\n"
               "      -S:Dump statementtable\n"
               "      -b:Dump bnd-file\n"
               "      -l:Dont generate #line statements\n")
         );
  exit(-1);
}

int _tmain( int argc, TCHAR **argv ) {
  TCHAR *cp;
  bool lex_trace       = false;
  bool dumpSymbolTable = false;
  bool dumpstatements  = false;
  bool dumpbnd         = false;
  bool genlinemarks    = true;

  argvExpand(argc,argv);

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for( cp++; *cp; cp++ ) {
      switch( *cp ) {
      case 'd': lex_trace      = true ; continue;
      case 's': dumpSymbolTable= true ; continue;
      case 'S': dumpstatements = true ; continue;
      case 'b': dumpbnd        = true ; continue;
      case 'l': genlinemarks   = false; continue;
      default : usage();
      }
      break;
    }
  }

  if( !*argv ) {
    usage();
  }
  bool allok = true;

  for(;*argv; argv++) {
    Parser p(*argv);
    if(p.allok()) {
      p.m_dumpStatements = dumpstatements;
      p.m_dumpbnd        = dumpbnd;
      p.m_lex_trace      = lex_trace;
      p.m_genLineMarks   = genlinemarks;

      p.parseFile();

      if(dumpSymbolTable) {
        p.dumpSymbolTable();
      }
    }

    _ftprintf(p.m_errorfile,_T("%d error(s).\n")  , p.errorCount());
    _ftprintf(p.m_errorfile,_T("%d warning(s).\n"), p.warningCount());

    if(p.errorCount() > 0) {
      allok = false;
      break;
    }
  }
  return allok ? 0 : -1;
}
