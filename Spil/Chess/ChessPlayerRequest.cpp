#include "stdafx.h"
#include "AbstractMoveFinder.h"

//#define TEST_REFCOUNT

ChessPlayerRequest::ChessPlayerRequest(const Game &game, const TimeLimit &timeLimit, bool hint, bool verbose)
:m_type(REQUEST_FINDMOVE)
{
  m_param = new RequestParamFindMove(game, timeLimit, hint, verbose); TRACE_NEW(m_param);
}

ChessPlayerRequest::ChessPlayerRequest(const MoveBase &move, bool hint)
:m_type(REQUEST_FETCHMOVE)
{
  m_param = new RequestParamFetchMove(SearchMoveResult(move, hint)); TRACE_NEW(m_param);
}

ChessPlayerRequest::ChessPlayerRequest(const Game &game)
:m_type(REQUEST_GAMECHANGED)
{
  m_param = new RequestParamGameChanged(game); TRACE_NEW(m_param);
}

ChessPlayerRequest::ChessPlayerRequest(const PrintableMove &move)
: m_type(REQUEST_MOVEDONE)
{
  m_param = new RequestParamMoveDone(move); TRACE_NEW(m_param);
}

ChessPlayerRequest::ChessPlayerRequest(const String &msgText, bool error)
:m_type(REQUEST_SHOWMESSAGE)
{
  m_param = new RequestParamShowMessage(msgText, error); TRACE_NEW(m_param);
}

  // REQUEST_CONNECT
ChessPlayerRequest::ChessPlayerRequest(const SocketChannel &channel)
:m_type(REQUEST_CONNECT)
{
  m_param = new RequestParamConnect(channel); TRACE_NEW(m_param);
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
    m_param = nullptr;
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
  if(m_param != nullptr) {
    const int refCount = m_param->addref();
#if defined(TEST_REFCOUNT)
    verbose(_T("request(%s).addRef(). refCount=%d\n"), toString().cstr(), refCount);
#endif
  }
}

void ChessPlayerRequest::release() {
  if(m_param != nullptr) {
    const int refCount = m_param->release();
    if(refCount == 0) {
      SAFEDELETE(m_param);
    }
    m_param = nullptr;
#if defined(TEST_REFCOUNT)
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

const RequestParamFindMove &ChessPlayerRequest::getParamFindMove() const {
  checkType(__TFUNCTION__, REQUEST_FINDMOVE);
  return *(RequestParamFindMove*)m_param;
}

const RequestParamFetchMove &ChessPlayerRequest::getParamFetchMove() const {
  checkType(__TFUNCTION__, REQUEST_FETCHMOVE);
  return *(RequestParamFetchMove*)m_param;
}

const RequestParamGameChanged &ChessPlayerRequest::getParamGameChanged() const {
  checkType(__TFUNCTION__, REQUEST_GAMECHANGED);
  return *(RequestParamGameChanged*)m_param;
}

const RequestParamMoveDone &ChessPlayerRequest::getParamMoveDone() const {
  checkType(__TFUNCTION__, REQUEST_MOVEDONE);
  return *(RequestParamMoveDone*)m_param;
}

const RequestParamShowMessage &ChessPlayerRequest::getParamShowMessage() const {
  checkType(__TFUNCTION__, REQUEST_SHOWMESSAGE);
  return *(RequestParamShowMessage*)m_param;
}

const RequestParamConnect &ChessPlayerRequest::getParamConnect() const {
  checkType(__TFUNCTION__, REQUEST_CONNECT);
  return *(RequestParamConnect*)m_param;
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
