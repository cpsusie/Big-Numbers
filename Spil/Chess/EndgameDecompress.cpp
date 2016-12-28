#include "stdafx.h"
#include <MFCUtil/ProgressWindow.h>
#include <MFCUtil/resource.h>
#include <CPUInfo.h>
#include <SynchronizedQueue.h>
#include "EndGameTablebase.h"

#ifndef TABLEBASE_BUILDER

class InterruptedException {
public:
  bool dummy;
};

typedef SynchronizedQueue<EndGameTablebase*> DecompressJobQueue;

class DecompressThread : public Thread, public ByteCounter {
private:
  DecompressJobQueue        &m_jobQueue;
  mutable Semaphore          m_gate;
  EndGameTablebase          *m_currentTablebase;
  String                     m_currentMsg;
  UINT64                     m_byteCounterStart;
  UINT64                     m_currentFileSize;
  bool                       m_interrupted, m_terminated;
  void setCurrentFileSize() {
    m_byteCounterStart = getCount();
    m_currentFileSize  = m_currentTablebase ? m_currentTablebase->getFileSize(COMPRESSEDTABLEBASE) : 0;
  }
  void setCurrentMessage(const String &msg) {
    m_gate.wait();
    m_currentMsg = msg;
    m_gate.signal();
  }
  inline bool isInterrupted() const {
    return m_interrupted;
  }
public:
  DecompressThread(DecompressJobQueue &jobQueue);
  ~DecompressThread();
  inline USHORT getSubProgress() {
    return (USHORT)PERCENT(getCount() - m_byteCounterStart, m_currentFileSize);
  }
  String getCurrentMessage() const {
    m_gate.wait();
    const String result = m_currentMsg;
    m_gate.signal();
    return result;
  }
  void incrCount(UINT64 n) {
    if(isInterrupted()) throw InterruptedException();
    ByteCounter::incrCount(n);
  }
  UINT getMaxChunkSize() const {
    return 50000;
  }
  void kill() {
    m_interrupted = true;
    if(!m_terminated) resume();
  }
  UINT run();
};

DecompressThread::DecompressThread(DecompressJobQueue &jobQueue) : m_jobQueue(jobQueue), m_currentTablebase(NULL) {
  setSelectedLanguageForThread();
  setCurrentFileSize();
  m_interrupted = false;
  m_terminated  = false;
}

DecompressThread::~DecompressThread() {
  kill();
  for(int i = 0; i < 10; i++) {
    if(!stillActive()) return;
    Sleep(200);
  }
  debugLog(_T("DecompressThread still running when destructor ended\n"));
}

UINT DecompressThread::run() {
  try {
    for (;;) {
      if(isInterrupted()) break;
      try {
        m_currentTablebase = m_jobQueue.get(500);
      } catch (...) {
        if(!m_jobQueue.isEmpty()) continue;
        break;
      }
      if(isInterrupted()) break;
      setCurrentMessage(m_currentTablebase->getName());
      setCurrentFileSize();
      m_currentTablebase->decompress(this);
      verbose(_T("\n"));
    }
    setCurrentMessage(_T("Done"));
  } catch (...) {
    // do nothing
  }
  m_terminated  = true;
  return 0;
}

class DecompressJob : public InteractiveRunnable {
private:
  DecompressJobQueue              m_jobQueue;
  USHORT                          m_jobQueueStartSize;
  CompactArray<DecompressThread*> m_threadArray;
  String                          m_title;
  void killThreads();
  void clearThreadArray();
  void suspendThreads();
  void resumeThreads();
  bool anyThreadsRunning() const;
public:
  DecompressJob(const EndGameTablebaseList &list);
  ~DecompressJob();
  USHORT getMaxProgress() {         // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
    return m_jobQueueStartSize;
  }
  USHORT getProgress() {            // do. Should return a short in the range [0..getMaxProgress()]
    return (USHORT)(m_jobQueueStartSize - m_jobQueue.size());
  };
  USHORT getSubProgressPercent(UINT index) {  // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
    return m_threadArray[index]->getSubProgress();
  }
  USHORT getJobCount() const {
    return (USHORT)m_threadArray.size();
  }
  String getProgressMessage(UINT index) {
    return m_threadArray[index]->getCurrentMessage();
  }
  String getTitle() {
    return m_title;
  }
  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_INTERRUPTABLE | IR_SUSPENDABLE | IR_SHOWPROGRESSMSG;
  }
  UINT run();
};

DecompressJob::DecompressJob(const EndGameTablebaseList &list) {
  setSelectedLanguageForThread();
  m_jobQueue.addAll(list);
  m_jobQueueStartSize = (USHORT)m_jobQueue.size();
#ifdef _DEBUG
  const int cpuCount = 1;
#else
  const int cpuCount = getProcessorCount();
#endif // _DEBUG
  for (int i = 0; i < cpuCount; i++) {
    m_threadArray.add(new DecompressThread(m_jobQueue));
  }

  m_title = format(_T("%s - %s:%s")
                 ,loadString(IDS_DECOMPRESSALLTITLE).cstr()
                 ,loadString(IDS_METRIC).cstr()
                 ,EndGameKeyDefinition::getMetricName()
                  );
}

DecompressJob::~DecompressJob() {
  clearThreadArray();
}

bool DecompressJob::anyThreadsRunning() const {
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    if(m_threadArray[i]->stillActive()) {
      return true;
    }
  }
  return false;
}

void DecompressJob::killThreads() {
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    m_threadArray[i]->kill();
  }
  for(;;) {
    if(!anyThreadsRunning()) {
      return;
    }
    Sleep(1000);
  }
}

void DecompressJob::clearThreadArray() {
  killThreads();
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    delete m_threadArray[i];
  }
  m_threadArray.clear();
}

void DecompressJob::suspendThreads() {
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    m_threadArray[i]->suspend();
  }
}

void DecompressJob::resumeThreads() {
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    m_threadArray[i]->resume();
  }
}

UINT DecompressJob::run() {
  for (size_t i = 0; i < m_threadArray.size(); i++) {
    m_threadArray[i]->start();
  }
  while(!m_jobQueue.isEmpty() || anyThreadsRunning()) {
    if(isSuspendOrCancelButtonPressed()) {
      if(isInterrupted()) {
        killThreads();
        break;
      } else if(isSuspended()) {
        suspendThreads();
        SuspendThread(GetCurrentThread());
        resumeThreads();
      }
    }
    Sleep(200);
  }
  return 0;
}

static EndGameTablebaseList getNotDecompressedEndGameTablebases() {
  EndGameTablebaseList result = EndGameTablebase::getExistingEndGameTablebases();
  for(size_t i = result.size(); i--;) {
    if(!result[i]->needDecompress()) {
      result.remove(i);
    }
  }
  return result;
}

void EndGameTablebase::decompressAll() { // static
  EndGameTablebaseList jobList = getNotDecompressedEndGameTablebases();
  if(jobList.size() == 0) {
    AfxMessageBox(IDS_ALLTABLEBASESDECOMPRESSED, MB_ICONINFORMATION|MB_OK);
  } else {
    DecompressJob job(jobList);
    ProgressWindow(NULL, job, 0, 200);
    if (job.isInterrupted()) {
      AfxMessageBox(_IDS_INTERRUPTED_BY_USER, MB_ICONEXCLAMATION|MB_OK);
    } else {
      AfxMessageBox(IDS_ALLTABLEBASESDECOMPRESSED, MB_ICONINFORMATION|MB_OK);
    }
  }
}

#endif
