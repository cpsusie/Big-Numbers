#include "stdafx.h"
#include <Thread.h>
#include "SortJob.h"
#include "SortPanelWnd.h"

template <class T> class SimpleComparator : public CountComparator<T> {
protected:
  SortPanelWnd       &m_wnd;
  std::atomic<char>  &m_jobFlags;
  const T            *m_e0;
public:
  SimpleComparator(SortPanelWnd &wnd)
  : CountComparator<T>(wnd.getCompareCount())
  , m_wnd(wnd)
  , m_jobFlags(wnd.getJobFlags())
  , m_e0((const T*)wnd.getDataArray().getData())
  {
  }

  int cmp(const void *e1, const void *e2) {
    if(m_jobFlags) {
      if(m_jobFlags & TERMINATE_SORT) {
        m_jobFlags &= ~TERMINATE_SORT;
        throw false;
      }
      if(m_jobFlags & PAUSE_SORT) {
        m_jobFlags &= ~PAUSE_SORT;
        m_wnd.waitForResume(STATE_PAUSED);
      }
    }
    return CountComparator<T>::cmp(e1, e2);
  }

  AbstractComparator *clone() const {
    return new SimpleComparator<T>(m_wnd);
  }
};

template <class T> class ComparatorWithUpdate : public SimpleComparator<T> {
protected:
  CClientDC m_dc;
  bool     &m_fast;
public:
  ComparatorWithUpdate(SortPanelWnd &wnd)
    : SimpleComparator<T>(wnd)
    , m_fast(wnd.getFast())
    , m_dc(&wnd)
  {
  }

  int cmp(const void *e1, const void *e2) {
    const size_t index1 = (T*)e1 - m_e0;
    const size_t index2 = (T*)e2 - m_e0;
    m_wnd.updateScreen(m_dc, m_fast);
    m_wnd.updateMarks(m_dc, index1, index2);
    if(!m_fast) {
      Sleep(2);
    }
    return __super::cmp(e1, e2);
  }
};

template <class T> class ComparatorWithUpdateBigSize : public ComparatorWithUpdate<T> {
private:
  const int m_elementSize;
public:
  ComparatorWithUpdateBigSize(SortPanelWnd &wnd)
    : ComparatorWithUpdate<T>(wnd)
    , m_elementSize(wnd.getInitParameters().m_elementSize)
  {
  }

  int cmp(const void *e1, const void *e2) {
    const size_t index1 = ((BYTE*)e1 - (BYTE*)m_e0) / m_elementSize;
    const size_t index2 = ((BYTE*)e2 - (BYTE*)m_e0) / m_elementSize;
    m_wnd.updateScreen(m_dc, m_fast);
    m_wnd.updateMarks(m_dc, index1, index2);
    if(!m_fast) {
      Sleep(2);
    }
    return SimpleComparator<T>::cmp(e1, e2);
  }
};

SortJob::SortJob(SortPanelWnd *wnd)
: m_thread(nullptr)
, m_wnd(*wnd)
{
}

SortJob::~SortJob() {
  m_wnd.m_jobFlags = TERMINATE_SORT | KILL_THREAD;
  if(m_wnd.getJobState() != STATE_RUNNING) {
    m_wnd.setJobState(STATE_RUNNING);
  }
  m_terminated.wait();
}

UINT SortJob::run() {
  m_terminated.wait();
  m_thread = Thread::getCurrentThread();
#if defined(_DEBUG)
  m_thread->setDescription(m_wnd.m_sortMethod.getName());
#endif
  m_wnd.waitForResume(STATE_IDLE);
  while(!(m_wnd.m_jobFlags & KILL_THREAD)) {
    bool sortOk;
    try {
      sortOk = true;
      CClientDC dc(&m_wnd);
      m_wnd.m_startTime = getTimeUsage();

      if(m_wnd.isAnimatedSort()) {
        doAnimatedSort();
        m_wnd.updateScreen(dc, m_wnd.m_fast);
        m_wnd.updateMarks(dc, -1,-1);
      } else {
        doSort();
        m_wnd.updateScreen(dc, true);
      }
      sortOk = m_wnd.m_dataArray.isSorted();
    } catch(bool) {
      // ignore
    } catch(Exception e) {
      showException(e);
    } catch(...) {
      showError(_T("%s:Unknown Exception in Sortthread"), __TFUNCTION__);
    }
    if(m_wnd.m_jobFlags & KILL_THREAD) {
      break;
    }
    if(!sortOk) {
      m_wnd.waitForResume(STATE_ERROR);
    } else {
      m_wnd.waitForResume(STATE_IDLE);
    }
  }
  m_wnd.setJobState(STATE_KILLED);
  m_terminated.notify();
  return 0;
}

void SortJob::doSort() {
  DataArray &array = m_wnd.getDataArray();
  const int n      = array.size();
  const int size   = array.getElementSize();
  switch(size) {
  case sizeof(BYTE):
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, SimpleComparator<BYTE>(m_wnd));
    break;
  case sizeof(unsigned short):
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, SimpleComparator<unsigned short>(m_wnd));
    break;
  default:
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, SimpleComparator<unsigned int>(m_wnd));
    break;
  }
}

void SortJob::doAnimatedSort() {
  DataArray &array = m_wnd.getDataArray();
  const int n      = array.size();
  const int size   = array.getElementSize();

  switch(size) {
  case sizeof(BYTE):
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, ComparatorWithUpdate<BYTE>(m_wnd));
    break;
  case sizeof(unsigned short):
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, ComparatorWithUpdate<unsigned short>(m_wnd));
    break;
  case sizeof(unsigned int)  :
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, ComparatorWithUpdate<unsigned int>(m_wnd));
    break;
  default:
    m_wnd.m_sortMethod.getMethod()(array.getData(), n, size, ComparatorWithUpdateBigSize<unsigned int>(m_wnd));
    break;
  }
}

double SortJob::getTimeUsage() const {
  return m_thread ? m_thread->getThreadTime() : 0;
}
