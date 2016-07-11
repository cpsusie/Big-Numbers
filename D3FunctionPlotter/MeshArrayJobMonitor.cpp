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
  int                               m_threadsRunning;
  mutable Semaphore                 m_gate;
  Semaphore                         m_done;
  StringArray                       m_errors;
  void clearResultQueue();
public:
  const MeshArrayJobParameter &m_param;
  MeshArrayJobMonitor(const MeshArrayJobParameter &param)
  : m_param(param)
  , m_done(0)
  {
    m_jobCount       = 0;
    m_threadsRunning = 0;
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
  void waitUntilAllDone();
  void changeThreadCount(int dt);
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

void MeshArrayJobMonitor::changeThreadCount(int dt) {
  m_gate.wait();
  m_threadsRunning += dt;
  if(m_threadsRunning == 0) {
    m_done.signal();
  }
  m_gate.signal();
}

void MeshArrayJobMonitor::waitUntilAllDone() {
  m_done.wait();
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

class MeshBuilderThread : public Thread {
private:
  InteractiveRunnable &m_runnable;
  MeshArrayJobMonitor &m_jobs;
  VariableMeshCreator *m_meshCreator;
public:
  MeshBuilderThread(InteractiveRunnable &runnable, MeshArrayJobMonitor &jobs);
  ~MeshBuilderThread();
  unsigned int run();
};

MeshBuilderThread::MeshBuilderThread(InteractiveRunnable &runnable, MeshArrayJobMonitor &jobs) 
: m_runnable(runnable)
, m_jobs(jobs)
{
  m_meshCreator = m_jobs.m_param.fetchMeshCreator();
}

MeshBuilderThread::~MeshBuilderThread(){
  delete m_meshCreator;
}

unsigned int MeshBuilderThread::run() {
  double t;
  m_jobs.changeThreadCount(1);
  while(m_jobs.fetchJob(t)) {
    if(m_runnable.isInterrupted()) {
      m_jobs.clearJobQueue();
      break;
    }
    try {
      m_jobs.addResult(t, m_meshCreator->createMesh(t));
    } catch(Exception e) {
      m_jobs.addError(e.what());
    } catch(...) {
      m_jobs.addError("Unknown Exception");
    }
  }
  m_jobs.changeThreadCount(-1);
  return 0;
}

class WorkerThreadArray :  public CompactArray<MeshBuilderThread*> {
private:
  InteractiveRunnable &m_runnable;
  MeshArrayJobMonitor &m_jobs;
public:
  WorkerThreadArray(InteractiveRunnable *runnable, MeshArrayJobMonitor &jobs)
  : m_runnable(*runnable)
  , m_jobs(jobs) {
  }
  ~WorkerThreadArray();
  void startNewThread();
};

void WorkerThreadArray::startNewThread() {
  add(new MeshBuilderThread(m_runnable, m_jobs));
  last()->start();
}

WorkerThreadArray::~WorkerThreadArray() {
  for(size_t i = 0; i < size(); i++) {
    delete (*this)[i];
  }
  clear();
}

class MeshArrayCreator : public InteractiveRunnable {
private:
  MeshArrayJobMonitor m_jobs;
  const int           m_frameCount;
public:
  MeshArrayCreator(const MeshArrayJobParameter &param)
  : m_jobs(param)
  , m_frameCount(param.getTimeCount())
  {
  }

  unsigned short getMaxProgress() {
    return m_frameCount;
  }
  unsigned short getProgress() {
    return m_jobs.getJobsDone();
  }
  String getTitle() {
    return "Creating Mesh Array";
  }
  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE | IR_SHOWTIMEESTIMATE;
  }
  MeshArray getResult() {
    return m_jobs.getResult();
  }
  unsigned int run();
};

unsigned int MeshArrayCreator::run() {
  const double stept = m_jobs.m_param.getTimeInterval().getLength() / (m_frameCount-1);
  double       t     = m_jobs.m_param.getTimeInterval().getFrom();

  for(int i = 0; i < m_frameCount; i++, t += stept) {
    m_jobs.addJob(t);
  }
  const int processorCount = getProcessorCount();
  WorkerThreadArray workers(this, m_jobs);
  for(int i = 0; i < processorCount; i++) {
    workers.startNewThread();
  }

  m_jobs.waitUntilAllDone();
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
