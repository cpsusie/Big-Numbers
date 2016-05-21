#include "pch.h"
#include <LinkedList.h>
#include <Random.h>

/* DEBUG-klasse til optælling af kald til ListImpl */
#ifdef DEBUG

class AutoCounter {
public:
  char *m_name;
  int   m_counter;
  AutoCounter(char *name) { m_name = name; m_counter = 0; }
  ~AutoCounter() { printf("count(%s):%d\n",m_name,m_counter); }
  AutoCounter &operator++(int k) { m_counter++; return *this;}
};

static AutoCounter resizecounter("resize");
static AutoCounter descounter("destructor");
static AutoCounter initcounter("init");

#endif

DEFINECLASSNAME(ListImpl);

ListImpl::ListImpl(AbstractObjectManager &objectManager) {
  init(objectManager);
}

void ListImpl::init(AbstractObjectManager &objectManager) {
  m_objectManager = objectManager.clone();
  m_firstPage     = NULL;
  m_freeList      = NULL;
  m_first         = m_last = NULL;
  m_size          = 0;
  m_updateCount   = 0;
#ifdef DEBUG
  initcounter++;
#endif
}

ListImpl::~ListImpl() {
  clear();
  releaseAllPages();
  delete m_objectManager;
#ifdef DEBUG
  descounter++;
#endif
}

ListNode *ListImpl::createNode(const void *e) {
  ListNode *result = fetchNode();
  result->m_data   = m_objectManager->cloneObject(e);
  result->m_next   = result->m_prev = NULL;
  return result;
}

void ListImpl::deleteNode(ListNode *n) {
  m_objectManager->deleteObject(n->m_data);
  releaseNode(n);
}

const ListNode *ListImpl::findNode(unsigned int index) const {
  if(index <= (unsigned int)size()/2) {
    unsigned int i = 0;
    for(ListNode *n = m_first; n && i++ < index; n = n->m_next);
    return n;
  } else {
    if(size() == 0) {
      return NULL;
    }
    unsigned int i = size()-1;
    for(ListNode *n = m_last; n && i-- > index; n = n->m_prev);
    return n;
  }
}

AbstractCollection *ListImpl::clone(bool cloneData) const {
  ListImpl *copy = new ListImpl(*m_objectManager);
  if(cloneData) {
    for(AbstractIterator *it = ((ListImpl*)this)->getIterator(); it->hasNext();) {
      copy->add(it->next());
    }
    delete it;
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

void ListImpl::releaseAllPages() {
  for(ListNodePage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_nextPage;
    delete p;
  }
  m_firstPage = NULL;
  m_freeList  = NULL;
}

void ListImpl::throwOutOfRangeException(const TCHAR *method, unsigned int index) const {
  throwMethodInvalidArgumentException(s_className, method, _T("index %u out of range. Size=%lu."), index, m_size);
}

void *ListImpl::getElement(unsigned int index) {
  if(index >= m_size) { 
    throwOutOfRangeException(_T("getElement"), index);
  }
  return (void*)findNode(index)->m_data;
}

const void *ListImpl::getElement(unsigned int index) const {
  if(index >= m_size) {
    throwOutOfRangeException(_T("const getElement"), index);
  }
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

bool ListImpl::add(unsigned int i, const void *e) {
  if(i > m_size) {
    throwOutOfRangeException(_T("add"), i);
  }
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

static const TCHAR *removeErrorText      = _T("%s::%s:Cannot remove element from empty list.");
static const TCHAR *getElementeErrorText = _T("%s::%s:List is empty");

void ListImpl::removeFirst() {
  if(size() == 0) {
    throwException(removeErrorText, s_className, _T("removeFirst"));
  }
  removeNode(m_first);
}

void ListImpl::removeLast() {
  if(size() == 0) {
    throwException(removeErrorText, s_className, _T("removeLast"));
  }
  removeNode(m_last);
}

void ListImpl::removeIndex(unsigned int i) {
  if(i >= m_size) {
    throwOutOfRangeException(_T("removeIndex"), i);
  }
  removeNode((ListNode*)findNode(i));
}

bool ListImpl::remove(const void *e) {
  throwMethodUnsupportedOperationException(s_className, _T("remove"));
  return false;
}

bool ListImpl::contains(const void *e) const {
  throwMethodUnsupportedOperationException(s_className, _T("contains"));
  return false;
}

const void *ListImpl::select() const {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("select"));
  }
  return findNode(randInt() % size())->m_data;
}

void *ListImpl::select() {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("select"));
  }
  return findNode(randInt() % size())->m_data;
}

void *ListImpl::first() {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("first"));
  }
  return m_first->m_data;
}

const void *ListImpl::first() const {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("first"));
  }
  return m_first->m_data;
}

void *ListImpl::last() {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("last"));
  }
  return m_last->m_data;
}

const void *ListImpl::last() const {
  if(size() == 0) {
    throwException(getElementeErrorText, s_className, _T("last"));
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
  unsigned long       m_updateCount;
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

ListNodePage::ListNodePage(ListNodePage *nextPage) : m_nextPage(nextPage) {
  ListNode *p = &LASTVALUE(m_nodeArray);
  
  for((p--)->m_next = NULL; p >= m_nodeArray; p--) {
    p->m_next = p+1;
  }
}
