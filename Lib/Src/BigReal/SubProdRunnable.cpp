#include "pch.h"
#include <Thread.h>
#include "ResourcePoolInternal.h"

void SubProdRunnable::clear() {
  m_x         = NULL;
  m_y         = NULL;
  m_f         = NULL;
  m_result    = NULL;
}

UINT SubProdRunnable::run() {
  SETTHREADDESCRIPTION(format(_T("Multiplier(level %d"), m_level));

//  const UINT id = __super::getId();
//  const UINT ypos = id + 20;
//  Console::printf(100,ypos, _T("Thread[%u]:request:%4d lvl:%2d"), id, m_requestCount, m_level);
  BigReal::product(*m_result,*m_x, *m_y, *m_f, m_level);
  return 0;
}

void SubProdRunnable::setInOut(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level) {
  m_result      = &result;
  m_x           = &x;
  m_y           = &y;
  m_f           = &f;
  m_level       = level;
}

void SubProdRunnableArray::reset() {
  __super::clear();
  m_digitPoolArray.clear();
}

SubProdRunnableArray::~SubProdRunnableArray() {
  if(isEmpty() && m_digitPoolArray.isEmpty()) return;
  BigRealResourcePool::releaseSubProdRunnableArray(*this);
}

void SubProdRunnableArray::clear(UINT runnableCount, UINT digitPoolCount) {
  __super::clear(-1);
  setCapacity(runnableCount);
  m_digitPoolArray.clear(-1);
  m_digitPoolArray.setCapacity(digitPoolCount);
}
