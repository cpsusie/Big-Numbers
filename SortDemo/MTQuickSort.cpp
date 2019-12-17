#include "stdafx.h"
#include <Thread.h>
#include <ThreadPool.h>
#include <CPUInfo.h>
#include <SynchronizedQueue.h>

class SynchronizedComparator : public AbstractComparator {
private:
  FastSemaphore       m_lock;
  AbstractComparator &m_cmp;
public:
  SynchronizedComparator(AbstractComparator &cmp)
    : m_cmp(cmp)
  {
  }
  int cmp(const void *e1, const void *e2);
  AbstractComparator *clone() const;
};

int SynchronizedComparator::cmp(const void *e1, const void *e2) {
  m_lock.wait();
  try {
    const int result = m_cmp.cmp(e1, e2);
    m_lock.notify();
    return result;
  } catch (...) {
    m_lock.notify();
    throw;
  }
}

AbstractComparator *SynchronizedComparator::clone() const {
  throwException(_T("SynchonizedComparator not cloneable"));
  return NULL;
}

template<typename T> class JobParam {
public:
  T      *m_base;
  size_t  m_size;

  JobParam(T *base = NULL, size_t size = 0) {
    m_base = base;
    m_size = size;
  }
  inline bool isEmpty() const {
    return m_size == 0;
  }
};

template<typename T> class PartitionWorker;

template<typename T> class MTQuicksortClass {
private:
  RunnableArray                        m_workerArray; // 1 thread for each processor in the CPU
  SynchronizedQueue<JobParam<T> >      m_jobQueue;    // Common jobqueue for all threads
  AbstractComparator                  *m_comparator;  // the comparator specifying the order of the elements
  const int                            m_processorCount;
  FastSemaphore                        m_lock;
  bool                                 m_killAllRequest;
  void cleanup();
public:
  MTQuicksortClass()
  : m_processorCount(getProcessorCount())
  , m_killAllRequest(false)
  {
  }
  ~MTQuicksortClass() {
    cleanup();
  }
  void sort(T *base, size_t nelem, AbstractComparator &comparator);

  inline void addJob(T *base, size_t size) {
    m_jobQueue.put(JobParam<T>(base, size));
  }
  inline JobParam<T> &getJob(int id, JobParam<T> &job);

  AbstractComparator &getComparator() {
    return *m_comparator;
  }
  void killAll();
  std::atomic<UINT> m_activeCount;
};

template<typename T> class PartitionWorker : public Runnable {
private:
  int                  m_id;
  MTQuicksortClass<T> &m_qc;
  AbstractComparator  &m_comparator;
  FastSemaphore        m_terminated;
  void doPartition(const JobParam<T> &job);
public:
  PartitionWorker(MTQuicksortClass<T> &qc, int id);
  ~PartitionWorker() {
    m_terminated.wait();
  }
  UINT run();
};

// ------------------------------------- MTQuicksortClass implementation --------------------------------------

template<typename T> void MTQuicksortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  cleanup();
  m_comparator = &comparator;
  for(int i = 0; i < m_processorCount; i++) {
    PartitionWorker<T> *worker = new PartitionWorker<T>(*this, i); TRACE_NEW(worker);
    m_workerArray.add(worker);
  }
  m_activeCount = (UINT)m_workerArray.size();
  addJob(base, nelem);
  ThreadPool::executeInParallel(m_workerArray);
}

template<typename T> void MTQuicksortClass<T>::cleanup() {
  for(size_t i = 0; i < m_workerArray.size(); i++) {
    Runnable *job = m_workerArray[i];
    SAFEDELETE(job);
  }
  m_workerArray.clear();
  m_jobQueue.clear();
}

template<typename T> void MTQuicksortClass<T>::killAll() {
  m_lock.wait();
  if(!m_killAllRequest) {
    m_killAllRequest = true;
    m_jobQueue.clear();
    for(size_t i = 0; i < m_workerArray.size(); i++) {
      addJob(NULL,0);
    }
  }
  m_lock.notify();
}

template<typename T> JobParam<T> &MTQuicksortClass<T>::getJob(int id, JobParam<T> &job) {
  const int thisActiveAcount = m_activeCount--;
  if((thisActiveAcount == 1) && m_jobQueue.isEmpty()) { // we are the only active worker and there are no jobs in queue...time to move on
    killAll();
  }
  job = m_jobQueue.get();
  m_activeCount++;
  return job;
}

// ------------------------------------- PartitionWorker implementation --------------------------------------

template<typename T> PartitionWorker<T>::PartitionWorker(MTQuicksortClass<T> &qc, int id)
: m_qc(qc)
, m_comparator(m_qc.getComparator())
{
  m_id = id;
}

#define MINMTSPLITSIZE 20

template<typename T> UINT PartitionWorker<T>::run() {
#ifdef _DEBUG
  setThreadDescription(format(_T("Partition(%d)"), m_id));
#endif
  m_terminated.wait();
  try {
    JobParam<T> job;
    for(;;) {
      m_qc.getJob(m_id, job);
      if(job.isEmpty()) {
        break;
      }
      doPartition(job);
    }
  } catch(...) {
    m_qc.killAll();
    m_qc.m_activeCount--;
  }
  m_terminated.notify();
  return 0;
}

template<typename T> void PartitionWorker<T>::doPartition(const JobParam<T> &job) {
  DECLARE_STACK(stack, 80);
  PUSH(stack, job.m_base, job.m_size);
  while(!ISEMPTY(stack)) {
    T     *base;
    size_t nelem;
    POP(stack, base, nelem, T);

tailrecurse:
    switch(nelem) {
    case 0:
    case 1:
      continue;
    case 2:
      MTSORT2(0, 1);
      continue;
    case 3:
      MTSORT3OPT(0, 1, 2);
      continue;
    default:
      MTSORT3OPT(0, nelem/2, nelem-1);
      break;
    }

    const T pivot = base[nelem/2];
    T *ip = TEPTR(1), *jp = TEPTR(nelem-2);
    do {
      while(ip <= jp && m_comparator.cmp(ip,&pivot) < 0) ip++;  // while e[i]  < pivot
      while(ip <= jp && m_comparator.cmp(&pivot,jp) < 0) jp--;  // while pivot < e[j]
      if(ip < jp) {
        TPSWAP(ip, jp);
      }
      if(ip <= jp) {
        ip++;
        jp--;
      }
    } while(ip <= jp);
    const size_t i = ip - base;
    const size_t j = jp - base;

    if(j > nelem - i) {
      if(j > 0) {
        const size_t n = j+1;
        if(n < MINMTSPLITSIZE) {
          PUSH(stack, base, n);
        } else {
          m_qc.addJob(base, n);
        }
      }
      if(i < nelem-1) {
        const size_t n = nelem - i;
        if(n < MINMTSPLITSIZE) {
          base  = ip;
          nelem = n;
          goto tailrecurse;
        } else {
          m_qc.addJob(ip, n);
        }
      }
    } else {
      if(i < nelem-1) {
        const size_t n = nelem - i;
        if(n < MINMTSPLITSIZE) {
          PUSH(stack, ip, n);
        } else {
          m_qc.addJob(ip, n);
        }
      }
      if(j > 0) {
        const size_t n = j+1;
        if(n < MINMTSPLITSIZE) {
          nelem = n;
          goto tailrecurse;
        } else {
          m_qc.addJob(base, n);
        }
      }
    }
  }
}

void MTQuickSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  if(nelem < MINMTSPLITSIZE) {
    quickSort(base, nelem, width, comparator);
  } else {
    SynchronizedComparator syncCmp(comparator);
    switch(width) {
    case sizeof(char)  :
      MTQuicksortClass<char   >().sort((char*)base   , nelem, syncCmp);
      break;
    case sizeof(short) :
      MTQuicksortClass<short  >().sort((short*)base  , nelem, syncCmp);
      break;
    case sizeof(long)  : // include pointertypes
      MTQuicksortClass<long   >().sort((long*)base   , nelem, syncCmp);
      break;
    case sizeof(__int64):
      MTQuicksortClass<__int64>().sort((__int64*)base, nelem, syncCmp);
      break;
    default            : // for all other values of width, we must use the hard way to copy and swap elements
      quickSort(base, nelem, width, comparator);
      break;
    }
  }
}
