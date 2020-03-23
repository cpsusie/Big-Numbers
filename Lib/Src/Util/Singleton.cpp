#include "pch.h"
#include <cstdlib>
#include <Singleton.h>

typedef enum {
  REQUEST_STACK
 ,RELEASE_STACK
} StackRequest;

typedef CompactStack<Singleton*> _SingletonStack;

static _SingletonStack *stackRequest(StackRequest request) {
  static FastSemaphore    lock;
  static _SingletonStack *stack = NULL;
  switch(request) {
  case REQUEST_STACK:
    lock.wait();
    if(stack == NULL) {
      stack = new _SingletonStack();
    }
    return stack;
  case RELEASE_STACK:
    if(stack && stack->isEmpty()) {
      delete stack;
      stack = NULL;
    }
    lock.notify();
    break;
  default:
    abort();
  }
  return NULL;
}

// Don't use debugLog in here. infinite recursion
static inline _SingletonStack &getStack() {
  return *stackRequest(REQUEST_STACK);
}

static inline void releaseStack() {
  stackRequest(RELEASE_STACK);
}

// Don't use debugLog in here. infinite recursion
void Singleton::registerThis() {
  if(!m_registered) {
    getStack().push(this);
    m_registered = true;
    static bool destructorInstalled = false;
    if(!destructorInstalled) {
      atexit(singletonsDestructor);
      destructorInstalled = true;
    }
    releaseStack();
  }
}

// Don't use debugLog in here. infinite recursion
static void singletonsDestructor() {
  _SingletonStack &stack = getStack();
  while(!stack.isEmpty()) {
    Singleton *s = stack.pop();
    s->m_factory.releaseInstance();
  }
  releaseStack();
}
