#include "pch.h"
#include <cstdlib>
#include <Singleton.h>

template <typename T> class SimpleStack {
private:
  size_t  m_capacity, m_size;
  T      *m_items;

  void indexError(const TCHAR *method, size_t index) const {
    throwIndexOutOfRangeException(method, index, m_size);
  }

  void selectError(const TCHAR *method) const {
    throwSelectFromEmptyCollectionException(method);
  }

  void createItems(size_t capacity) {
    if(capacity == 0) {
      m_items = NULL;
    } else {
      m_items = new T[capacity];
      memset(m_items, 0, capacity * sizeof(T));
    }
    m_capacity = capacity;
  }
  inline SimpleStack &destroyItems() {
    if(m_items) {
      delete[] m_items;
      m_items = NULL;
    }
    m_capacity = 0;
    return *this;
  }

  inline void init(size_t capacity) {
    m_size        = 0;
    createItems(capacity);
  }

  SimpleStack &setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return *this;
    }
    T *newItems = capacity ? new T[capacity] : NULL;
    if(m_size) {
      __assume(m_size);
      if(newItems == NULL) {
        throwException(_T("new failed"));
      }
      __assume(newItems);
      memcpy(newItems, m_items, m_size * sizeof(T));
    }
    if(capacity > m_size) {
      T           *extra  = newItems + m_size;
      const size_t nbytes = (capacity - m_size) * sizeof(T);
      memset(extra, 0, nbytes);
    }
    destroyItems();
    m_items    = newItems;
    m_capacity = capacity;
    return *this;
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  SimpleStack(const SimpleStack<T> &src);                // not implemented
  SimpleStack<T> &operator=(const SimpleStack<T> &src);  // not implemented

public:
  SimpleStack() {
    init(0);
  }

  explicit SimpleStack(size_t capacity) {
    init(capacity);
  }

  ~SimpleStack() {
    destroyItems();
  }

  SimpleStack &push(const T &e) {
    if(m_size == m_capacity) {
      setCapacity(3*m_capacity+5);
    }
    m_items[m_size++] = e;
    return *this;
  }
  T pop() {
    if(isEmpty()) throwException(_T("stack underflow"));
    return m_items[--m_size];
  }
  inline T &top() {
    if(isEmpty()) indexError(__TFUNCTION__, m_size);
    return m_items[m_size-1];
  }
  inline const T &top() const {
    if(isEmpty()) indexError(__TFUNCTION__, m_size);
    return m_items[m_size-1];
  }
  inline bool isEmpty() const {
    return m_size == 0;
  }
  inline SimpleStack &clear() {
    m_size = 0;
    return destroyItems();
  }
};

typedef enum {
  REQUEST_STACK
 ,RELEASE_STACK
} StackRequest;

typedef SimpleStack<Singleton*> _SingletonStack;

static _SingletonStack *stackRequest(StackRequest request) {
  static FastSemaphore    lock;
  static _SingletonStack *stack = NULL;
  switch(request) {
  case REQUEST_STACK:
    lock.wait();
    if(stack == NULL) {
      stack = new _SingletonStack(50);
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
