#include "stdafx.h"
#include "AbstractMoveFinder.h"

//#define TEST_REFCOUNT

ChessPlayerRequest::ChessPlayerRequest(const Game &game, const TimeLimit &timeLimit, bool hint, bool verbose)
:m_type(REQUEST_FINDMOVE)
{
  m_param = new FindMoveRequestParam(game, timeLimit, hint, verbose);
}

ChessPlayerRequest::ChessPlayerRequest(const MoveBase &move, bool hint)
:m_type(REQUEST_FETCHMOVE)
{
  m_param = new FetchMoveRequestParam(SearchMoveResult(move, hint));
}

ChessPlayerRequest::ChessPlayerRequest(const Game &game)
:m_type(REQUEST_GAMECHANGED)
{
  m_param = new GameChangedRequestParam(game);
}

ChessPlayerRequest::ChessPlayerRequest(const PrintableMove &move)
: m_type(REQUEST_MOVEDONE)
{
  m_param = new MoveDoneRequestParam(move);
}

ChessPlayerRequest::ChessPlayerRequest(const String &msgText, bool error)
:m_type(REQUEST_SHOWMESSAGE)
{
  m_param = new ShowMessageRequestParam(msgText, error);
}

  // REQUEST_CONNECT
ChessPlayerRequest::ChessPlayerRequest(const SocketChannel &channel)
:m_type(REQUEST_CONNECT)
{
  m_param = new ConnectRequestParam(channel);
}

ChessPlayerRequest::ChessPlayerRequest(ChessPlayerRequestType type) {
  switch (type) {
  case REQUEST_NULLMOVE     :
  case REQUEST_STOPSEARCH   :
  case REQUEST_MOVENOW      :
  case REQUEST_RESET        :
  case REQUEST_DISCONNECT   :
  case REQUEST_KILL         :
    m_type  = type;
    m_param = NULL;
    break;
  default                   :
    throwInvalidArgumentException(__TFUNCTION__,_T("type=%s"), getRequestName(type));
  }
}

ChessPlayerRequest::ChessPlayerRequest(const ChessPlayerRequest &src) 
: m_type(src.getType())
, m_param(src.m_param)
{
  addref();
}

ChessPlayerRequest::~ChessPlayerRequest() {
  release();
}

void ChessPlayerRequest::addref() {
  if(m_param != NULL) {
    const int refCount = m_param->addref();
#ifdef TEST_REFCOUNT
    verbose(_T("request(%s).addRef(). refCount=%d\n"), toString().cstr(), refCount);
#endif
  }
}

void ChessPlayerRequest::release() {
  if(m_param != NULL) {
    const int refCount = m_param->release();
    if(refCount == 0) {
      delete m_param;
    }
    m_param = NULL;
#ifdef TEST_REFCOUNT
    verbose(_T("request(%s).release(). refCount=%d\n"), toString().cstr(), refCount);
#endif
  }
}

ChessPlayerRequest &ChessPlayerRequest::operator=(const ChessPlayerRequest &src) {
  if(&src != this) {
    release();
    m_type  = src.m_type;
    m_param = src.m_param;
    addref();
  }
  return *this;
}

const FindMoveRequestParam &ChessPlayerRequest::getFindMoveParam() const {
  checkType(__TFUNCTION__, REQUEST_FINDMOVE);
  return *(FindMoveRequestParam*)m_param;
}

const FetchMoveRequestParam &ChessPlayerRequest::getFetchMoveParam() const {
  checkType(__TFUNCTION__, REQUEST_FETCHMOVE);
  return *(FetchMoveRequestParam*)m_param;
}

const GameChangedRequestParam &ChessPlayerRequest::getGameChangedParam() const {
  checkType(__TFUNCTION__, REQUEST_GAMECHANGED);
  return *(GameChangedRequestParam*)m_param;
}

const MoveDoneRequestParam &ChessPlayerRequest::getMoveDoneParam() const {
  checkType(__TFUNCTION__, REQUEST_MOVEDONE);
  return *(MoveDoneRequestParam*)m_param;
}

const ShowMessageRequestParam &ChessPlayerRequest::getShowMessageParam() const {
  checkType(__TFUNCTION__, REQUEST_SHOWMESSAGE);
  return *(ShowMessageRequestParam*)m_param;
}

const ConnectRequestParam &ChessPlayerRequest::getConnectParam() const {
  checkType(__TFUNCTION__, REQUEST_CONNECT);
  return *(ConnectRequestParam*)m_param;
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
  caseStr(MOVEDONE   )
  caseStr(SHOWMESSAGE)
  caseStr(RESET      )
  caseStr(CONNECT    )
  caseStr(DISCONNECT )
  caseStr(KILL       )
  default:return _T("??");
  }
#undef caseStr
}
