#include "stdafx.h"
#include <SynchronizedQueue.h>
#include <MFCUtil/ProgressWindow.h>
#include <CPUInfo.h>
#include "MeshArrayJobMonitor.h"

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
  SynchronizedQueue<double>         m_jobQueue;
  SynchronizedQueue<MeshResult>     m_resultQueue;
  int                               m_jobCount;
  mutable Semaphore                 m_gate;
  StringArray                       m_errors;
  void clearResultQueue();
public:
  const MeshArrayJobParameter &m_param;
  MeshArrayJobMonitor(const MeshArrayJobParameter &param)
  : m_param(param)
  {
    m_jobCount       = 0;
  }
  ~MeshArrayJobMonitor() {
    clearResultQueue();
  }
  void addJob(double t) {
    m_jobQueue.put(t);
    m_jobCount++;
  }
  void addResult(double t, LPD3DXMESH mesh);
  void addError(const String msg);
  void clearJobQueue();
  bool fetchJob(double &t);
  int  getJobsDone() const;
  MeshArray getResult();
};

void MeshArrayJobMonitor::addResult(double t, LPD3DXMESH mesh) {
  m_gate.wait();
  m_resultQueue.put(MeshResult(t, mesh));
  m_gate.signal();
}

void MeshArrayJobMonitor::addError(const String msg) {
  m_gate.wait();
  m_errors.add(msg);
  m_gate.signal();
}

void MeshArrayJobMonitor::clearJobQueue() {
  m_gate.wait();
  m_jobQueue.clear();
  m_gate.signal();
}

bool MeshArrayJobMonitor::fetchJob(double &t) {
  m_gate.wait();
  bool result;
  if(m_jobQueue.isEmpty()) {
    result = false;
  } else {
    t = m_jobQueue.get();
    result = true;
  }
  m_gate.signal();
  return result;
}

int MeshArrayJobMonitor::getJobsDone() const {
  return (int)(m_resultQueue.size() + m_errors.size());
}

void MeshArrayJobMonitor::clearResultQueue() {
  m_gate.wait();
  while(!m_resultQueue.isEmpty()) {
    MeshResult mr = m_resultQueue.get();
    mr.m_mesh->Release();
  }
  m_gate.signal();
}

static int meshResultCmpByTime(const MeshResult &m1, const MeshResult &m2) {
  return sign(m1.m_t - m2.m_t);
}

MeshArray MeshArrayJobMonitor::getResult() {
  CompactArray<MeshResult> tmp;
  while(!m_resultQueue.isEmpty()) {
    tmp.add(m_resultQueue.get());
  }
  tmp.sort(meshResultCmpByTime);
  MeshArray result;
  for(size_t i = 0; i < tmp.size(); i++) {
    MeshResult &m = tmp[i];
    result.add(m.m_mesh);
    m.m_mesh->Release();
  }
  return result;
}

class MeshArrayCreator;

class MeshBuilderWorker : public InterruptableRunnable {
private:
  MeshArrayCreator    &m_arrayCreator;
  VariableMeshCreator *m_meshCreator;
  MeshArrayJobMonitor &getJobMonitor();
public:
  MeshBuilderWorker(MeshArrayCreator &arrayCreator);
  ~MeshBuilderWorker();
  UINT run();
};

class MeshArrayCreator : public InteractiveRunnable {
private:
  MeshArrayJobMonitor m_jobMonitor;
  const UINT          m_frameCount;
public:
  MeshArrayCreator(const MeshArrayJobParameter &param)
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
    return "Creating Mesh Array";
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
    workerArray.add(new MeshBuilderWorker(*this));
  }
  ThreadPool::executeInParallel(workerArray);
  for(size_t i = 0; i < workerArray.size(); i++) {
    delete workerArray[i];
  }
  workerArray.clear();
  return 0;
}

MeshBuilderWorker::MeshBuilderWorker(MeshArrayCreator &arrayCreator) 
: m_arrayCreator(arrayCreator)
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
      jm.addError("Unknown Exception");
    }
  }
  return 0;
}

MeshArray MeshArrayJobParameter::createMeshArray(CWnd *wnd) {
  MeshArrayCreator mac(*this);
  ProgressWindow(wnd, mac, 300, 200);
  if(mac.isInterrupted()) {
    throwException(_T("Interrupted by user"));
  };
  return mac.getResult();
}
