#include "stdafx.h"
#include <SocketStream.h>
#include <Thread.h>

#define DEBUG_SERVER

class ClientCursor {
private:
  DbCursor              *m_dbcursor;
  VirtualCode            m_vc;
  HostVarList            m_hostvar;
  HostVarDescriptionList m_desc;
public:
  SqlApiBindProgramId    m_programid;
  int                    m_apiopt;
  HostVarList            m_fetchvarlist;
  ClientCursor(Database *db, const SqlApiBindProgramId &programid, int apiopt, HostVarList &hostvar);
  bool fetch();
  ~ClientCursor();
  void getSqlca(sqlca &sqlca);
};

void ClientCursor::getSqlca(sqlca &sqlca) {
  sqlca = m_dbcursor->m_sqlca;
}

ClientCursor::ClientCursor(Database *db, const SqlApiBindProgramId &programid, int apiopt, HostVarList &hostvar) {
  m_programid = programid;
  m_apiopt    = apiopt;
  m_hostvar   = hostvar;
  sqlLoadCode(*db,
               m_programid,m_apiopt,
               m_vc
             );
//  m_vc.dump();
  m_dbcursor = new DbCursor(*db,m_vc,m_hostvar);
  m_desc = m_dbcursor->getDescription();
}

bool ClientCursor::fetch() {
  m_fetchvarlist.describe(m_desc);
  if(!m_dbcursor->fetch(m_fetchvarlist)) return false;
  return true;
}

ClientCursor::~ClientCursor() {
  delete m_dbcursor;
}

class ClientConnection {
  int findClientCursor(const SqlApiBindProgramId &programid, int apiopt);
public:
  Database      *m_db;
  BufferedSocket m_p;
  SqlApiCom      m_combuffer;
  ClientConnection(SOCKET s);
  ~ClientConnection();
  Array<ClientCursor*> m_opencursor;
  void closeDatabase();
  bool readRequest();
  void writeReply();
  void execute();
  void executeConnect();
  void executeCreateDb();
  void executeDropDb();
  void executeBind();
  void executeUnbind();
  void executeOpenCursor();
  void executeFetchCursor();
  void executeCloseCursor();
};

ClientConnection::ClientConnection(SOCKET s) : m_p(s) {
  m_db = NULL;
}

ClientConnection::~ClientConnection() {
  closeDatabase();
}

void ClientConnection::closeDatabase() {
  if(m_db != NULL) { 
    while(m_opencursor.size() > 0) {
      delete m_opencursor[0];
      m_opencursor.remove(0);
    }
    delete m_db; 
    m_db = NULL;
  }
}

bool ClientConnection::readRequest() {
  try {
    if(!m_p.receive()) return false;
    m_p >> m_combuffer;
    return true;
  } catch(Exception e) {
    return false;
  }
}

void ClientConnection::writeReply() {
  m_p << m_combuffer;
  m_p.send();
}

void ClientConnection::executeCreateDb() {
  SqlApiCreateDb crdb;
  SysTableDbData dbdef;
  int i;

  m_p >> crdb;
  _stprintf(dbdef.m_path,_T("%c:\\%s"),crdb.drive,crdb.dbname);
  _tcscpy(dbdef.m_dbName,crdb.dbname);
  for(i = 0; i < 256; i++ ) dbdef.m_colseq[i] = crdb.colseq[i];
  try {
    Database::create( dbdef );
    m_combuffer.m_ca.seterror(0);
  } catch(sqlca ca) {
    m_combuffer.m_ca = ca;
  }
}

void ClientConnection::executeDropDb() {
  char dbname[100];

  m_p >> dbname;
  try {
    Database::destroy(dbname);
    m_combuffer.m_ca.seterror(0);
  } catch(sqlca ca) {
    m_combuffer.m_ca = ca;
  }
}

void ClientConnection::executeConnect() {
  SysTableUserData userrec;
  TCHAR dbname[100];
  TCHAR username[100];
  TCHAR password[100];

  if( m_combuffer.m_apiopt == SQL_CALL_CONNECT_USING)
    m_p >> dbname >> username >> password;
  else
    m_p >> dbname;
#ifdef DEBUG_SERVER
  _tprintf(_T("Now connecting to <%s> user <%s> password <%s>\n")
           ,dbname,username,password);
#endif
  try {
    m_db = new Database(dbname);

    m_db->sysTabReadUser(username, userrec);

    if(_tcscmp(userrec.m_password,password)) {
      closeDatabase();
      throwSqlError(SQL_INVALID_PASSWORD,_T("Invalid password"));
    }
  } catch(sqlca ca) {
    m_combuffer.m_ca = ca;
  }
}

void ClientConnection::executeBind() {
  SqlApiBindStmt bndstmt;

  m_p >> bndstmt;

#ifdef DEBUG_SERVER
  _tprintf(_T("Received bind request.\n<%-40.40s>\n"),bndstmt.m_stmt);
#endif
  StringArray errmsg;
  try {
    VirtualCode vc;
    if(sqlCompile( *m_db, m_combuffer.m_programid, bndstmt, vc, errmsg, m_combuffer.m_ca )) {
      if(vc.totalSize() > 0) {
        m_db->trbegin();
        sqlSaveCode( *m_db, m_combuffer.m_programid, bndstmt.m_stmtHead.m_nr, vc);
        m_db->trcommit();
      }
#ifdef DEBUG_SERVER
      vc.dump();
#endif
    }
  } catch(sqlca ca) {
    if(m_db->inTMF()) m_db->trabort();
    m_combuffer.m_ca = ca;
  } catch(Exception e) {
//    _tprintf(_T("caught Exception:%s"),e.what());
    errmsg.add(e.what());
    m_combuffer.m_ca.seterror(SQL_FATAL_ERROR,_T("%s"),e.what());
  }

  m_p << m_combuffer
      << errmsg;
  m_p.send();
}

void ClientConnection::executeUnbind() {
  try {
    m_db->trbegin();
    m_db->sysTabCodeDelete(m_combuffer.m_programid.fileName);
    m_db->trcommit();
  } catch(sqlca ca) {
    if(m_db->inTMF()) m_db->trabort();
    m_combuffer.m_ca = ca;
  }
}

int ClientConnection::findClientCursor(const SqlApiBindProgramId &programid, int apiopt) {
  for(UINT i = 0; i < m_opencursor.size(); i++) {
    ClientCursor *cursor = m_opencursor[i];
    if(cursor->m_programid == programid && cursor->m_apiopt == apiopt)
      return i;
  }
  return -1;
}

void ClientConnection::executeOpenCursor() {
  if(findClientCursor(m_combuffer.m_programid,m_combuffer.m_apiopt) >= 0) {
    m_combuffer.m_ca.seterror(SQL_CURSOR_ALREADY_OPEN,_T("Cursor already open"));
    m_p.clear();
  }
  else {
    HostVarList hostvar;
    m_p >> hostvar;
    try {
      ClientCursor *cursor = new ClientCursor(m_db, m_combuffer.m_programid, m_combuffer.m_apiopt, hostvar);
      m_opencursor.add(cursor);
    } catch(sqlca ca) {
      m_combuffer.m_ca = ca;
    }
  }
  writeReply();
}

void ClientConnection::executeCloseCursor() {
  int c = findClientCursor(m_combuffer.m_programid,m_combuffer.m_apiopt);
  if(c < 0)
    m_combuffer.m_ca.seterror(SQL_CURSOR_NOT_OPEN,_T("Cursor not open"));
  else {
    delete m_opencursor[c];
    m_opencursor.removeIndex(c);
  }
  writeReply();
}

void ClientConnection::executeFetchCursor() {
  int c = findClientCursor(m_combuffer.m_programid,m_combuffer.m_apiopt);
  if(c < 0) {
    m_combuffer.m_ca.seterror(SQL_CURSOR_NOT_OPEN,_T("Cursor not open"));
    m_p << m_combuffer;
  }
  else {
    if(m_opencursor[c]->fetch()) {
      m_p << m_combuffer;
      m_p << m_opencursor[c]->m_fetchvarlist;
    }
    else {
      m_opencursor[c]->getSqlca(m_combuffer.m_ca);
      m_p << m_combuffer;
    }
  }
  m_p.send();
}

void ClientConnection::execute() {
  VirtualCode vc;

  HostVarList hostvar;
  m_p >> hostvar;

  try {
    sqlLoadCode(*m_db
                ,m_combuffer.m_programid,m_combuffer.m_apiopt
                ,vc
               );
    sqlExecute(*m_db
               ,vc
               ,hostvar
               ,m_combuffer.m_ca
               );
  } catch(sqlca ca) {
    m_combuffer.m_ca = ca;
  }
}

class ServerThread : public Thread {
  SOCKET m_socket;
public:
  ServerThread(SOCKET socket);
  UINT run();
  ~ServerThread();
};

ServerThread::ServerThread(SOCKET socket) {
  m_socket = socket;
#ifdef DEBUG_SERVER
  _tprintf(_T("threadid:%d\n"),getThreadId());
  resume();
#endif
}

ServerThread::~ServerThread() {
}

UINT ServerThread::run() {
  ClientConnection cc(m_socket);

#ifdef DEBUG_SERVER
  _tprintf(_T("ServerThread communiate on socket %s\n"), toString((INT64)m_socket).cstr());
#endif
  try {
    while(cc.readRequest()) {
      switch(cc.m_combuffer.m_apicall) {
        case SQL_CALL_CONNECT   :
          switch(cc.m_combuffer.m_apiopt) {
            case SQL_CALL_CONNECT_RESET:
              cc.closeDatabase();
              cc.writeReply();
              break;
            case SQL_CALL_CONNECT_DB:
            case SQL_CALL_CONNECT_USING:
              cc.executeConnect();
              cc.writeReply();
              break;
          }
          break;
        case SQL_CALL_CREATE    :
          cc.executeCreateDb();
          cc.writeReply();
          break;
        case SQL_CALL_DROP      :
          cc.executeDropDb();
          cc.writeReply();
          break;
        case SQL_CALL_COMMIT    :
        case SQL_CALL_IMMEDIATE :
        case SQL_CALL_ROLLBACK  :
        case SQL_CALL_PREPARE   :
        case SQL_CALL_DESCRIBE  :
          cc.writeReply();
          break;

        case SQL_CALL_OPEN      :
          cc.executeOpenCursor();
          break;
        case SQL_CALL_CLOSE     :
          cc.executeCloseCursor();
          break;
        case SQL_CALL_FETCH     :
          cc.executeFetchCursor();
          break;
        case SQL_CALL_EXECUTE   :
#ifdef DEBUG_SERVER
          _tprintf(_T("execute request: programid <%s,%s> apiopt:%d\n"),
            cc.m_combuffer.m_programid.fileName,
            cc.m_combuffer.m_programid.Timestamp,
            cc.m_combuffer.m_apiopt);
          _tprintf(_T("Now loading statement\n"));
#endif
          cc.execute();
          cc.writeReply();
          break;
      
       case SQL_CALL_BIND       :
         cc.executeBind();
         break;  

       case SQL_CALL_UNBIND     :
         cc.executeUnbind();
         cc.writeReply();
         break;

       default:
         cc.m_combuffer.m_ca.sqlcode = SQL_ERROR_INVALID_APICALL;
         cc.writeReply();
         break;
      }
    }
  } catch(sqlca ca) {
    cc.m_combuffer.m_ca = ca;
    cc.writeReply();
  } catch(...) {
    cc.m_combuffer.m_ca.seterror(SQL_FATAL_ERROR,_T("Uncaught Exception in sqlserver"));
    cc.writeReply();
  }
#ifdef DEBUG_SERVER
  _tprintf(_T("ServerThread termination\n"));
#endif
  tcpClose(m_socket);
  return 0;
}

static unsigned short getPort() {
  TCHAR *envvar = _T("SQLSERVER");
  TCHAR *envval = _tgetenv(envvar);
  TCHAR *sm;
  unsigned short portnr;

  if(envval == NULL)
    throwException(_T("%s not defined"),envvar);
  
  if(((sm = _tcschr(envval,';')) == NULL) || _stscanf(sm+1,_T("%hd"),&portnr) != 1)
    throwException(_T("%s must contain servername and portnumber"),envvar);
  return portnr;
}

int main() {
  try {
    SOCKET listen = tcpCreate(getPort());

    Array<ServerThread*> threads;
#ifdef DEBUG_SERVER
    _tprintf(_T("Server is running. waiting for connections...\n"));
#endif
    for(;;) {
      SOCKET s = tcpAccept(listen);
#ifdef DEBUG_SERVER
      _tprintf(_T("Got a connection. Now create a serverprocess\n"));
#endif
      threads.add(new ServerThread(s));
      for(UINT i = 0; i < threads.size(); i++) {
        ServerThread *thr = threads[i];
        if(!thr->stillActive()) {
#ifdef DEBUG_SERVER
          _tprintf(_T("Removing Thread [%d]\n"),i); // her leakes et handle for hver gang !!!
#endif
          threads.removeIndex(i);
          delete thr;
        }
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"),e.what());
    exit(-1);
  }
  return 0;
}
