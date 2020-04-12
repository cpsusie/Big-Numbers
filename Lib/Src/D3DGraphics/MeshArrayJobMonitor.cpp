#include "pch.h"
#include <CompactStack.h>
#include <CPUInfo.h>
#include <ThreadPool.h>
#include <MFCUtil/ProgressWindow.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>

class MeshResult {
public:
  double     m_t;
  LPD3DXMESH m_mesh;
  MeshResult() : m_t(0), m_mesh(NULL) {
  }
  MeshResult(double t, LPD3DXMESH mesh) : m_t(t), m_mesh(mesh) {
  }
};

class MeshArrayJobMonitor {
private:
  CompactStack<double>              m_jobStack;
  CompactArray<MeshResult>          m_resultArray;
  int                               m_jobCount;
  mutable FastSemaphore             m_lock;
  StringArray                       m_errors;
  void clearResultQueue();
public:
  const AbstractMeshArrayJobParameter &m_param;
  MeshArrayJobMonitor(const AbstractMeshArrayJobParameter &param)
  : m_param(param)
  {
    m_jobCount = 0;
  }
  ~MeshArrayJobMonitor() {
    clearResultQueue();
  }
  void      addJob(  double t);
  bool      fetchJob(double &t);
  void      clearJobQueue();
  void      addResult(double t, LPD3DXMESH mesh);
  void      addError(const String &msg);
  int       getJobsDone() const;
  MeshArray getResult();
};

void MeshArrayJobMonitor::addResult(double t, LPD3DXMESH mesh) {
  m_lock.wait();
  m_resultArray.add(MeshResult(t, mesh));
  m_lock.notify();
}

void MeshArrayJobMonitor::addError(const String &msg) {
  m_lock.wait();
  m_errors.add(msg);
  m_lock.notify();
}

void MeshArrayJobMonitor::clearJobQueue() {
  m_lock.wait();
  m_jobStack.clear();
  m_lock.notify();
}

void MeshArrayJobMonitor::addJob(double t) {
  m_lock.wait();
  m_jobStack.push(t);
  m_jobCount++;
  m_lock.notify();
}

bool MeshArrayJobMonitor::fetchJob(double &t) {
  m_lock.wait();
  bool result;
  if(m_jobStack.isEmpty()) {
    result = false;
  } else {
    t = m_jobStack.pop();
    result = true;
  }
  m_lock.notify();
  return result;
}

int MeshArrayJobMonitor::getJobsDone() const {
  m_lock.wait();
  const int n = (int)(m_resultArray.size() + m_errors.size());
  m_lock.notify();
  return n;
}

void MeshArrayJobMonitor::clearResultQueue() { // private. No need to synchronize
  for(size_t i = 0; i < m_resultArray.size(); i++) {
    LPD3DXMESH mesh = m_resultArray[i].m_mesh;
    SAFERELEASE(mesh);
  }
  m_resultArray.clear();
}

static int meshResultCmpByTime(const MeshResult &m1, const MeshResult &m2) {
  return sign(m1.m_t - m2.m_t);
}

MeshArray MeshArrayJobMonitor::getResult() {
  m_lock.wait();
  CompactArray<MeshResult> tmp = m_resultArray;
  tmp.sort(meshResultCmpByTime);
  MeshArray result;
  for(size_t i = 0; i < tmp.size(); i++) {
    result.add(tmp[i].m_mesh);
  }
  tmp.clear();
  clearResultQueue();
  m_lock.notify();
  return result;
}

class MeshArrayCreator;

class MeshBuilderWorker : public Runnable {
private:
  MeshArrayCreator            &m_arrayCreator;
  AbstractVariableMeshCreator *m_meshCreator;
  MeshArrayJobMonitor         &getJobMonitor();
public:
  MeshBuilderWorker(MeshArrayCreator *arrayCreator);
  ~MeshBuilderWorker();
  UINT run();
};

class MeshArrayCreator : public InteractiveRunnable {
private:
  MeshArrayJobMonitor m_jobMonitor;
  const UINT          m_frameCount;
  RunnableArray       m_workerArray;
  void cleanup();
public:
  MeshArrayCreator(const AbstractMeshArrayJobParameter &param)
  : m_jobMonitor(param)
  , m_frameCount(param.getFrameCount())
  {
  }
  ~MeshArrayCreator() {
    cleanup();
  }
  double getMaxProgress() const {
    return m_frameCount;
  }
  double getProgress() const {
    return m_jobMonitor.getJobsDone();
  }
  String getTitle() {
    return _T("Creating Mesh Array");
  }
  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE | IR_SHOWTIMEESTIMATE;
  }
  MeshArrayJobMonitor &getJobMonitor() {
    return m_jobMonitor;
  }
  MeshArray getResult() {
    return m_jobMonitor.getResult();
  }
  UINT safeRun();
};

UINT MeshArrayCreator::safeRun() {
  const double stept = getJobMonitor().m_param.getTimeInterval().getLength() / (m_frameCount-1);
  double       t     = getJobMonitor().m_param.getTimeInterval().getFrom();

  for(UINT i = 0; i < m_frameCount; i++, t += stept) {
    m_jobMonitor.addJob(t);
  }
#ifdef _DEBUG
  const int processorCount = getDebuggerPresent() ? 1 : getProcessorCount();
#else
  const int processorCount = getProcessorCount();
#endif
  for(int i = 0; i < processorCount; i++) {
    Runnable *r = new MeshBuilderWorker(this); TRACE_NEW(r);
    m_workerArray.add(r);
  }
  try {
    ThreadPool::executeInParallel(m_workerArray);
    cleanup();
  } catch(...) {
    cleanup();
    throw;
  }
  return 0;
}

void MeshArrayCreator::cleanup() {
  for(size_t i = 0; i < m_workerArray.size(); i++) {
    Runnable *r = m_workerArray[i];
    SAFEDELETE(r);
  }
  m_workerArray.clear();
}

MeshBuilderWorker::MeshBuilderWorker(MeshArrayCreator *arrayCreator)
: m_arrayCreator(*arrayCreator)
{
  m_meshCreator = getJobMonitor().m_param.fetchMeshCreator();
}

MeshBuilderWorker::~MeshBuilderWorker(){
  SAFEDELETE(m_meshCreator);
}

MeshArrayJobMonitor &MeshBuilderWorker::getJobMonitor() {
  return m_arrayCreator.getJobMonitor();
}

UINT MeshBuilderWorker::run() {
  MeshArrayJobMonitor &jm = getJobMonitor();
  double t;
  while(jm.fetchJob(t)) {
    if(m_arrayCreator.isInterrupted()) {
      jm.clearJobQueue();
      break;
    }
    try {
      jm.addResult(t, m_meshCreator->createMesh(t, &m_arrayCreator));
    } catch(Exception e) {
      jm.addError(e.what());
    } catch(...) {
      jm.addError(_T("Unknown Exception"));
    }
  }
  return 0;
}

MeshArray AbstractMeshArrayJobParameter::createMeshArray(CWnd *wnd) {
  MeshArrayCreator mac(*this);
  ProgressWindow(wnd, mac, 300, 200);
  if(mac.isInterrupted()) {
    throwException(_T("Interrupted by user"));
  };
  return mac.getResult();
}
