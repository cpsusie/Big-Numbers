#include "stdafx.h"
#include <Console.h>
#include <Math/FPU.h>
#include <ThreadPool.h>
#include "FunctionTest.h"

FastSemaphore   TesterJob::s_gate;
bool            TesterJob::s_allOk           = true;
bool            TesterJob::s_stopOnError     = false;
double          TesterJob::s_totalThreadTime = 0;
TestQueue       TesterJob::s_testQueue;
TestQueue       TesterJob::s_doneQueue;

void TesterJob::addTimeUsage(double threadTime) { // static
  s_gate.wait();
  s_totalThreadTime += threadTime;
  s_gate.notify();
}

UINT TesterJob::run() {
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
  const int ypos = THREADYPOS(m_id);
  const int thrId = GetCurrentThreadId();

  while(!s_testQueue.isEmpty()) {
    AbstractFunctionTest *test;
    try {
      test = s_testQueue.get(0);
    } catch(TimeoutException) {
      break;
    }

    if(!s_allOk && s_stopOnError) {
      s_doneQueue.put(test);
      continue;
    }

    Console::clearLine(ypos);
    Console::printf(0 , ypos, _T("Thread %5d Pool %d"), thrId, m_pool->getId());
    try {
      test->runTest(m_id, m_pool);
    } catch(StopException) {
      // ignore
    } catch(Exception e) {
      log(_T("Exception in thread %d testing %s:%s"), thrId, test->getFunctionName().cstr(), e.what());
      ERRLOG << e.what() << endl;
      TestStatistic::screenlog(_T("%s"), e.what());
      tcout.flush();
      s_allOk = false;
      if(s_stopOnError) {
        TestStatistic::stopNow();
      }
    }
    s_doneQueue.put(test);
  }
  addTimeUsage(getThreadTime());
  Console::clearLine(ypos);
  return 0;
}

void TesterJob::runAll(UINT threadCount, bool stopOnError) { // static
  s_stopOnError = stopOnError;
  startAll(threadCount);
  releaseAll();
}

void TesterJob::startAll(UINT threadCount) { // static
  if(threadCount == 1) {
    TesterJob(0).run();
  } else {
    RunnableArray jobs(threadCount);
    for(UINT i = 0; i < threadCount; i++) {
      Runnable *job = new TesterJob(i); TRACE_NEW(job);
      jobs.add(job);
    }
    ThreadPool::executeInParallel(jobs);
    for(UINT i = 0; i < jobs.size(); i++) {
      Runnable *job = jobs[i];
      SAFEDELETE(job);
    }
    jobs.clear();
  }
}

void TesterJob::releaseAll() { // static
  s_gate.wait();
  while(!s_doneQueue.isEmpty()) {
    AbstractFunctionTest *test = s_doneQueue.get();
    SAFEDELETE(test);
  }
  s_gate.notify();
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
  s_gate.notify();
}
