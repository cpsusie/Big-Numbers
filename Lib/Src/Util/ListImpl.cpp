#include "pch.h"
#include <LinkedList.h>

/* DEBUG-klasse til optælling af kald til ListImpl */
//#define DEBUG_LISTIMPL
#if defined(DEBUG_LISTIMPL)

class AutoCounter {
public:
  TCHAR *m_name;
  int    m_counter;
  AutoCounter(TCHAR *name) { m_name = name; m_counter = 0; }
  ~AutoCounter() { _tprintf(_T("%s:count(%s):%d\n"), __TFILE__, m_name, m_counter); }
  AutoCounter &operator++(int k) { m_counter++; return *this;}
};

static AutoCounter descounter(   _T("destructor"));
static AutoCounter initcounter(  _T("init"      ));

#endif

ListImpl::ListImpl(AbstractObjectManager &objectManager) {
  init(objectManager);
}

void ListImpl::init(AbstractObjectManager &objectManager) {
  m_objectManager = objectManager.clone(); TRACE_NEW(m_objectManager);
  m_first         = m_last = nullptr;
  m_size          = 0;
  m_updateCount   = 0;
#if defined(DEBUG_LISTIMPL)
  initcounter++;
#endif
}

ListImpl::~ListImpl() {
  clear();
  m_nodePool.releaseAll();
  SAFEDELETE(m_objectManager);
#if defined(DEBUG_LISTIMPL)
  descounter++;
#endif
}

ListNode *ListImpl::createNode(const void *e) {
  ListNode *result = m_nodePool.fetch();
  result->m_data   = m_objectManager->cloneObject(e);
  result->m_next   = result->m_prev = nullptr;
  return result;
}

void ListImpl::deleteNode(ListNode *n) {
  m_objectManager->deleteObject(n->m_data);
  m_nodePool.release(n);
}

const ListNode *ListImpl::findNode(size_t index) const {
  const ListNode *n;
  if(index <= size()/2) {
    size_t i = 0;
    for(n = m_first; i++ < index; n = n->m_next);
  } else {
    size_t i = size()-1;
    for(n = m_last; i-- > index; n = n->m_prev);
  }
  return n;
}

AbstractCollection *ListImpl::clone(bool cloneData) const {
  ListImpl         *clone = nullptr;
  AbstractIterator *it    = nullptr;
  clone = new ListImpl(*m_objectManager);
  try {
    if(cloneData) {
      it = getIterator(); TRACE_NEW(it);
      while(it->hasNext()) {
        clone->add(it->next());
      }
      SAFEDELETE(it);
    }
  } catch(...) {
    TRACE_NEW( clone);
    SAFEDELETE(it   );
    SAFEDELETE(clone);
    throw;
  }
  return clone;
}

void ListImpl::clear() {
  ListNode *next;
  for(ListNode *n = m_first; n != nullptr; n = next) {
    next = n->m_next;
    deleteNode(n);
  }
  m_first = m_last = nullptr;
  m_size = 0;
  m_updateCount++;
}

void ListImpl::indexError(const TCHAR *method, size_t index) const {
  throwIndexOutOfRangeException(method, index, m_size);
}

void ListImpl::removeError(const TCHAR *method) const {
  throwException(_T("%s:Cannot remove element from empty list"), method);
}

void ListImpl::getError(const TCHAR *method) const {
  throwException(_T("%s:List is empty"), method);
}

void *ListImpl::getElement(size_t index) const {
  if(index >= m_size) indexError(__TFUNCTION__, index);
  return (void*)findNode(index)->m_data;
}

bool ListImpl::add(const void *e) {
  ListNode *n = createNode(e);
  if(m_size == 0) {
    m_first = m_last = n;
  } else {
    m_last->m_next = n;
    n->m_prev      = m_last;
    m_last         = n;
  }
  m_size++;
  m_updateCount++;
  return true;
}

bool ListImpl::insert(size_t i, const void *e) {
  if(i > m_size) indexError(__TFUNCTION__, i);
  ListNode *n = createNode(e);
  if(i == 0) {
    n->m_next = m_first;
    if(m_first != nullptr) {
      m_first->m_prev = n;
    }
    m_first = n;
    if(m_last == nullptr) {
      m_last = n;
    }
  } else if(i == m_size) {
    n->m_prev = m_last;
    if(m_last != nullptr) {
      m_last->m_next = n;
    }
    m_last = n;
    if(m_first == nullptr) {
      m_first = n;
    }
  } else {
    ListNode *n1 = (ListNode*)findNode(i);
    n1->m_next->m_prev = n;
    n->m_next          = n1->m_next;
    n1->m_next         = n;
    n->m_prev          = n1;
  }
  m_size++;
  m_updateCount++;
  return true;
}

void ListImpl::removeNode(ListNode *n) {
  if(n->m_prev != nullptr) {
    n->m_prev->m_next = n->m_next;
  } else {
    m_first = n->m_next;
  }
  if(n->m_next != nullptr) {
    n->m_next->m_prev = n->m_prev;
  } else {
    m_last = n->m_prev;
  }
  deleteNode(n);
  m_size--;
  m_updateCount++;
}

void ListImpl::removeFirst() {
  if(size() == 0) {
    removeError(__TFUNCTION__);
  }
  removeNode(m_first);
}

void ListImpl::removeLast() {
  if(size() == 0) {
    removeError(__TFUNCTION__);
  }
  removeNode(m_last);
}

void ListImpl::removeIndex(size_t i) {
  if(i >= m_size) indexError(__TFUNCTION__, i);
  removeNode((ListNode*)findNode(i));
}

bool ListImpl::remove(const void *e) {
  throwUnsupportedOperationException(__TFUNCTION__);
  return false;
}

bool ListImpl::contains(const void *e) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return false;
}

void *ListImpl::first() const {
  if(size() == 0) {
    getError(__TFUNCTION__);
  }
  return m_first->m_data;
}

void *ListImpl::last() const {
  if(size() == 0) {
    getError(__TFUNCTION__);
  }
  return m_last->m_data;
}
