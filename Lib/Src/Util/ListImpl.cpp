#include "pch.h"
#include <LinkedList.h>
#include <Random.h>

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
  m_first         = m_last = NULL;
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
  result->m_next   = result->m_prev = NULL;
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
  ListImpl *copy = new ListImpl(*m_objectManager);
  if(cloneData) {
    AbstractIterator *it = ((ListImpl*)this)->getIterator(); TRACE_NEW(it);
    while(it->hasNext()) {
      copy->add(it->next());
    }
    SAFEDELETE(it);
  }
  return copy;
}

void ListImpl::clear() {
  ListNode *next;
  for(ListNode *n = m_first; n != NULL; n = next) {
    next = n->m_next;
    deleteNode(n);
  }
  m_first = m_last = NULL;
  m_size = 0;
  m_updateCount++;
}

void ListImpl::indexError(const TCHAR *method, size_t index) const {
  throwIndexOutOfRangeException(method, index, m_size);
}

void *ListImpl::getElement(size_t index) {
  if(index >= m_size) indexError(__TFUNCTION__, index);
  return (void*)findNode(index)->m_data;
}

const void *ListImpl::getElement(size_t index) const {
  if(index >= m_size) indexError(__TFUNCTION__, index);
  return findNode(index)->m_data;
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

bool ListImpl::add(size_t i, const void *e) {
  if(i > m_size) indexError(__TFUNCTION__, i);
  ListNode *n = createNode(e);
  if(i == 0) {
    n->m_next = m_first;
    if(m_first != NULL) {
      m_first->m_prev = n;
    }
    m_first = n;
    if(m_last == NULL) {
      m_last = n;
    }
  } else if(i == m_size) {
    n->m_prev = m_last;
    if(m_last != NULL) {
      m_last->m_next = n;
    }
    m_last = n;
    if(m_first == NULL) {
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
  if(n->m_prev != NULL) {
    n->m_prev->m_next = n->m_next;
  } else {
    m_first = n->m_next;
  }
  if(n->m_next != NULL) {
    n->m_next->m_prev = n->m_prev;
  } else {
    m_last = n->m_prev;
  }
  deleteNode(n);
  m_size--;
  m_updateCount++;
}

static const TCHAR *removeErrorText      = _T("%s:Cannot remove element from empty list.");
static const TCHAR *getElementeErrorText = _T("%s:List is empty");

void ListImpl::removeFirst() {
  if(size() == 0) {
    throwException(removeErrorText, __TFUNCTION__);
  }
  removeNode(m_first);
}

void ListImpl::removeLast() {
  if(size() == 0) {
    throwException(removeErrorText, __TFUNCTION__);
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

const void *ListImpl::select(RandomGenerator &rnd) const {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return findNode(randSizet(size(), rnd))->m_data;
}

void *ListImpl::select(RandomGenerator &rnd) {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return findNode(randSizet(size(), rnd))->m_data;
}

void *ListImpl::first() {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return m_first->m_data;
}

const void *ListImpl::first() const {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return m_first->m_data;
}

void *ListImpl::last() {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return m_last->m_data;
}

const void *ListImpl::last() const {
  if(size() == 0) {
    throwException(getElementeErrorText, __TFUNCTION__);
  }
  return m_last->m_data;
}

// -------------------------------------------------------------------------------

class ListIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  ListImpl           &m_list;
  ListNode           *m_next;
  ListNode           *m_current;
  size_t              m_updateCount;
public:
  AbstractIterator *clone();
  ListIterator(ListImpl &list);
  bool hasNext() const;
  void *next();
  void remove();
};

DEFINECLASSNAME(ListIterator);

ListIterator::ListIterator(ListImpl &list) : m_list(list) {
  m_updateCount = m_list.m_updateCount;
  m_next    = m_list.m_first;
  m_current = NULL;
}

AbstractIterator *ListIterator::clone() {
  return new ListIterator(*this);
}

bool ListIterator::hasNext() const {
  return m_next != NULL;
}

void *ListIterator::next() {
  if(m_next == NULL) {
    noNextElementError(s_className);
  }
  __assume(m_next);
  if(m_updateCount != m_list.m_updateCount) {
    concurrentModificationError(s_className);
  }
  m_current = m_next;
  m_next    = m_current->m_next;
  return m_current->m_data;
}

void ListIterator::remove() {
  if(m_current == NULL) {
    noCurrentElementError(s_className);
  }
  m_list.removeNode(m_current);
  m_current = NULL;
  m_updateCount = m_list.m_updateCount;
}

AbstractIterator *ListImpl::getIterator() {
  return new ListIterator(*this);
}
