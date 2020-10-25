#include "pch.h"
#include <Random.h>
#include <Array.h>

/* DEBUG_ARRAYIMPL-klasse til optælling af kald til ArrayImpl */
//#define DEBUG_ARRAYIMPL
#if defined(DEBUG_ARRAYIMPL)

class AutoCounter {
public:
  TCHAR *m_name;
  int        m_counter;
  AutoCounter(TCHAR *name) { m_name = name; m_counter = 0; }
  ~AutoCounter() { _tprintf(_T("%s:count(%s):%d\n"),__TFILE__, m_name,m_counter); }
  AutoCounter &operator++(int k) { m_counter++; return *this;}
};

static AutoCounter resizeUpCounter(  _T("resize up"  ));
static AutoCounter resizeDownCounter(_T("resize down"));
static AutoCounter descounter(       _T("destructor" ));
static AutoCounter initcounter(      _T("init"       ));

#endif

static inline size_t _max(size_t a, size_t b) {
  return a > b ? a : b;
}

ArrayImpl::ArrayImpl(AbstractObjectManager &objectManager, size_t capacity) {
  init(objectManager,0,_max(1,capacity));
}

void ArrayImpl::resize() {
  setCapacity(_max(2 * m_size,m_capacity));
}

void ArrayImpl::setCapacity(size_t capacity) {
  if(capacity != m_capacity) {
    if((int)capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == 0) {
      capacity = 1;
    }

    void **newBuffer = new void*[capacity]; TRACE_NEW(newBuffer);
    if(m_size > 0) {
      memcpy(newBuffer,m_elem,sizeof(m_elem[0])*m_size);
    }
    SAFEDELETEARRAY(m_elem);
    m_elem = newBuffer;

#if defined(DEBUG_ARRAYIMPL)
    if(capacity > m_capacity)
      resizeUpCounter++;
    else
      resizeDownCounter++;
#endif

    m_capacity = capacity;
  }
}

void ArrayImpl::init(const AbstractObjectManager &objectManager, size_t size, size_t capacity) {
  m_objectManager = objectManager.clone(); TRACE_NEW(m_objectManager);
  m_capacity = capacity;

  m_elem = new void*[m_capacity]; TRACE_NEW(m_elem);

  m_updateCount = 0;

//  memset(m_elem,0,m_capacity * sizeof(m_elem[0]));
  m_size = size;
#if defined(DEBUG_ARRAYIMPL)
  initcounter++;
#endif
}

intptr_t ArrayImpl::getSortCount(intptr_t from, intptr_t count) const {
  const intptr_t length = (intptr_t)size() - from;
  return min(count, length);
}

ArrayImpl::~ArrayImpl() {
  clear();
  SAFEDELETE(m_objectManager);
  SAFEDELETEARRAY(m_elem);

#if defined(DEBUG_ARRAYIMPL)
  descounter++;
#endif
}

AbstractCollection *ArrayImpl::clone(bool cloneData) const {
  ArrayImpl *clone = new ArrayImpl(*m_objectManager,m_capacity);
  try {
    if(cloneData) {
      clone->m_size = m_size;
      for(size_t i = 0; i < m_size; i++) {
        clone->m_elem[i] = m_objectManager->cloneObject(m_elem[i]);
      }
    }
  } catch(...) {
    TRACE_NEW( clone);
    SAFEDELETE(clone);
    throw;
  }
  return clone;
}

// if capacity < 0, it's left unchanged
void ArrayImpl::clear(intptr_t capacity) {
  if(m_size > 0) {
    for(size_t i = 0; i < m_size; i++) {
      m_objectManager->deleteObject(m_elem[i]);
    }
    m_size = 0;
    m_updateCount++;
  }
  if(capacity >= 0) {
    setCapacity(capacity);
  }
}

void ArrayImpl::clear() {
  clear(10);
}

void ArrayImpl::indexError(const TCHAR *method, size_t index) const {
  throwIndexOutOfRangeException(method, index, size());
}

void ArrayImpl::indexError(const TCHAR *method, size_t index, size_t count) const {
  throwIndexOutOfRangeException(method, index, count, size());
}

void ArrayImpl::emptyArrayError(const TCHAR *method) { // static
  throwEmptyArrayException(method);
}

void ArrayImpl::unsupportedOperationError(const TCHAR *method) const {
  throwUnsupportedOperationException(method);
}

bool ArrayImpl::add(const void *e) {
  if(m_size >= m_capacity) {
    resize();
  }
  m_elem[m_size++] = m_objectManager->cloneObject(e);
  m_updateCount++;
  return true;
}

bool ArrayImpl::insert(size_t index, const void *e, size_t count) {
  if(index > m_size) indexError(__TFUNCTION__, index);
  if(count == 0) {
    return false;
  }
  if(m_size + count > m_capacity) {
    setCapacity(2*(m_size + count) + 5);
  }
  if(index < m_size) {
    memmove(m_elem+index+count, m_elem+index, (m_size-index)*sizeof(m_elem[0]));
  }
  m_size += count;
  while(count--) {
    m_elem[index++] = m_objectManager->cloneObject(e);
  }
  m_updateCount++;
  return true;
}

void ArrayImpl::removeIndex(size_t index, size_t count) {
  if(count == 0) {
    return;
  }
  const size_t j = index + count;
  if(j > m_size) indexError(__TFUNCTION__, index, count);
  for(size_t k = index; k < j; k++) {
    m_objectManager->deleteObject(m_elem[k]);
  }
  if(j < m_size) {
    memmove(m_elem+index, m_elem+j, (m_size-j)*sizeof(m_elem[0]));
//    m_elem[m_size] = nullptr;
  }
  m_size -= count;
  if(m_size < m_capacity/2 && m_capacity > 10) {
    setCapacity(_max(10, m_size));
  }
  m_updateCount++;
}

bool ArrayImpl::remove(const void *e) {
  unsupportedOperationError(__TFUNCTION__);
  return false;
}

void ArrayImpl::swap(size_t i1, size_t i2) {
  DEFINEMETHODNAME;
  if(i1 >= m_size) indexError(method, i1);
  if(i2 >= m_size) indexError(method, i2);
  if(i1 != i2) {
    std::swap(m_elem[i1], m_elem[i2]);
  }
  m_updateCount++;
}

bool ArrayImpl::contains(const void *e) const {
  unsupportedOperationError(__TFUNCTION__);
  return false;
}
