#include "stdafx.h"
#include <Console.h>
#include <Tokenizer.h>
#include <Thread.h>
#include <InterruptableRunnable.h>
#include <ThreadPool.h>
#include <SynchronizedQueue.h>

typedef SynchronizedQueue<int> TestQueue;

typedef enum {
  PRODUCER = 0x1
 ,CONSUMER = 0x2
 ,ALL      = 0x3
} TesterType;

class QueueTester : public InterruptableRunnable {
protected:
  const TesterType   m_type;
  const int          m_id;
  const String       m_name;
  TestQueue         &m_queue;
  UINT               m_sleepmsec;
  bool               m_verbose;
  const int          m_consoleLine;
  int                m_lmargin;
  QueueTester(TesterType type, int id, TestQueue &queue, int consoleLine)
    : m_type(type)
    , m_id(id)
    , m_name(format(_T("%s %02d"), (type==PRODUCER)?_T("Producer"):_T("Consumer"),id))
    , m_queue(queue)
    , m_sleepmsec(1000)
    , m_verbose(true)
    , m_consoleLine(consoleLine)
    , m_lmargin(0)
  {
    out(0, _T("%s:"), m_name.cstr());
    m_lmargin += 15;
  }
  inline void takeANap() {
    if(m_sleepmsec) {
      Sleep(m_sleepmsec);
    }
  }
  void out(int x, const TCHAR *format, ...);
  void showStatus();
public:
  inline void setVerbose(bool v) {
    m_verbose = v;
    showStatus();
  }
  inline void setSleepTime(UINT msec) {
    m_sleepmsec = msec;
    showStatus();
  }
  inline TesterType getType() const {
    return m_type;
  }
  inline int getId() const {
    return m_id;
  }
};

void QueueTester::out(int x, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  Console::vcprintf(x+m_lmargin, m_consoleLine, format, argptr);
  va_end(argptr);
}

void QueueTester::showStatus() {
  Console::cprintf(100, m_consoleLine, _T("Verbose:%5s,sleep:%5u"), boolToStr(m_verbose), m_sleepmsec);
}

class Producer : public QueueTester {
private:
  static int s_producerCount;
public:
  Producer(int id, TestQueue &queue) : QueueTester(PRODUCER, id, queue, s_producerCount++) {
  }
  UINT safeRun() {
    setThreadDescription(m_name);
    showStatus();
    const int start = 10000 * m_id, end = start + 10000;
    int sendCounter = 0;
    for(int i = start; i <= end; i++) {
      if(m_verbose) {
        takeANap();
        out(0, _T("put(%u)"),i);
      }
      checkInterruptAndSuspendFlags();
      m_queue.put(i);
      sendCounter++;
    }
    out(0, _T("Total number sent:%-15s:%d"), format(_T("[%d..%d]"),start, end).cstr(), sendCounter);
    return 0;
  }
  static int getCount() {
    return s_producerCount;
  }
};

int Producer::s_producerCount = 0;

class Consumer : public QueueTester {
private:
  static int s_consumerCount;
  int        m_timeoutmsec;
public:
  Consumer(int id, TestQueue &queue)
    : QueueTester(CONSUMER, id, queue, 10+s_consumerCount++)
    , m_timeoutmsec(1000)
  {
  }
  UINT safeRun() {
    setThreadDescription(m_name);
    showStatus();
    int timeoutCounter = 0, receivedCounter = 0;
    while(timeoutCounter < 1000) {
      if(m_verbose) {
        takeANap();
        out(0,_T("Call get(timeout=%d):%s"), m_timeoutmsec, _T(""));
      }
      try {
        checkInterruptAndSuspendFlags();
        const int i = m_queue.get(m_timeoutmsec);
        receivedCounter++;
        if(m_verbose) out(35, _T("...Got %5u  (%10s            "), i, format1000(receivedCounter).cstr());
      } catch(TimeoutException e) {
        timeoutCounter++;
        if(m_verbose) out(35, _T("Exception:%s    (%10s)           "), e.what(), format1000(timeoutCounter).cstr());
      }
    }
    out(0, _T("Total number received:%d, timeouts:%d"), receivedCounter, timeoutCounter);
    return 0;
  }
  inline void setTimeout(int msec) {
    m_timeoutmsec = msec;
  }
  static int getCount() {
    return s_consumerCount;
  }
};

int Consumer::s_consumerCount = 0;

class UserInput : public Runnable {
private:
  TestQueue                  m_queue;
  CompactArray<QueueTester*> m_allRunners;
  void suspendAll(  TesterType flags = ALL);
  void interruptAll(TesterType flags = ALL);
  void resumeAll(   TesterType flags = ALL);
  void setVerboseAll(bool verbose, TesterType flags = ALL);
  void setTimeoutAllConsumers(int msec);
  void setSleepAll(           UINT msec, TesterType flags = ALL);
  void quitAll();
  QueueTester *findQueueTester(TesterType type, int id) const {
    for(size_t i = 0; i < m_allRunners.size(); i++) {
      QueueTester *qt = m_allRunners[i];
      if((qt->getType() == type) && (qt->getId())) {
        return qt;
      }
    }
    throwException(_T("%s with id %d not found."), (type==PRODUCER)?_T("Producer"):_T("Consumer"),id);
    return nullptr;
  }
  inline Consumer *findConsumer(int id) const {
    return (Consumer*)findQueueTester(CONSUMER, id);
  }
  inline Producer *findProducer(int id) const {
    return (Producer*)findQueueTester(PRODUCER, id);
  }
public:
  UINT run();
};

UINT UserInput::run() {
  setThreadDescription(_T("Userinput"));
  COORD sz = Console::getWindowSize();
  const String prompt = _T("Enter command:(S)uspend <id>, (R)esume <id>, Add (C)consumer, Add (P)roducer, (A)ll {suspend|resume|verbose|silent|timeout <msec>|sleep <msec>}, (Q)uit:");
  for(;;) {
    Console::printf(0, sz.Y-2, _T("%-*s"), sz.X-1,prompt.cstr());
    Console::setCursorPos((int)prompt.length(), sz.Y-2);
    String line;
    readLine(stdin, line); line.trim();
    Console::setCursorPos((int)(prompt.length() + line.length()), sz.Y-2);
    line = toLowerCase(line);
    const String delim = _T(" ");
    Tokenizer tok(line, delim);
    try {
      const String w = tok.next();
      const TCHAR cmd = w[0];
      switch(cmd) {
      case 'c':
        { const int   id = Consumer::getCount();
          Consumer   *t  = new Consumer(id, m_queue);
          m_allRunners.add(t);
          ThreadPool::executeNoWait(*t);
        }
        break;
      case 'p':
        { const int id = Producer::getCount();
          Producer *t  = new Producer(id, m_queue);
          m_allRunners.add(t);
          ThreadPool::executeNoWait(*t);
        }
        break;
      case 's':
      case 'r':
        { const UINT id = tok.getUint();
          if(cmd == 's') {
            findProducer(id)->setSuspended();
          } else {
            findProducer(id)->resume();
          }
        }
        break;
      case 'a':
        { String subcommand = tok.next();
          TesterType flags = ALL;
          if(subcommand == "c") {
            flags      = CONSUMER;
            subcommand = tok.next();
          } else if(subcommand == "p") {
            flags      = PRODUCER;
            subcommand = tok.next();
          }
          if(subcommand == "suspend") {
            suspendAll(flags);
          } else if(subcommand == "resume") {
            resumeAll(flags);
          } else if(subcommand == "verbose") {
            setVerboseAll(true,flags);
          } else if(subcommand == "silent") {
            setVerboseAll(false,flags);
          } else if(subcommand == "timeout") {
            const int msec = tok.getInt();
            setTimeoutAllConsumers(msec);
          } else if(subcommand == "sleep") {
            const UINT msec = tok.getUint();
            setSleepAll(msec,flags);
          } else {
            throwException(_T("Invalid subcommand:%s"), subcommand.cstr());
          }
        }
        break;
      case 'q':
        quitAll();
        return 0;
      } // switch
      Console::cprintf(0, sz.Y-1, _T("%-*s"), sz.X-1,_T(" "));
    } catch(Exception e) {
      Console::cprintf(0, sz.Y-1, _T("Error:%-*s"), sz.X-1, e.what());
    }
  } // for
}

void UserInput::suspendAll(TesterType flags) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & flags) {
      t->setSuspended();
    }
  }
}

void UserInput::interruptAll(TesterType flags) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & flags) {
      t->setInterrupted();
    }
  }
}

void UserInput::resumeAll(TesterType flags) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & flags) {
      t->resume();
    }
  }
}
void UserInput::setVerboseAll(bool verbose, TesterType flags) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & flags) {
      t->setVerbose(verbose);
    }
  }
}
void UserInput::setTimeoutAllConsumers(int msec) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & CONSUMER) {
      ((Consumer*)t)->setTimeout(msec);
    }
  }
}

void UserInput::setSleepAll(UINT msec, TesterType flags) {
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    QueueTester *t = m_allRunners[i];
    if(t->getType() & flags) {
      ((Consumer*)t)->setSleepTime(msec);
    }
  }
}

void UserInput::quitAll() {
  suspendAll(PRODUCER);
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    m_queue.put(0);
  }
  interruptAll();
  for(size_t i = 0; i < m_allRunners.size(); i++) {
    delete m_allRunners[i];
  }
  m_allRunners.clear();
  m_queue.clear();
}

static void usage() {
  _ftprintf(stderr,_T("TestFastSemaphore\n"));
  exit(-1);
}

int _tmain(int argc, const TCHAR **argv) {
  UserInput input;
  input.run();
  return 0;
}
