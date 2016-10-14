#include "stdafx.h"
#include <io.h>
#include <process.h>
#include <ByteMemoryStream.h>
#include <Random.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

class PositionInfoRequestParam {
public:
  GameKey m_gameKey;
  bool    m_swapPlayers;
  PositionInfoRequestParam(SOCKET s) {
    tcpRead(s, this, sizeof(PositionInfoRequestParam));
  }
  PositionInfoRequestParam(const GameKey &gameKey, bool swapPlayers)
  : m_gameKey(gameKey)
  , m_swapPlayers(swapPlayers)
  {
  }
};

class RequestPositionInfo {
private:
  const RemoteTablebaseRequest   m_request;
  const PositionInfoRequestParam m_param;
public:
  RequestPositionInfo(RemoteTablebaseRequest request, const GameKey &gameKey, bool swapPlayers) 
  : m_request(request)
  , m_param(gameKey, swapPlayers)
  {
  }
  void write(SOCKET s) {
    tcpWrite(s, this, sizeof(RequestPositionInfo));
  }
};

class SetGlobalsRequestParam {
public:
  String          m_dbPath;
  TablebaseMetric m_metric;
  SetGlobalsRequestParam(SOCKET s) {
    read(s);
  }

  SetGlobalsRequestParam(const String &dbPath, TablebaseMetric metric) : m_dbPath(dbPath), m_metric(metric) {
  }
  void write(SOCKET s) const;
  void read( SOCKET s);
};

void SetGlobalsRequestParam::write(SOCKET s) const {
  tcpWriteString(s, m_dbPath);
  tcpWrite(s, &m_metric, sizeof(m_metric));
}

void SetGlobalsRequestParam::read(SOCKET s) {
  m_dbPath = tcpReadString(s);
  tcpRead(s, &m_metric, sizeof(m_metric));
}

class RequestSetGlobals {
private:
  const RemoteTablebaseRequest m_request;
  const SetGlobalsRequestParam m_param;
public:
  RequestSetGlobals(const String &dbPath, TablebaseMetric metric) 
    : m_request(REQUEST_SETGLOBALS)
    , m_param(dbPath, metric) {
  }
  void write(SOCKET s) const;
};

void RequestSetGlobals::write(SOCKET s) const {
  tcpWrite(s, &m_request, sizeof(m_request));
  m_param.write(s);
}

class RemoteTablebaseLoadReply {
public:
  RemoteTablebaseReply m_replyCode;
  String               m_msg; // fileName if LOAD_OK, errorMessage if LOAD_FAILED
  RemoteTablebaseLoadReply(SOCKET s) {
    read(s);
  }
  RemoteTablebaseLoadReply(RemoteTablebaseReply replyCode, const String &msg);
  void write(SOCKET s) const;
  void read( SOCKET s);
};

RemoteTablebaseLoadReply::RemoteTablebaseLoadReply(RemoteTablebaseReply replyCode, const String &msg) {
  m_replyCode = replyCode;
  m_msg       = msg;
}

void RemoteTablebaseLoadReply::write(SOCKET s) const {
  tcpWrite(s, &m_replyCode, sizeof(m_replyCode));
  tcpWriteString(s, m_msg);
}

void RemoteTablebaseLoadReply::read( SOCKET s) {
  tcpRead(s, &m_replyCode, sizeof(m_replyCode));
  m_msg = tcpReadString(s);
}


// --------------------------------------------------------------------------------------------------------

SOCKET RemoteEndGameSubTablebase::listenSocket = INVALID_SOCKET;

RemoteEndGameSubTablebase::RemoteEndGameSubTablebase(const EndGameKeyDefinition &keydef) : EndGameSubTablebase(keydef) {
  m_socket = INVALID_SOCKET;
  m_loaded = false;
}

String RemoteEndGameSubTablebase::load(ByteCounter *byteCounter) {
  return load(false);
}

#ifdef TABLEBASE_BUILDER
String RemoteEndGameSubTablebase::loadPacked() {
  return load(true);
}
#endif

String RemoteEndGameSubTablebase::load(bool packed) {
  if(m_socket != INVALID_SOCKET) {
    return _T("");
  }

  static int portNumber = -1;

  if(listenSocket == INVALID_SOCKET) {
    if(portNumber < 0) {
      randomize();
      portNumber = randInt() % 10000 + 1000;
    }
    listenSocket = tcpCreate(portNumber);
  }
  const String programName = getModuleFileName();
  if(_tspawnlp(_P_NOWAITO, programName.cstr(), programName.cstr()
              ,_T("-R")
              , m_keydef.toString().cstr()
              ,format(_T("%d"), portNumber).cstr()
              ,NULL) == -1) {
    throwErrNoOnSysCallException(_T("_tspawnlp"));
  }
  m_socket = tcpAccept(listenSocket);
  sendRequest(REQUEST_SETGLOBALS);

  log(_T("\nLoading %s..."), getName().cstr());
  sendRequest(packed ? REQUEST_LOADPACKED : REQUEST_LOAD);

  RemoteTablebaseLoadReply reply(m_socket);
  if(reply.m_replyCode != LOAD_OK) {
    throwException(_T("Load failed:%s"), reply.m_msg.cstr());
  } else {
    log(_T("All loaded from %s\n"), reply.m_msg.cstr());
  }

  m_loaded = true;
  return reply.m_msg;
}

EndGamePositionStatus RemoteEndGameSubTablebase::getPositionStatus(const Game &game, bool swapPlayers) const {
  RequestPositionInfo(REQUEST_POSITIONSTATUS, game.getKey(), swapPlayers).write(m_socket);

  EndGamePositionStatus ps;
  tcpRead(m_socket, &ps, sizeof(ps));
  return ps;
}

EndGameResult RemoteEndGameSubTablebase::getPositionResult(const Game &game, bool swapPlayers) const {
  RequestPositionInfo(REQUEST_POSITIONRESULT, game.getKey(), swapPlayers).write(m_socket);

  EndGameResult pr;
  tcpRead(m_socket, &pr, sizeof(pr));
  return pr;
}

void RemoteEndGameSubTablebase::unload() {
  if(m_socket != INVALID_SOCKET) {
    sendRequest(REQUEST_QUIT);
  }
  m_loaded = false;
}

#ifdef TABLEBASE_BUILDER

bool RemoteEndGameSubTablebase::allKeysFound() const {
  const RemoteTablebaseRequest requestCode = REQUEST_ALLKEYSFOUND;
  tcpWrite(m_socket, &requestCode, sizeof(requestCode));
  bool result;

  tcpRead(m_socket, &result, sizeof(result));
  return result;
}

#endif

void RemoteEndGameSubTablebase::sendRequest(RemoteTablebaseRequest requestCode) {
  switch(requestCode) {
  case REQUEST_SETGLOBALS:
    RequestSetGlobals(EndGameKeyDefinition::getDbPath(), EndGameKeyDefinition::getMetric()).write(m_socket);
    break;

  case REQUEST_LOAD      :
  case REQUEST_LOADPACKED:
    tcpWrite(m_socket, &requestCode, sizeof(requestCode));
    break;

  case REQUEST_QUIT :
    tcpWrite(m_socket, &requestCode, sizeof(requestCode));
    tcpClose(m_socket);
    m_socket = INVALID_SOCKET;
    break;
  }
}

// Server-side
RemoteTablebaseRequest RemoteEndGameSubTablebase::getRequestCode(SOCKET s) {
  RemoteTablebaseRequest request;
  tcpRead(s, &request, sizeof(request));
  return request;
}

void RemoteEndGameSubTablebase::remoteService(TCHAR **argv) { // static.
  DEFINEMETHODNAME;
  if(_tcscmp(*argv, _T("-R")) != 0) {
    throwInvalidArgumentException(method, _T("argv[0]=%s, Must be -s"), *argv);
  }
  argv++;
  String tablebaseName;
  int    portNumber;

  if(!*argv) {
    throwInvalidArgumentException(method, _T("argv[1]=NULL, Must be tablename"));
  } else {
    tablebaseName = *(argv++);
  }
  if(!*argv) {
    throwInvalidArgumentException(method, _T("argv[2]=NULL, Must be portnumber"));
  } else {
    portNumber = _tstoi(*argv);
  }
  SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

  EndGameTablebase &tablebase = EndGameTablebase::getInstanceByName(tablebaseName);
  SOCKET connection = tcpOpen(portNumber);
  Game workGame;
  for(;;) {
    const RemoteTablebaseRequest request = getRequestCode(connection);
    switch(request) {
    case REQUEST_SETGLOBALS       :
      { SetGlobalsRequestParam param(connection);
        EndGameKeyDefinition::setDbPath(param.m_dbPath);
        EndGameKeyDefinition::setMetric(param.m_metric);
      }
      break;

    case REQUEST_LOAD             :
      { try {
          const String fileName = tablebase.load();
          RemoteTablebaseLoadReply(LOAD_OK, fileName).write(connection);
        } catch(Exception e) {
          RemoteTablebaseLoadReply(LOAD_FAILED, e.what()).write(connection);
        }
      }
      break;
#ifdef TABLEBASE_BUILDER
    case REQUEST_LOADPACKED       :
      { try {
          const String fileName = tablebase.loadPacked();
          RemoteTablebaseLoadReply(LOAD_OK, fileName).write(connection);
        } catch(Exception e) {
          RemoteTablebaseLoadReply(LOAD_FAILED, e.what()).write(connection);
        }
      }
      break;
    case REQUEST_ALLKEYSFOUND     :
      { const bool result = tablebase.allKeysFound();
        tcpWrite(connection, &result, sizeof(result));
      }
      break;
#endif
    case REQUEST_POSITIONSTATUS   :
      { PositionInfoRequestParam param(connection);
        workGame = param.m_gameKey;
        const EndGamePositionStatus state = tablebase.getPositionStatus(workGame, param.m_swapPlayers);
        tcpWrite(connection, &state, sizeof(state));
      }
      break;

    case REQUEST_POSITIONRESULT   :
      { PositionInfoRequestParam param(connection);
        workGame = param.m_gameKey;
        const EndGameResult result = tablebase.getPositionResult(workGame, param.m_swapPlayers);
        tcpWrite(connection, &result, sizeof(result));
      }
      break;

    case REQUEST_QUIT             :
      tablebase.unload();
      tcpClose(connection);
      return;

    default                   :
      throwException(_T("Invalid command:%d"), request);
    }
  }
}
