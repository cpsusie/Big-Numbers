#include "pch.h"

void SynchronizedStringQueue::waitForResults(int expectedResultCount) {
  String errorMsg;
  for(;expectedResultCount > 0; expectedResultCount--) {
    TCHAR *str = get();
    if((str != NULL) && (errorMsg.length() == 0)) {
      errorMsg = str;
      FREE(str);
    }
  }
  if(errorMsg.length() > 0) {
    throwException(errorMsg);
  }
}

MultiplierThread::MultiplierThread(int id)
: Thread(format(_T("M%03d"), id), 2048)
, BigRealResource(id)
, m_digitPool(NULL)
, m_resultQueue(NULL)
, m_execute(0)
{
  setDeamon(true);
  m_requestCount = 0;
  start();
}

UINT MultiplierThread::run() {
//  const int id = BigRealResource::getId();
//  const int ypos = id + 20;
  for(;;) {
    try {
      m_execute.wait();
      m_requestCount++;
//      Console::printf(100,ypos, _T("Thread[%d]:request:%4d lvl:%2d"), id, m_requestCount, m_level);
      BigReal::product(*m_result,*m_x, *m_y, *m_f, m_level);
      m_resultQueue->put(NULL);
//      Console::printf(140,ypos, _T("%7.3lf   "), getThreadTime()/1000000);
    } catch(Exception e) {
      m_resultQueue->put(STRDUP(e.what())); // indicating an error
    }
  }
  return 0;
}

void MultiplierThread::multiply(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level) {
  m_result      = &result;
  m_x           = &x;
  m_y           = &y;
  m_f           = &f;
  m_level       = level;
  m_execute.signal();
}
