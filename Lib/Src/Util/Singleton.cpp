#include "pch.h"
#include <cstdlib>
#include <Singleton.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static FastSemaphore            lock;
static CompactStack<Singleton*> singletonStack;
static bool                     destructorInstalled = false;

// Don't use debugLog in here. infinite recursion
void Singleton::registerThis() {
  lock.wait();
  if(!m_registered) {
    singletonStack.push(this);
    m_registered = true;
    if (!destructorInstalled) {
      atexit(singletonsDestructor);
      destructorInstalled = true;
    }
  }
  lock.notify();
}

// Don't use debugLog in here. infinite recursion
static void singletonsDestructor() {
  lock.wait();
  while(!singletonStack.isEmpty()) {
    Singleton *s = singletonStack.pop();
    s->m_factory.releaseInstance();
  }
  lock.notify();
}
