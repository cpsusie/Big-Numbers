#include "stdafx.h"
#include "SocketStream.h"
#include "sqlcom.h"

void SqlApiCom::dump(FILE *f) {
  m_ca.dump(f);
}

SqlApiCom::SqlApiCom(SqlApiCallCode             apicall  ,
                     int                        apiopt
                    ) {
  m_apicall   = apicall;
  m_apiopt    = apiopt;
}

SqlApiCom::SqlApiCom(SqlApiCallCode             apicall  ,
                     int                        apiopt   ,
                     const SqlApiBindProgramId &programid
                    ) {
  m_apicall   = apicall;
  m_apiopt    = apiopt;
  m_programid = programid;
}

SqlApiCom::SqlApiCom(SqlApiCallCode             apicall  ,
                     int                        apiopt   ,
                     const SqlApiBindProgramId &programid,
                     const sqlca               &ca       ) {
  m_apicall   = apicall;
  m_apiopt    = apiopt;
  m_programid = programid;
  m_ca        = ca;
}

static BufferedSocket *sqlpipe = NULL;

static bool getserverport(TCHAR *servername, unsigned int &portnr) {
  TCHAR *s = _tgetenv(_T("SQLSERVER"));

  if(s == NULL) return false;
  _tcscpy(servername, s);
  if(((s = _tcschr(servername,';')) != NULL) && _stscanf(s+1,_T("%d"),&portnr) == 1) {
    *s = '\0';
    return true;
  } else {
    return false;
  }
}

static void closeconnection() {
  delete sqlpipe;
  sqlpipe = NULL;
}

static void openconnection() {
  TCHAR        servername[256];
  unsigned int portnr;

  if(sqlpipe != NULL) {
    throwSqlError(SQL_ERROR_ALREADY_CONNECTED, _T("Application already connected"));
  }
  if(!getserverport(servername,portnr)) {
    throwSqlError(SQL_ERROR_NO_SERVER, _T("No dbserver available"));
  }
  sqlpipe = new BufferedSocket(portnr, servername);
}

void sqlapi_bindda( struct sqlda *sqlda) {
}

static void copyvar(SqlApiVarList &dst, const SqlApiVarList &src) {
  if(dst.sqltype != src.sqltype) {
    throwSqlError(SQL_INVALID_HOSTVAR_TYPE, _T("Invalid type of hostvar"));
  }

  int len = dst.sqllen < src.sqllen ? dst.sqllen : src.sqllen;
  memcpy(dst.sqldata,src.sqldata,len);
  if(dst.sqlind != NULL && src.sqlind != NULL) {
    memcpy(dst.sqlind, src.sqlind, sizeof(*src.sqlind));
  } else if(src.sqlind && *src.sqlind == INDICATOR_UNDEFINED && dst.sqlind == NULL) {
    throwSqlError(SQL_NOINDICATOR,_T("Unable to return nullvalue in hostvar because no indicator is specified"));
  }
}

static void copyinput(int            in,
                      SqlApiVarList *varlist) {
  (*sqlpipe) << in;
  for(int i = 0; i < in;i++) (*sqlpipe) << varlist[i];
}

static void copyoutput(int            in,
                       int            out,
                       SqlApiVarList *varlist
                      ) {
  for(int i = 0; i < out; i++) (*sqlpipe) >> varlist[in+i];
}

static void copyoutput(int            out,
                       SqlApiVarList *varlist
                      ) {
  HostVarList hv;
  (*sqlpipe) >> hv;
  if((int)hv.size() != out) {
    throwSqlError(SQL_INVALID_NO_OF_COLUMNS, _T("Invalid number of columns specified in HostVarList (=%d) expected %d"),
                  out, hv.size());
  }
  for(int i = 0; i < out; i++) {
    copyvar(varlist[i],hv[i]);
  }
}

static void connect(const SqlApiBindProgramId &programid,
                    int                        opt         ,
                    int                        in          ,
                    SqlApiVarList             *varlist     ,
                    sqlca                     &cca
                   ) {
  switch(opt) {
    case SQL_CALL_CONNECT_RESET:
      if(sqlpipe == NULL) {
        throwSqlError(SQL_ERROR_NO_CONNECTION,_T("No Database-connection"));
        return;
      }
      closeconnection();
      break;

    case SQL_CALL_CONNECT_DB:
      try {
        openconnection();
        (*sqlpipe) << SqlApiCom(SQL_CALL_CONNECT,SQL_CALL_CONNECT_DB,programid);
        (*sqlpipe) << (char*)varlist[0].sqldata; // dbname
        sqlpipe->send();
        sqlpipe->receive();
        SqlApiCom com;
        (*sqlpipe) >> com;
        cca = com.m_ca;
      } catch(sqlca) {
        closeconnection();
        throw;
      };
      break;

    case SQL_CALL_CONNECT_USING:
      try {
        openconnection();
        (*sqlpipe) << SqlApiCom(SQL_CALL_CONNECT,SQL_CALL_CONNECT_USING,programid);
        (*sqlpipe) << (char*)varlist[0].sqldata  // dbname
                   << (char*)varlist[1].sqldata  // username
                   << (char*)varlist[2].sqldata; // password
        sqlpipe->send();
        sqlpipe->receive();
        SqlApiCom com;
        (*sqlpipe) >> com;
        cca = com.m_ca;
      } catch(sqlca) {
        closeconnection();
        throw;
      }
      break;

    default:
      throwSqlError(SQL_ERROR_INVALID_APICALL,_T("Invalid connectoption (%s,%d)"),programid.fileName,opt);
      break;
  }
}

static void execute(const SqlApiBindProgramId &programid,
                    int                        opt         ,
                    int                        in          ,
                    int                        out         ,
                    SqlApiVarList             *varlist     ,
                    sqlca                     &cca
                   ) {
  _tprintf(_T("apicall EXECUTE.\n"));

  (*sqlpipe) << SqlApiCom(SQL_CALL_EXECUTE, opt,programid);
  copyinput(in,varlist);
  sqlpipe->send();
  sqlpipe->receive();
  SqlApiCom com;
  (*sqlpipe) >> com;
  cca = com.m_ca;
  copyoutput(in,out,varlist);
}


static void execute_open(const SqlApiBindProgramId &programid,
                         int                        opt      ,
                         int                        in       ,
                         SqlApiVarList             *varlist  ,
                         sqlca                     &cca) {
  (*sqlpipe) << SqlApiCom(SQL_CALL_OPEN, opt, programid);
  copyinput(in,varlist);
  sqlpipe->send();
  sqlpipe->receive();
  SqlApiCom com;
  (*sqlpipe) >> com;
  cca = com.m_ca;
}

static void execute_close(const SqlApiBindProgramId &programid,
                          int                        opt      ,
                          sqlca                     &cca) {
  (*sqlpipe) << SqlApiCom(SQL_CALL_CLOSE, opt, programid);
  sqlpipe->send();
  sqlpipe->receive();
  SqlApiCom com;
  (*sqlpipe) >> com;
  cca = com.m_ca;
}

static void execute_fetch(const SqlApiBindProgramId &programid   ,
                          int                        opt         ,
                          int                        out         ,
                          SqlApiVarList             *varlist     ,
                          sqlca                     &cca) {
  (*sqlpipe) << SqlApiCom(SQL_CALL_FETCH, opt, programid);
  sqlpipe->send();
  sqlpipe->receive();
  SqlApiCom com;
  (*sqlpipe) >> com;
  cca = com.m_ca;
  if(cca.sqlcode == SQL_OK) copyoutput(out,varlist);
}

void sqlapi_call(int                    call     , // SqlApiCallCode
                 int                    opt      ,
                 void                  *programid,
                 int                    in       ,
                 int                    out      ,
                 SqlApiVarList         *varlist  ,
                 sqlca                 &cca
                ) {
  cca.init();
  if(call != SQL_CALL_CONNECT) {
    if(sqlpipe == NULL) {
      cca.seterror(SQL_ERROR_NO_CONNECTION, _T("No Databaseconnection"));
      return;
    }
  }

  SqlApiBindProgramId bndprogramid(programid);

  // vi ved at sqlpipe er åben

  try {
    switch(call) {
    case SQL_CALL_COMMIT    :
      break;
    case SQL_CALL_CONNECT   :
      connect(bndprogramid,opt, in, varlist, cca);
      break;
    case SQL_CALL_IMMEDIATE :
    case SQL_CALL_ROLLBACK  :
    case SQL_CALL_PREPARE   :
    case SQL_CALL_DESCRIBE  :
      _tprintf(_T("apicall %d er ikke implementeret endnu\n"),call);
      break;
    case SQL_CALL_OPEN      :
      execute_open(bndprogramid, opt, in, varlist,cca);
      break;

    case SQL_CALL_CLOSE     :
      execute_close(bndprogramid, opt, cca);
      break;
    case SQL_CALL_FETCH     :
      execute_fetch(bndprogramid, opt, out, varlist, cca);
      break;

    case SQL_CALL_EXECUTE   :
      execute(bndprogramid, opt, in, out, varlist,cca);
      break;

    default:
      throwSqlError(SQL_ERROR_INVALID_APICALL, _T("Invalid apicall (%s,%d,%d)"), bndprogramid. fileName, call, opt);
      break;
    }
  } catch(sqlca ca) {
    cca = ca;
  }
}

void sqlapi_bind(    const SqlApiBindProgramId   &programid,
                     const SqlApiBindStmt        &bndstmt  ,
                     StringArray                 &errmsg,
                     sqlca                       &cca ) {

  cca.init();

  try {
    (*sqlpipe) << SqlApiCom(SQL_CALL_BIND,0,programid);
    (*sqlpipe) << bndstmt;

    sqlpipe->send();
    sqlpipe->receive();

    SqlApiCom com;
    (*sqlpipe) >> com;
    cca = com.m_ca;
    (*(Packer*)sqlpipe) >> errmsg;
  } catch(sqlca ca) {
    cca = ca;
  }
}

void sqlapi_unbind(   const TCHAR                *filename ,
                      sqlca                       &cca
                      ) {
  cca.init();

  try {
    SqlApiBindProgramId programid;
    memset(&programid,0,sizeof(programid));
    _tcsncpy(programid.fileName,filename,ARRAYSIZE(programid.fileName)-1);
    (*sqlpipe) << SqlApiCom(SQL_CALL_UNBIND,0,programid);

    sqlpipe->send();
    sqlpipe->receive();

    SqlApiCom com;
    (*sqlpipe) >> com;
    cca = com.m_ca;
  } catch(sqlca ca) {
    cca = ca;
  }
}

void sqlapi_createdb( const SqlApiCreateDb &crdb,
                      sqlca                &cca) {

  if(sqlpipe != NULL) {
    cca.seterror(SQL_ERROR_ALREADY_CONNECTED, _T("Can't create Database when a connection is open"));
    return;
  }

  try {
    openconnection();
    (*sqlpipe) << SqlApiCom(SQL_CALL_CREATE,0);
    (*sqlpipe) << crdb;
    sqlpipe->send();
    sqlpipe->receive();
    SqlApiCom com;
    (*sqlpipe) >> com;
    cca = com.m_ca;
    closeconnection();
  } catch(sqlca ca) {
    cca = ca;
  }
}

void sqlapi_dropdb(   const TCHAR                 *dbname,
                      sqlca                       &cca) {

  if(sqlpipe != NULL) {
    cca.seterror(SQL_ERROR_ALREADY_CONNECTED, _T("Can't drop Database when a connection is open"));
    return;
  }

  try {
    openconnection();
    (*sqlpipe) << SqlApiCom(SQL_CALL_DROP,0);
    (*sqlpipe) << dbname;
    sqlpipe->send();
    sqlpipe->receive();
    SqlApiCom com;
    (*sqlpipe) >> com;
    cca = com.m_ca;
    closeconnection();
  } catch(sqlca ca) {
    cca = ca;
  }
}
