#include "stdafx.h"
#include "AbstractMoveFinder.h"

MoveFinderThreadRequest::MoveFinderThreadRequest(const Game &game, const TimeLimit &timeLimit, bool hint)
:m_type(REQUEST_FINDMOVE)
{
  m_data.m_findMoveParam = new FindMoveRequestParam(game,timeLimit,hint);
}

MoveFinderThreadRequest::MoveFinderThreadRequest(const Game &game)
:m_type(REQUEST_GAMECHANGED)
{
  m_data.m_gameChangedParam = new MoveFinderRequestParamGame(game);
}

MoveFinderThreadRequest::MoveFinderThreadRequest(const MoveBase &move, bool hint)
:m_type(REQUEST_FETCHMOVE)
{
  m_data.m_searchResult.m_move = move;
  m_data.m_searchResult.m_hint = hint;
}

MoveFinderThreadRequest::MoveFinderThreadRequest(MoveFinderThreadRequestType type) {
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

MoveFinderThreadRequest::MoveFinderThreadRequest(const MoveFinderThreadRequest &src) 
: m_type(src.getType())
, m_data(src.m_data)
{
  addRef();
}

MoveFinderThreadRequest::~MoveFinderThreadRequest() {
  release();
}

void MoveFinderThreadRequest::addRef() {
  switch(m_type) {
  case REQUEST_FINDMOVE   :
    m_data.m_findMoveParam->addRef();
    break;
  case REQUEST_GAMECHANGED:
    m_data.m_gameChangedParam->addRef();
    break;
  default                 :
    break; // do nothing
  }
}

void MoveFinderThreadRequest::release() {
  switch(m_type) {
  case REQUEST_FINDMOVE   :
    if(m_data.m_findMoveParam->release() == 0) {
      delete m_data.m_findMoveParam;
    }
    break;
  case REQUEST_GAMECHANGED:
    if(m_data.m_gameChangedParam->release() == 0) {
      delete m_data.m_gameChangedParam;
    }
    break;
  default:
    break; // do nothing
  }
  cleanData();
}

void MoveFinderThreadRequest::cleanData() {
  memset(&m_data, 0, sizeof(m_data));
}

MoveFinderThreadRequest &MoveFinderThreadRequest::operator=(const MoveFinderThreadRequest &src) {
  if(&src != this) {
    release();
    m_type = src.m_type;
    m_data = src.m_data;
    addRef();
  }
  return *this;
}

const FindMoveRequestParam &MoveFinderThreadRequest::getFindMoveParam() const {
  switch (getType()) {
  case REQUEST_FINDMOVE   : return *m_data.m_findMoveParam;
  default                 : throwInvalidType(__TFUNCTION__);
  }
  return *m_data.m_findMoveParam; // to make compiler happy
}

const MoveFinderRequestParamGame &MoveFinderThreadRequest::getGameChangedParam() const {
  switch (getType()) {
  case REQUEST_GAMECHANGED: return *m_data.m_gameChangedParam;
  default                 : throwInvalidType(__TFUNCTION__);
  }
  return *m_data.m_gameChangedParam; // to make compiler happy
}

const SearchMoveResult &MoveFinderThreadRequest::getSearchResult() const {
  switch (getType()) {
  case REQUEST_FETCHMOVE: return m_data.m_searchResult;
  default               : throwInvalidType(__TFUNCTION__);
  }
  return m_data.m_searchResult; // to make compiler happy
}

String MoveFinderThreadRequest::toString() const {
  return getRequestName(m_type);
}

void MoveFinderThreadRequest::throwInvalidType(const TCHAR *method) const {
  throwException(_T("%s:Request type is %s"), method, getRequestName());
}

const TCHAR *MoveFinderThreadRequest::getRequestName(MoveFinderThreadRequestType request) { // static
#define caseStr(s) case REQUEST_##s: return _T(#s);
  switch(request) {
  caseStr(FINDMOVE   )
  caseStr(NULLMOVE   )
  caseStr(STOPSEARCH )
  caseStr(MOVENOW    )
  caseStr(FETCHMOVE  )
  caseStr(GAMECHANGED)
  caseStr(RESET      )
  caseStr(DISCONNECT )
  caseStr(KILL       )
  default:return _T("??");
  }
#undef caseStr
}
