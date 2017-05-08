#include "stdafx.h"
#include "AbstractMoveFinder.h"

//#define TEST_REFCOUNT

ChessPlayerRequest::ChessPlayerRequest(const Game &game, const TimeLimit &timeLimit, bool hint, bool verbose)
:m_type(REQUEST_FINDMOVE)
{
  m_data.m_findMoveParam = new FindMoveRequestParam(game, timeLimit, hint, verbose);
}

ChessPlayerRequest::ChessPlayerRequest(const Game &game)
:m_type(REQUEST_GAMECHANGED)
{
  m_data.m_gameChangedParam = new GameChangedRequestParam(game);
}

ChessPlayerRequest::ChessPlayerRequest(const MoveBase &move, bool hint)
:m_type(REQUEST_FETCHMOVE)
{
  m_data.m_fetchMoveParam.m_move = move;
  m_data.m_fetchMoveParam.m_hint = hint;
}

ChessPlayerRequest::ChessPlayerRequest(const String &msgText, bool error)
:m_type(REQUEST_SHOWMESSAGE)
{
  m_data.m_showMessageParam = new ShowMessageRequestParam(msgText, error);
}

  // REQUEST_CONNECT
ChessPlayerRequest::ChessPlayerRequest(const SocketChannel &channel)
:m_type(REQUEST_CONNECT)
{
  m_data.m_connectParam = new ConnectRequestParam(channel);
}

ChessPlayerRequest::ChessPlayerRequest(ChessPlayerRequestType type) {
  switch (type) {
  case REQUEST_NULLMOVE     :
  case REQUEST_STOPSEARCH   :
  case REQUEST_MOVENOW      :
  case REQUEST_RESET        :
  case REQUEST_DISCONNECT   :
  case REQUEST_KILL         :
    m_type = type;
    cleanData();
    break;
  default                   :
    throwInvalidArgumentException(__TFUNCTION__,_T("type=%s"), getRequestName(type));
  }
}

ChessPlayerRequest::ChessPlayerRequest(const ChessPlayerRequest &src) 
: m_type(src.getType())
, m_data(src.m_data)
{
  addref();
}

ChessPlayerRequest::~ChessPlayerRequest() {
  release();
}

void ChessPlayerRequest::addref() {
  int refCount = 0;
  switch(m_type) {
  case REQUEST_FINDMOVE   :
    refCount = m_data.m_findMoveParam->addref();
    break;
  case REQUEST_GAMECHANGED:
    refCount = m_data.m_gameChangedParam->addref();
    break;
  case REQUEST_SHOWMESSAGE:
    refCount = m_data.m_showMessageParam->addref();
    break;
  case REQUEST_CONNECT    :
    refCount = m_data.m_connectParam->addref();
    break;

  default                 :
    break; // do nothing
  }
#ifdef TEST_REFCOUNT
  if (refCount > 0) {
    verbose(_T("request(%s).addRef(). refCount=%d\n"), toString().cstr(), refCount);
  }
#endif
}

void ChessPlayerRequest::release() {
  int refCount = 500;
  switch(m_type) {
  case REQUEST_FINDMOVE   :
    if((refCount=m_data.m_findMoveParam->release()) == 0) {
      delete m_data.m_findMoveParam;
    }
    break;
  case REQUEST_GAMECHANGED:
    if((refCount = m_data.m_gameChangedParam->release()) == 0) {
      delete m_data.m_gameChangedParam;
    }
    break;
  case REQUEST_SHOWMESSAGE  :
    if ((refCount = m_data.m_showMessageParam->release()) == 0) {
      delete m_data.m_showMessageParam;
    }
    break;
  case REQUEST_CONNECT      :
    if((refCount = m_data.m_connectParam->release()) == 0) {
      delete m_data.m_connectParam;
    }
    break;
  default:
    break; // do nothing
  }
#ifdef TEST_REFCOUNT
  if (refCount != 500) {
    verbose(_T("request(%s).release(). refCount=%d\n"), toString().cstr(), refCount);
  }
#endif
  cleanData();
}

void ChessPlayerRequest::cleanData() {
  memset(&m_data, 0, sizeof(m_data));
}

ChessPlayerRequest &ChessPlayerRequest::operator=(const ChessPlayerRequest &src) {
  if(&src != this) {
    release();
    m_type = src.m_type;
    m_data = src.m_data;
    addref();
  }
  return *this;
}

const FindMoveRequestParam &ChessPlayerRequest::getFindMoveParam() const {
  checkType(__TFUNCTION__, REQUEST_FINDMOVE);
  return *m_data.m_findMoveParam;
}

const GameChangedRequestParam &ChessPlayerRequest::getGameChangedParam() const {
  checkType(__TFUNCTION__, REQUEST_GAMECHANGED);
  return *m_data.m_gameChangedParam;
}

const FetchMoveRequestParam &ChessPlayerRequest::getFetchMoveParam() const {
  checkType(__TFUNCTION__, REQUEST_FETCHMOVE);
  return m_data.m_fetchMoveParam;
}

const ShowMessageRequestParam &ChessPlayerRequest::getShowMessageParam() const {
  checkType(__TFUNCTION__, REQUEST_SHOWMESSAGE);
  return *m_data.m_showMessageParam;
}

const ConnectRequestParam &ChessPlayerRequest::getConnectParam() const {
  checkType(__TFUNCTION__, REQUEST_CONNECT);
  return *m_data.m_connectParam;
}

String ChessPlayerRequest::toString() const {
  return getRequestName(m_type);
}

const TCHAR *ChessPlayerRequest::getRequestName(ChessPlayerRequestType request) { // static
#define caseStr(s) case REQUEST_##s: return _T(#s);
  switch(request) {
  caseStr(FINDMOVE   )
  caseStr(NULLMOVE   )
  caseStr(STOPSEARCH )
  caseStr(MOVENOW    )
  caseStr(FETCHMOVE  )
  caseStr(GAMECHANGED)
  caseStr(SHOWMESSAGE)
  caseStr(RESET      )
  caseStr(CONNECT    )
  caseStr(DISCONNECT )
  caseStr(KILL       )
  default:return _T("??");
  }
#undef caseStr
}
