#include "stdafx.h"
#include <Tokenizer.h>
#include <signal.h>
#include <errno.h>
#include <direct.h>
#include <io.h>
#include <MyString.h>
//#include <SqlRegex.h>

extern void testTupleField();

static TCHAR *indexTypeStr(char type) {
  switch(type) {
  case INDEXTYPE_PRIMARY    : return _T("Primary key");
  case INDEXTYPE_UNIQUE     : return _T("Unique index");
  case INDEXTYPE_NON_UNIQUE : return _T("Index");
  default:
    return _T("Unknown indextype");
  }
}

static bool userbreak = false;
class UserBreak : public Exception {
public:
  UserBreak() : Exception(_T("Interrupted by user")) {}
};

void interruptHandler(int sig) {
//  _tprintf(_T("Interrupt\n"));
  userbreak = true;
}

static String getCommand(FILE *f) { // reads command from f
  String stmt;

  for(;;) {
    if(isatty(f)) {
      userbreak = false;
      signal(SIGINT,interruptHandler);
      signal(SIGBREAK,interruptHandler);
      _tprintf(_T(">"));
    }
    TCHAR line[4000];
    if(FGETS(line,ARRAYSIZE(line),f) == NULL && !isatty(f)) {
      break;
    }
    if(userbreak) {
      throw UserBreak();
    }
    if(line[0] == '-') continue; // commentline
    String tmp = line;
    tmp.trimRight();
    size_t len = tmp.length();
    if(len > 0 && tmp[len-1] == ';') {
      stmt += left(tmp,len-1);
      return stmt;
    }
    stmt += line + String(_T("\n\r"));
  }
  return stmt;
}

class Session {
private:
  Database &m_db;
  FILE     *m_output;

public:
  Session(Database &db) : m_db(db) { m_output = stdout; }
  ~Session() { setOutput(EMPTYSTRING); }
  void testTableCursor(const String &tableName);
  void dumpData(const String &tableName);
  void dumpKeyFile(const String &indexName);
  void dumpKeydef(const String &indexName);
  void setOutput(const String &fname);
  void invoke(const String &tableName);
  void compile(const String &str);
  void sqlexec(const String &str);
  bool trySpecialCommand(const String &stmt);
  void doCommand(const String &stmt);
  void run(const String &fname);
  void run(FILE *f);
};

void Session::testTableCursor(const String &tableName) {
  const TableDefinition &tableDef  = m_db.getTableDefinition(tableName);
  const TableInfo       &tableInfo = m_db.getTableInfo(tableDef.getSequenceNo());
  tableInfo.dump();
  TableCursorParam param;
  param.m_sequenceNo      = tableInfo.getSequenceNo();
  param.m_indexName       = tableInfo.getIndex(0).m_indexName;
  param.m_indexOnly       = false;
  param.m_beginRelOp      = RELOP_TRUE;
  param.m_beginFieldCount = 0;
  param.m_endRelOp        = RELOP_TRUE;
  param.m_endFieldCount   = 0;
  param.m_dir             = SORT_ASCENDING;

  for(UINT i = 0; i < tableDef.getColumnCount(); i++)
    param.m_fieldSet.add(i);

  Tuple tuple(tableDef.getColumnCount());
  TableCursor cursor(m_db, param, NULL, NULL );
  while(cursor.hasNext()) {
    cursor.next(tuple);
    if(userbreak) throw UserBreak();
    tuple.dump(m_output);
  }
}

void createDatabase(const String &dbname) {
  SysTableDbData dbdef;
  _tcscpy(dbdef.m_dbName,dbname.cstr());
  _stprintf(dbdef.m_path,_T("c:\\%s"),dbname.cstr());
  for(int i = 0; i < 256;i++) dbdef.m_colseq[i] = i;
  Database::create(dbdef);
}

void Session::dumpData(const String &tableName) {
  const TableDefinition &tableDef = m_db.getTableDefinition(tableName);
  DataFile datafile(m_db,tableDef.getFileName(),DBFMODE_READONLY,false);
  m_db.dataFileDump(tableName,m_output);
  datafile.freeTreeDump(m_output);
}

void Session::dumpKeyFile(const String &indexName) {
  const IndexDefinition &indexDef = m_db.getIndexDefinition(indexName);
  KeyFile index(indexDef.m_fileName,DBFMODE_READONLY,NULL);
  index.dump(m_output,FULL_DUMP);
}

void Session::dumpKeydef(const String &indexName) {
  const IndexDefinition &indexDef = m_db.getIndexDefinition(indexName);
  KeyFile index(indexDef.m_fileName,DBFMODE_READONLY,NULL);
  KeyFileDefinition keydef(index);
  keydef.dump(m_output);
}

void Session::setOutput(const String &fname) {
  if(m_output != stdout) { 
    fclose(m_output);
    m_output = stdout;
  }
  if(fname.length() != 0) {
    m_output = fopen(fname,_T("w"));
    if(m_output == NULL) {
      _tperror(fname.cstr());
      m_output = stdout;
    }
  }
}

void Session::invoke(const String &tableName) {
  TableDefinition tableDef;
  IndexArray      indexArray;

  ULONG sequenceno;
  if(_stscanf(tableName.cstr(),_T("%d"),&sequenceno) == 1) {
    tableDef   = m_db.getTableDefinition(sequenceno);
    indexArray = m_db.getIndexDefinitions(tableDef.getTableName());
  } else {
    tableDef   = m_db.getTableDefinition(tableName);
    indexArray = m_db.getIndexDefinitions(tableName);
  }
  tableDef.dump(m_output);
  _ftprintf(m_output,_T("Indexes:\n"));
  for(size_t i = 0; i < indexArray.size(); i++) {
    IndexDefinition &indexDef = indexArray[i];
    _ftprintf(m_output,_T("  %s %s\n"),indexTypeStr(indexDef.m_indexType),indexDef.m_indexName.cstr());
    for(UINT j = 0; j < indexDef.getColumnCount(); j++) {
      const IndexColumn &indexColumn = indexDef.getColumn(j);
      _ftprintf(m_output,_T("    %2u:%-30s %s\n"),j,tableDef.getColumn(indexColumn.m_col).m_name.cstr(), indexColumn.m_asc?_T("ASC"):_T("DESC"));
    }
    _ftprintf(m_output,_T("\n"));
  }
}

void Session::compile(const String &str) {
  VirtualCode   vc;
  sqlca         ca;
  StringArray   errmsg;
  if(sqlCompile(m_db,str.cstr(),vc,errmsg,ca)) {
    vc.dump(m_output);
  } else {
    for(size_t i = 0; i < errmsg.size(); i++) {
      _tprintf(_T("%s\n"),errmsg[i].cstr());
    }
    ca.dump();
  }
}

void Session::sqlexec(const String &str) {
  const TCHAR *stmt = str.cstr();
  if(_tcsnicmp(stmt,_T("select"),6) == 0) {
    SqlCursor cursor(m_db,stmt);
    if(cursor.ok()) {
      cursor.getDescription().dump();
      TuplePrintDefinition printwidth(cursor.getDescription());
      Tuple tuple((UINT)cursor.getDescription().size());
      int count = 0;
      while(cursor.fetch(tuple)) {
        if(userbreak) throw UserBreak();
        _ftprintf(m_output,_T("%s\n"),tuple.toString(printwidth).cstr());
        count++;
      }
      if(cursor.m_sqlca.sqlcode != SQL_OK && cursor.m_sqlca.sqlcode != SQL_NOT_FOUND)
        cursor.m_sqlca.dump(m_output);
      if(count == 0)
        _ftprintf(m_output,_T("no records found\n "));
    } else {
      for(size_t i = 0; i < cursor.m_errmsg.size(); i++) {
        _ftprintf(m_output,_T("%s\n"),cursor.m_errmsg[i].cstr());
      }
    }
    return;
  }

  sqlca           ca;
  VirtualCode     vc;
  StringArray     errmsg;
  if(sqlCompile(m_db,stmt,vc,errmsg,ca)) {
//    _ftprintf(m_output,_T("vc:\n"));
//    vc.dump(m_output);
    HostVarList hvar;
    sqlExecute(m_db,vc,hvar,ca);
    if(ca.sqlcode != SQL_OK)
      throw ca;
  } else {
    for(size_t i = 0; i < errmsg.size(); i++) {
      _ftprintf(m_output,_T("%s\n"),errmsg[i].cstr());
    }
    throw ca;
  }
}

static void showhelp(TCHAR *command) {
  const String cmd = toLowerCase(command);
  if(cmd.length() == 0) {
    _tprintf(_T("Available commands:\n"
              "  compile  create  dbdump  dir   drop  dump  dump1       dumpdata  dumpkey  dumpkeydef  edit\n"
              "  exit     help    invoke  list  out   quit  removedata  run       update   updatedata\n")
          );
    return;
  }
  if(cmd == _T("compile")) {
    _tprintf(_T("compile file\n"));
    return;
  }
  if(cmd == _T("create")) {
    _tprintf(_T("CREATE Database name\n"));
    _tprintf(_T("CREATE TABLE name LIKE name\n"));
    _tprintf(_T("CREATE TABLE name ( columns, PRIMARY KEY ( keyfields ) )\n"));
    _tprintf(_T("CREATE [UNIQUE] INDEX name ON name ( keyfields )\n"));
    return;
  }
  if(cmd == _T("dbdump")) {
    _tprintf(_T("dbdump\n"));
    return;
  }
  if(cmd == _T("dir")) {
    _tprintf(_T("dir\n"));
    return;
  }
  if(cmd == _T("drop")) {
    _tprintf(_T("DROP Database name\n"));
    _tprintf(_T("DROP TABLE tablename\n"));
    _tprintf(_T("DROP INDEX indexname\n"));
    return;
  }
  if(cmd == _T("dump")) {
    _tprintf(_T("dump tablename\n"));
    _tprintf(_T("dump index indexname\n"));
    return;
  }
  if(cmd == _T("dump1")) {
    _tprintf(_T("dump1 tablename\n"));
    return;
  }
  if(cmd == _T("dumpdata")) {
    _tprintf(_T("dumpdata tablename\n"));
    return;
  }
  if(cmd == _T("dumpkey")) {
    _tprintf(_T("dumpkey indexname\n"));
    return;
  }
  if(cmd == _T("dumpkeydef")) {
    _tprintf(_T("dumpkeydef indexname\n"));
    return;
  }
  if(cmd == _T("edit")) {
    _tprintf(_T("edit filename\n"));
    return;
  }
  if(cmd == _T("exit")) {
    _tprintf(_T("exit\n"));
    return;
  }
  if(cmd == _T("quit")) {
    _tprintf(_T("quit\n"));
    return;
  }
  if(cmd == _T("invoke")) {
    _tprintf(_T("invoke tablename\n"));
    return;
  }
  if(cmd == _T("list")) {
    _tprintf(_T("list filename\n"));
    return;
  }
  if(cmd == _T("out")) {
    _tprintf(_T("out [filename]\n"));
    return;
  }
  if(cmd == _T("removedata")) {
    _tprintf(_T("removedata tablename\n"));
    return;
  }
  if(cmd == _T("run")) {
    _tprintf(_T("run filename\n"));
    return;
  }
  if(cmd == _T("update")) {
    _tprintf(_T("update statistics [tablename]\n"));
    return;
  }
  if(cmd == _T("updatedata")) {
    _tprintf(_T("updatedata tablename\n"));
    return;
  }
}

static void error(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  String tmp = vformat(format, argptr);
  va_end(argptr);
  throwException(tmp);
}

typedef enum {
   COMMAND_OUT
  ,COMMAND_QUIT
  ,COMMAND_CD
  ,COMMAND_DBDUMP
  ,COMMAND_LIST
  ,COMMAND_INVOKE
  ,COMMAND_DUMP1
  ,COMMAND_DUMPKEY
  ,COMMAND_DUMPKEYDEF
  ,COMMAND_DUMPDATA
  ,COMMAND_REMOVEDATA
  ,COMMAND_UPDATEDATA
  ,COMMAND_RUN
  ,COMMAND_COMPILE
  ,COMMAND_UPDATE
  ,COMMAND_HELP
  ,COMMAND_DUMP
  ,COMMAND_CREATE
  ,COMMAND_DROP
  ,COMMAND_UNKNOWN
} Command;

class CommandToken {
public:
  TCHAR  *str;
  Command cmd;
};

static CommandToken commandList[] = {
  _T("out")       ,COMMAND_OUT
 ,_T("quit")      ,COMMAND_QUIT
 ,_T("exit")      ,COMMAND_QUIT
 ,_T("cd")        ,COMMAND_CD
 ,_T("dbdump")    ,COMMAND_DBDUMP
 ,_T("list")      ,COMMAND_LIST
 ,_T("invoke")    ,COMMAND_INVOKE
 ,_T("dump1")     ,COMMAND_DUMP1
 ,_T("dumpkey")   ,COMMAND_DUMPKEY
 ,_T("dumpkeydef"),COMMAND_DUMPKEYDEF
 ,_T("dumpdata")  ,COMMAND_DUMPDATA
 ,_T("removedata"),COMMAND_REMOVEDATA
 ,_T("updatedata"),COMMAND_UPDATEDATA
 ,_T("run")       ,COMMAND_RUN
 ,_T("compile")   ,COMMAND_COMPILE
 ,_T("update")    ,COMMAND_UPDATE
 ,_T("help")      ,COMMAND_HELP
 ,_T("dump")      ,COMMAND_DUMP
 ,_T("create")    ,COMMAND_CREATE
 ,_T("drop")      ,COMMAND_DROP
};

Command findCommand(const TCHAR *str) {
  for(int i = 0; i < ARRAYSIZE(commandList); i++) {
    if(_tcsicmp(commandList[i].str,str) == 0)
      return commandList[i].cmd;
  }
  return COMMAND_UNKNOWN;
}

bool Session::trySpecialCommand(const String &stmt) {
  Tokenizer tok(stmt,_T(" \n\r"));
  if(!tok.hasNext())
    return false;

  String cmd = tok.next();
  String arg1String,arg2String;
  TCHAR *arg1 = NULL,*arg2 = NULL;
  if(tok.hasNext()) {
    arg1String = tok.next();
    arg1 = arg1String.cstr();
  }
  if(tok.hasNext()) {
    arg2String = tok.next();
    arg2 = arg2String.cstr();
  }

  switch(findCommand(cmd.cstr())) {
  case COMMAND_OUT:
    if(arg1 == NULL) 
      setOutput(EMPTYSTRING);
    else
      setOutput(arg1);
    return true;

  case COMMAND_QUIT:
    exit(0);
    return true;

  case COMMAND_CD:
    if(arg1 != NULL) {
      chdir(arg1);
      return true;
    }
    return false;

  case COMMAND_DBDUMP:
    { bool dumpall = false;
      if(arg1 != NULL && _tcsicmp(arg1,_T("all")) == 0)
        dumpall = true;
      m_db.dump(m_output,dumpall);
      return true;
    }

  case COMMAND_LIST:
    { if(arg1 == NULL)
        error(_T("Invalid syntax. Usage:list <filename>"));
      _tprintf(_T("%s"), readTextFile(arg1).cstr());
      return true;
    }
  
  case COMMAND_INVOKE:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:invoke <tablename>"));
    invoke(arg1);
    return true;

  case COMMAND_DUMP1:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:dump1 <tablename>"));
    testTableCursor(arg1);
    return true;

  case COMMAND_DUMPKEY:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:dumpkey <indexname>"));
    dumpKeyFile(arg1);
    return true;

  case COMMAND_DUMPKEYDEF:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:dumpkeydef <indexname>"));
    dumpKeydef(arg1);
    return true;

  case COMMAND_DUMPDATA:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:dumpdata <tablename>"));
    dumpData(arg1);
    return true;

  case COMMAND_RUN:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:run <filename>"));
    run(arg1);
    return true;

  case COMMAND_COMPILE:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:compile <filename>"));
    compile(readTextFile(arg1));
    return true;

  case COMMAND_UPDATE:
    if(arg1 == NULL) 
      return false;
    if(_tcsicmp(arg1,_T("statistics")) == 0) {
      if(arg2 == NULL)
        m_db.updateStatistics();
      else
        m_db.updateStatistics(arg2);
      return true;
    }
    return false;

  case COMMAND_HELP:
    if(arg1 == NULL)
      showhelp(EMPTYSTRING);
    else
      showhelp(arg1);
    return true;

  case COMMAND_DUMP:
    if(arg1 == NULL)
      error(_T("Invalid syntax. Usage:dump [index] <name>"));
    if(_tcsicmp(arg1,_T("index")) == 0) {
      if(arg2 == NULL)
        error(_T("Invalid syntax. Usage:dump index <name>"));
      m_db.KeyFiledump(arg2,m_output);
    }
    else
      m_db.dataFileDump(arg1,m_output);
    return true;

  case COMMAND_CREATE:
    if(arg1 == NULL || _tcsicmp(arg1,_T("Database")) != 0)
      return false;
    if(arg2 == NULL)
      error(_T("Invalid syntax. Usage:create Database <name>"));
    createDatabase(arg2);
    return true;

  case COMMAND_DROP:
    if(arg1 == NULL || _tcsicmp(arg1,_T("Database")) != 0)
      return false;
    if(arg2 == NULL)
      error(_T("Invalid syntax. Usage:drop Database <name>"));
    Database::destroy(arg2);
    return true;

  default:
    return false;
  }
}

static const TCHAR *sqlCommands[] = {
  _T("update"),
  _T("select"),
  _T("insert"),
  _T("delete"),
  _T("drop"),
  _T("create"),
};

static bool isSqlCommand(const String &stmt) {
  Tokenizer tok(stmt,_T(" \n\r"));
  if(!tok.hasNext())
    return false;

  String cmd = tok.next();
  for(int i = 0; i < ARRAYSIZE(sqlCommands); i++)
    if(_tcsicmp(cmd.cstr(),sqlCommands[i]) == 0)
      return true;
  return false;
}

void Session::doCommand(const String &stmt) {
  if(trySpecialCommand(stmt))
    return;
  if(isSqlCommand(stmt)) {
    sqlexec(stmt);
  } else {
    _tsystem(stmt.cstr());
  }
}

void Session::run(const String &fname) {
  FILE *f = fopen(fname,_T("r"));
  if(f == NULL) {
    throwErrNoOnNameException(fname);
  }
  try {
    run(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void Session::run(FILE *f) {
  for(;;) {
    try {
      String command = getCommand(f);
      if(command.length() == 0)
        return;
      doCommand(command);
    } catch(sqlca ca) {
      if(f != stdin) 
        throw ca;
      else
        ca.dump(m_output);
    } catch(UserBreak b) {
      if(f != stdin) 
        throw b;
      else
        _ftprintf(m_output,_T("%s\n"),b.what());
    } catch(Exception e) {
      if(f != stdin) 
        throw e;
      else
        _ftprintf(m_output,_T("Error:%s\n"),e.what());
    }
  }
}


static void usage() {
  _ftprintf(stderr,_T("Usage:testcomp [options] dbname [file]\n"
                      "      testComp -cDbName\n"
                      "   Options:\n")
         );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  TCHAR *dbname     = NULL;

//  SqlLex::findBestHashMapSize();
//  testTupleField();
//  SqlRegex::testSqlRegex();

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'c':
          dbname = cp+1;
          createDatabase(dbname);
          break;
        default :
          usage();
        }
        break;
      }
    }

    if(dbname == NULL) {
      if(!*argv) {
        usage();
      }
      dbname = *(argv++);
    }
    Database db(dbname);

    Session session(db);
    session.run(stdin);
  } catch(sqlca ca) {
    ca.dump();
  }
  return 0;
}
