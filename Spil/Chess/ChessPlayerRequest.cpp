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

ChessPlayerRequest::ChessPlayerRequest(const String &errMsg, bool error)
:m_type(REQUEST_SHOWMESSAGE)
{
  m_data.m_showMessageParam = new ShowMessageRequestParam(errMsg, error);
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
  addRef();
}

ChessPlayerRequest::~ChessPlayerRequest() {
  release();
}

void ChessPlayerRequest::addRef() {
  int refCount = 0;
  switch(m_type) {
  case REQUEST_FINDMOVE   :
    refCount = m_data.m_findMoveParam->addRef();
    break;
  case REQUEST_GAMECHANGED:
    refCount = m_data.m_gameChangedParam->addRef();
    break;
  case REQUEST_SHOWMESSAGE:
    refCount = m_data.m_showMessageParam->addRef();
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
    addRef();
  }
  return *this;
}

const FindMoveRequestParam &ChessPlayerRequest::getFindMoveParam() const {
  switch (getType()) {
  case REQUEST_FINDMOVE   : return *m_data.m_findMoveParam;
  default                 : throwInvalidType(__TFUNCTION__);
  }
  return *m_data.m_findMoveParam; // to make compiler happy
}

const GameChangedRequestParam &ChessPlayerRequest::getGameChangedParam() const {
  switch (getType()) {
  case REQUEST_GAMECHANGED: return *m_data.m_gameChangedParam;
  default                 : throwInvalidType(__TFUNCTION__);
  }
  return *m_data.m_gameChangedParam; // to make compiler happy
}

const FetchMoveRequestParam &ChessPlayerRequest::getFetchMoveParam() const {
  switch (getType()) {
  case REQUEST_FETCHMOVE: return m_data.m_fetchMoveParam;
  default               : throwInvalidType(__TFUNCTION__);
  }
  return m_data.m_fetchMoveParam; // to make compiler happy
}

const ShowMessageRequestParam &ChessPlayerRequest::getShowMessageParam() const {
  switch (getType()) {
  case REQUEST_SHOWMESSAGE: return *m_data.m_showMessageParam;
  default                 : throwInvalidType(__TFUNCTION__);
  }
  return *m_data.m_showMessageParam; // to make compiler happy
}

String ChessPlayerRequest::toString() const {
  return getRequestName(m_type);
}

void ChessPlayerRequest::throwInvalidType(const TCHAR *method) const {
  throwException(_T("%s:Request type is %s"), method, getRequestName());
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
  caseStr(DISCONNECT )
  caseStr(KILL       )
  default:return _T("??");
  }
#undef caseStr
}
