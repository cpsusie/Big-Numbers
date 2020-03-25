#include "stdafx.h"
#include <Thread.h>
#include "SyncQueue.h"

typedef SyncQueue<int> TestQueue;

class ProducerThread : public Thread {
private:
  TestQueue &m_queue;
public:
  ProducerThread(TestQueue &queue)
    : Thread(_T("Producer"))
    , m_queue(queue)
  {
  }
  UINT run() {
    for(int i = 0;; i++) {
      _tprintf(_T("put %u\n"), i);
      m_queue.put(i);
      Sleep(1000);
    }
  }
};

class ConsumerThread : public Thread {
private:
  TestQueue &m_queue;
public:
  ConsumerThread(TestQueue &queue)
    : Thread("Consumer")
    , m_queue(queue)
  {
  }
  UINT run() {
    for(;;) {
      _tprintf(_T("calling get\n"));
      const int i = m_queue.get();
      _tprintf(_T("got %u\n"), i);
    }
  }
};

static void usage() {
  _ftprintf(stderr,_T("TestFastSemaphore\n"));
  exit(-1);
}

int _tmain(int argc, const TCHAR **argv) {

  TestQueue queue;
  ProducerThread producer(queue);
  ConsumerThread consumer(queue);

  consumer.start();
  producer.start();

  for(;;) {
    TCHAR line[100];
    GETS(line);
  }
  return 0;
}
