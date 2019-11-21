#include "stdafx.h"
#include <Thread.h>
#include <TinyBitSet.h>
#include <CPUInfo.h>
#include <SynchronizedQueue.h>

template <class T> class PartitionJob {
public:
  T      *m_base;
  size_t  m_size;

  PartitionJob(T *base = NULL, size_t size = 0) {
    m_base = base;
    m_size = size;
  }
  inline bool isEmpty() const {
    return m_size == 0;
  }
};

template <class T> class PartitioningThread;

template <class T> class MTQuicksortClass {
private:
  CompactArray<PartitioningThread<T>*> m_threadArray; // 1 thread for each processor in the CPU
  SynchronizedQueue<PartitionJob<T> >  m_jobQueue;    // Common jobqueue for all threads
  AbstractComparator                  *m_comparator;  // the comparator specifying the order of the elements
  const int                            m_processorCount;
  BitSet32                             m_activeThreads;
  Semaphore                            m_gate, m_done;
  bool                                 m_killed;

  void cleanup();
public:
  MTQuicksortClass()
  : m_processorCount(getProcessorCount())
  , m_done(0)
  , m_killed(false)
  {
  }
  ~MTQuicksortClass() {
    cleanup();
  }
  void sort(T *base, size_t nelem, AbstractComparator &comparator);

  inline void addJob(T *base, size_t size) {
    m_jobQueue.put(PartitionJob<T>(base, size));
  }
  inline PartitionJob<T> &getJob(int id, PartitionJob<T> &job);

  AbstractComparator &getComparator() {
    return *m_comparator;
  }
  void killAll();
  void setActive(int id, bool active);
};

template <class T> class PartitioningThread : public Thread {
private:
  int                  m_id;
  MTQuicksortClass<T> &m_qc;
  AbstractComparator  &m_comparator;
  void doPartition(const PartitionJob<T> &job);
public:
  PartitioningThread(MTQuicksortClass<T> &qc, int id);
  unsigned int run();
};

// ------------------------------------- MTQuicksortClass implementation --------------------------------------

template <class T> void MTQuicksortClass<T>::sort(T *base, size_t nelem, AbstractComparator &comparator) {
  cleanup();
  m_comparator = &comparator;
  for(int i = 0; i < m_processorCount; i++) {
    m_threadArray.add(new PartitioningThread<T>(*this, i));
  }
  addJob(base, nelem);
  for(size_t i = 0; i < m_threadArray.size(); i++) {
    m_threadArray[i]->start();
  }
  m_done.wait();
  if (m_killed) {
    throw false;
  } else {
    killAll();
  }
}

template <class T> void MTQuicksortClass<T>::cleanup() {
  for(size_t i = 0; i < m_threadArray.size(); i++) {
    Thread *thr = m_threadArray[i];
    for (int k = 0; k < 10; k++) {
      if(!thr->stillActive()) break;
      Sleep(20);
    }
    delete thr;
  }
  m_threadArray.clear();
  m_killed = false;
}

template <class T> void MTQuicksortClass<T>::setActive(int id, bool active) {
  m_gate.wait();
  if(active) {
    m_activeThreads.add(id);
  } else {
    m_activeThreads.remove(id);
    if (m_activeThreads.isEmpty()) {
      m_done.notify();
    }
  }
  m_gate.notify();
}

template <class T> void MTQuicksortClass<T>::killAll() {
  m_gate.wait();
  if(!m_killed) {
    m_killed = true;
    m_jobQueue.clear();
    for (size_t i = 0; i < m_threadArray.size(); i++) {
      addJob(NULL,0);
    }
  }
  m_gate.notify();
}

template <class T> PartitionJob<T> &MTQuicksortClass<T>::getJob(int id, PartitionJob<T> &job) {
  if(!m_jobQueue.isEmpty()) {
    job = m_jobQueue.get();
  } else {
    setActive(id, false);
    job = m_jobQueue.get();
    setActive(id, true);
  }
  return job;
}

// ------------------------------------- PartitioningThread implementation --------------------------------------

template <class T> PartitioningThread<T>::PartitioningThread(MTQuicksortClass<T> &qc, int id)
: m_qc(qc)
, m_comparator(m_qc.getComparator())
{
  m_id = id;
  setDeamon(true);
}

#define MINMTSPLITSIZE 20

template <class T> unsigned int PartitioningThread<T>::run() {
  m_qc.setActive(m_id, true);
  try {
    PartitionJob<T> job;
    for(;;) {
      m_qc.getJob(m_id, job);
      if(job.isEmpty()) break;
      doPartition(job);
    }
  } catch (...) {
    m_qc.killAll();
  }
  m_qc.setActive(m_id, false);
  return 0;
}

template <class T> void PartitioningThread<T>::doPartition(const PartitionJob<T> &job) {
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

class SynchronizedComparator : public AbstractComparator {
private:
  Semaphore           m_gate;
  AbstractComparator &m_cmp;
public:
  SynchronizedComparator(AbstractComparator &cmp) : m_cmp(cmp) {
  }
  int cmp(const void *e1, const void *e2);
  AbstractComparator *clone() const;
};

int SynchronizedComparator::cmp(const void *e1, const void *e2) {
  m_gate.wait();
  try {
    const int result = m_cmp.cmp(e1, e2);
    m_gate.notify();
    return result;
  }
  catch (...) {
    m_gate.notify();
    throw;
  }
}

AbstractComparator *SynchronizedComparator::clone() const {
  throwException(_T("SynchonizedComparator not cloneable"));
  return NULL;
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
