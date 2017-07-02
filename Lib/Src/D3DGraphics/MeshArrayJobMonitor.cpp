#include "pch.h"
#include <SynchronizedQueue.h>
#include <MFCUtil/ProgressWindow.h>
#include <CPUInfo.h>
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
  mutable Semaphore                 m_gate;
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
  void addJob(  double t);
  bool fetchJob(double &t);
  void clearJobQueue();
  void addResult(double t, LPD3DXMESH mesh);
  void addError(const String &msg);
  int  getJobsDone() const;
  MeshArray getResult();
};

void MeshArrayJobMonitor::addResult(double t, LPD3DXMESH mesh) {
  m_gate.wait();
  m_resultArray.add(MeshResult(t, mesh));
  m_gate.signal();
}

void MeshArrayJobMonitor::addError(const String &msg) {
  m_gate.wait();
  m_errors.add(msg);
  m_gate.signal();
}

void MeshArrayJobMonitor::clearJobQueue() {
  m_gate.wait();
  m_jobStack.clear();
  m_gate.signal();
}

void MeshArrayJobMonitor::addJob(double t) {
  m_gate.wait();
  m_jobStack.push(t);
  m_jobCount++;
  m_gate.signal();
}

bool MeshArrayJobMonitor::fetchJob(double &t) {
  m_gate.wait();
  bool result;
  if(m_jobStack.isEmpty()) {
    result = false;
  } else {
    t = m_jobStack.pop();
    result = true;
  }
  m_gate.signal();
  return result;
}

int MeshArrayJobMonitor::getJobsDone() const {
  m_gate.wait();
  const int n = (int)(m_resultArray.size() + m_errors.size());
  m_gate.signal();
  return n;
}

void MeshArrayJobMonitor::clearResultQueue() { // private. No need to synchronize
  for(size_t i = 0; i < m_resultArray.size(); i++) {
    m_resultArray[i].m_mesh->Release();
  }
  m_resultArray.clear();
}

static int meshResultCmpByTime(const MeshResult &m1, const MeshResult &m2) {
  return sign(m1.m_t - m2.m_t);
}

MeshArray MeshArrayJobMonitor::getResult() {
  m_gate.wait();
  CompactArray<MeshResult> tmp = m_resultArray;
  tmp.sort(meshResultCmpByTime);
  MeshArray result;
  for(size_t i = 0; i < tmp.size(); i++) {
    result.add(tmp[i].m_mesh);
  }
  tmp.clear();
  clearResultQueue();
  m_gate.signal();
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
public:
  MeshArrayCreator(const AbstractMeshArrayJobParameter &param)
  : m_jobMonitor(param)
  , m_frameCount(param.getFrameCount())
  {
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
  UINT run();
};

UINT MeshArrayCreator::run() {
  const double stept = getJobMonitor().m_param.getTimeInterval().getLength() / (m_frameCount-1);
  double       t     = getJobMonitor().m_param.getTimeInterval().getFrom();

  for(UINT i = 0; i < m_frameCount; i++, t += stept) {
    m_jobMonitor.addJob(t);
  }
  const int processorCount = getProcessorCount();
  RunnableArray workerArray;
  for (int i = 0; i < processorCount; i++) {
    workerArray.add(new MeshBuilderWorker(this));
  }
  ThreadPool::executeInParallel(workerArray);
  for(size_t i = 0; i < workerArray.size(); i++) {
    delete workerArray[i];
  }
  workerArray.clear();
  return 0;
}

MeshBuilderWorker::MeshBuilderWorker(MeshArrayCreator *arrayCreator)
: m_arrayCreator(*arrayCreator)
{
  m_meshCreator = getJobMonitor().m_param.fetchMeshCreator();
}

MeshBuilderWorker::~MeshBuilderWorker(){
  delete m_meshCreator;
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
      jm.addResult(t, m_meshCreator->createMesh(t));
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
