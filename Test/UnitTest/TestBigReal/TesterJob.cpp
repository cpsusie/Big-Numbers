#include "stdafx.h"
#include <Console.h>
#include <Math/FPU.h>
#include "FunctionTest.h"

Semaphore       TesterJob::s_gate;
Semaphore       TesterJob::s_allDone(0);
int             TesterJob::s_runningCount    = 0;
bool            TesterJob::s_allOk           = true;
double          TesterJob::s_totalThreadTime = 0;
TestQueue       TesterJob::s_testQueue;
TestQueue       TesterJob::s_doneQueue;
BigRealJobArray TesterJob::s_testerJobs;

void TesterJob::incrRunning() {
  s_gate.wait();
  s_runningCount++;
  s_gate.signal();
}

void TesterJob::decrRunning() {
  s_gate.wait();
  s_runningCount--;
  if(s_runningCount == 0 && s_testQueue.isEmpty()) {
    s_allDone.signal();
  }
  s_gate.signal();
}

void TesterJob::addTimeUsage(double threadTime) { // static
  s_gate.wait();
  s_totalThreadTime += threadTime;
  s_gate.signal();
}

unsigned int TesterJob::run() {
  incrRunning();
  const int ypos = THREADYPOS(m_id);

  DigitPool *pool = BigRealResourcePool::fetchDigitPool();
  const int thrId = GetCurrentThreadId();

  while(!s_testQueue.isEmpty()) {
    AbstractFunctionTest *test;
    try {
      test = s_testQueue.get(0);
    } catch(TimeoutException) {
      break;
    }

    Console::clearLine(ypos);
    Console::printf(0 , ypos, _T("Thread %d Pool %d"), thrId, pool->getId());
    try {
      test->runTest(m_id, pool);
    } catch(Exception e) {
      log(_T("Exception in thread %d testing %s:%s"), thrId, test->getFunctionName().cstr(), e.what());
      ERRLOG << e.what() << endl;
      TestStatistic::screenlog(_T("%s"), e.what());
      tcout.flush();
      s_allOk = false;
    }
    s_doneQueue.put(test);
  }
  addTimeUsage(getThreadTime());
  BigRealResourcePool::releaseDigitPool(pool);
  Console::clearLine(ypos);
  decrRunning();
  return 0;
}

void TesterJob::runAll(UINT threadCount) { // static
  startAll(threadCount);
  waitUntilAllDone();
  releaseAll();
}

void TesterJob::startAll(UINT threadCount) { // static
  if(threadCount == 1) {
    FPU::setPrecisionMode(FPU_HIGH_PRECISION);
    TesterJob job(0);
    job.run();
  } else {
    s_gate.wait();
    for(UINT i = 0; i < threadCount; i++) {
      Runnable *job = new TesterJob(i); TRACE_NEW(job);
      s_testerJobs.add(job);
    }
    s_gate.signal();
    BigRealResourcePool::executeInParallel(s_testerJobs);
  }
}

void TesterJob::releaseAll() { // static
  s_gate.wait();
  while(!s_doneQueue.isEmpty()) {
    AbstractFunctionTest *test = s_doneQueue.get();
    SAFEDELETE(test);
  }
  for(size_t i = 0; i < s_testerJobs.size(); i++) {
    SAFEDELETE(s_testerJobs[i]);
  }
  s_testerJobs.clear();
  s_gate.signal();
}

void TesterJob::shuffleTestOrder() { // static
  s_gate.wait();
  CompactArray<AbstractFunctionTest*> testArray;
  while(!s_testQueue.isEmpty()) {
    testArray.add(s_testQueue.get());
  }
  testArray.shuffle();
  for(size_t i = 0; i < testArray.size(); i++) {
    s_testQueue.put(testArray[i]);
  }
  testArray.clear();
  s_gate.signal();
}
