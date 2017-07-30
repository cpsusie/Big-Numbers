static char sqlapi_program_id[240] = {
115,  0,113,  0,108,  0, 98,  0,105,  0,110,  0,100,  0, 46,  0,115,  0,113,  0,
 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 83,  0,117,  0,110,  0, 32,  0, 74,  0,117,  0,108,  0, 32,  0, 51,  0, 48,  0,
 32,  0, 49,  0, 56,  0, 58,  0, 53,  0, 50,  0, 58,  0, 48,  0, 51,  0, 32,  0,
 50,  0, 48,  0, 49,  0, 55,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

};
/* sqlbind.sqc Sun Jul 30 18:52:03 2017 */
#line 1 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
#include "targetver.h"
#include <sqlapi.h>
#include <comdef.h>
#include <atlconv.h>


#ifdef __NEVER__
  EXEC SQL INCLUDE SQLCA;
#endif
#line 6 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
#include <sqlapi.h>
sqlca sqlca;
#line 6 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"



#ifdef __NEVER__
  EXEC SQL BEGIN DECLARE SECTION;
#endif
#line 8 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"


  static char dbname[20];
  static char username[30];
  static char password[30];


#ifdef __NEVER__
  EXEC SQL END DECLARE SECTION;
#endif
#line 14 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"


static void dbConnect() {


#ifdef __NEVER__
  EXEC SQL CONNECT TO :dbname user :username using :password;
#endif
#line 18 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
{
  SqlApiVarList sqlapi_varl[3];
#line 19 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
  sqlapi_varl[0].sqltype = 29;
  sqlapi_varl[0].sqllen  = 20;
  sqlapi_varl[0].sqldata = (void*)dbname;
  sqlapi_varl[0].sqlind = 0L;
#line 19 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
  sqlapi_varl[1].sqltype = 29;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)username;
  sqlapi_varl[1].sqlind = 0L;
#line 19 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
  sqlapi_varl[2].sqltype = 29;
  sqlapi_varl[2].sqllen  = 30;
  sqlapi_varl[2].sqldata = (void*)password;
  sqlapi_varl[2].sqlind = 0L;
#line 19 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
  sqlapi_call(1,5,sqlapi_program_id,3,0,sqlapi_varl,sqlca);
#line 19 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
}
#line 18 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
#line 17 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"


  if(sqlca.sqlcode != SQL_OK) {
    printf("Cannot connect to database <%s>\n",dbname);
    sqlca.dump();
    exit(-1);
  }
}

static void dbDisconnect() {

#ifdef __NEVER__
  EXEC SQL CONNECT RESET END-EXEC
#endif
#line 28 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
{
  sqlapi_call(1,3,sqlapi_program_id,0,0,NULL,sqlca);
#line 28 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
}
#line 27 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"
#line 27 "C:/mytools2015/Dbase/SqlBind/sqlbind.sqc"

}

static bool verbose = false;

static void message(const TCHAR *format,...) {
  if(!verbose) return;
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format, argptr);
  va_end(argptr);
}

static bool bindProgram(const String &inputfname) {
  SqlApiBindProgramId programid;
  SqlApiBindStmt      bndstmt;
  FILE               *bndfile;
  String              tmpStmt;
  bool                allok = true;

  bndfile = fopen( inputfname, _T("rb"));
  if(!bndfile) {
    _tperror(inputfname.cstr());
    return false;
  }
  if(!sqlapi_bndprogramidfread(bndfile,programid)) {
    _ftprintf(stderr,_T("sqlapi_bndprogramidfread failed on %s\n"),inputfname.cstr());
    fclose(bndfile);
    return false;
  }

  sqlapi_unbind(programid.m_fileName,sqlca);

  while(sqlapi_bndstmtfread(bndfile,bndstmt)) {
    tmpStmt = bndstmt.m_stmt;
    message(_T("Now bind <%s ...>\n"),left(tmpStmt.replace(_T('\n'),_T(' ')),60).cstr());
    StringArray errmsg;

    sqlapi_bind(programid,bndstmt,errmsg,sqlca);

    if(sqlca.sqlcode != SQL_OK) {
      if(errmsg.size() == 0) {
        sqlca.dump();
      } else {
        for(size_t i = 0; i < errmsg.size(); i++) {
          _tprintf(_T("%s\n"),errmsg[i].cstr());
        }
      }
      allok = false;
    }
  }

  fclose(bndfile);
  return allok;
}

static bool bindProgram(const String &db, TCHAR **filenames) {
  USES_CONVERSION;
  bool        allok  = true;
  const char *adbstr = T2A(db.cstr());

  strcpy(dbname  ,adbstr);
  strcpy(username,"ADM");
  strcpy(password,"ADM");

  dbConnect();

  for(;*filenames; filenames++) {
    if(bindProgram(*filenames)) {
      message(_T("Bind %s succeeded.\n"),*filenames);
    } else {
      allok = false;
      _tprintf(_T("Bind %s failed.\n"), *filenames);
      break;
    }
  }
  dbDisconnect();
  return allok;
}

static bool unbindProgram(const String &db, TCHAR **filenames) {
  USES_CONVERSION;
  bool        allok  = true;
  const char *adbstr = T2A(db.cstr());

  strcpy(dbname  ,adbstr);
  strcpy(username,"ADM" );
  strcpy(password,"ADM" );

  dbConnect();

  for(;*filenames; filenames++) {
    sqlapi_unbind(*filenames,sqlca);
    if(sqlca.sqlcode == SQL_OK) {
      message(_T("Unbind %s succeeded.\n"),*filenames);
    } else if(sqlca.sqlcode == SQL_NOT_FOUND) {
      _tprintf(_T("Warning:")); sqlca.dump();
    } else {
      allok = false;
      sqlca.dump();
      _tprintf(_T("Unbind %s failed.\n"), *filenames);
      break;
    }
  }

  dbDisconnect();
  return allok;
}

static bool createDatabase(const String &dbname) {
  SqlApiCreateDb crdb;
  int i;

  if(dbname.length() >= ARRAYSIZE(crdb.m_dbname)) {
    _ftprintf(stderr,_T("dbname <%s> too long. Maxsize=%zd\n"),dbname.cstr(),ARRAYSIZE(crdb.m_dbname)-1);
    return false;
  }
  memset(&crdb,0,sizeof(crdb));
  _tcscpy(crdb.m_dbname,dbname.cstr());
  crdb.m_drive = 'c';
  for(i=0;i<256;i++) crdb.m_colseq[i] = i;
  sqlapi_createdb( crdb, sqlca);
  if(sqlca.sqlcode != SQL_OK) {
    _tprintf(_T("create database failed. "));
    sqlca.dump();
    return false;
  } else {
    message(_T("create database <%s> succeeded\n"),dbname.cstr());
    return true;
  }
}

static bool dropDatabase(const String &dbname) {
  sqlapi_dropdb( dbname.cstr(), sqlca);
  if(sqlca.sqlcode != SQL_OK) {
    _tprintf(_T("drop database failed. "));
    sqlca.dump();
    return false;
  } else {
    message(_T("drop database <%s> succeeded\n"),dbname.cstr());
    return true;
  }
}

static void usage() {
  fprintf( stderr,
           "Usage:sqlbind [-v] -cdatabase : Create database\n"
           "      sqlbind [-v] -ddatabase : Drop database\n"
           "      sqlbind [-v] -bdatabase files : Bind files to database\n"
           "      sqlbind [-v] -udatabase files : Unbind programfiles (use extension .sqc)\n"
         );
  exit(-1);
}

typedef enum {
  CREATEDB,
  DROPDB  ,
  BINDPROGRAM,
  UNBINDPROGRAM
} command;

int _tmain( int argc, TCHAR **argv ) {
  TCHAR *cp;
  TCHAR *dbname = NULL;
  command cmd;

  argvExpand(argc, argv);

  for( argv++; *argv && *(cp = *argv) == '-'; argv++ ) {
    for( cp++; *cp; cp++ ) {
      switch( *cp ) {
        case 'c':
          cmd = CREATEDB;
          dbname = cp + 1;
          break;
        case 'd':
          cmd = DROPDB;
          dbname = cp + 1;
          break;
        case 'b':
          cmd = BINDPROGRAM;
          dbname = cp + 1;
          break;
        case 'u':
          cmd = UNBINDPROGRAM;
          dbname = cp + 1;
          break;
        case 'v':
          verbose = true;
          break;
        default :
          usage();
      }
      break;
    }
  }

  if(dbname == NULL || _tcslen(dbname) == 0) usage();
  try {
    switch(cmd) {
    case BINDPROGRAM:
      if( !*argv ) usage();
      return bindProgram(dbname,argv) ? 0 : -1;
      break;
    case UNBINDPROGRAM:
      if(!*argv ) usage();
      return unbindProgram(dbname,argv) ? 0 : -1;
      break;
    case CREATEDB:
      return createDatabase(dbname) ? 0 : -1;
      break;
    case DROPDB:
      return dropDatabase(dbname) ? 0 : -1;
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
