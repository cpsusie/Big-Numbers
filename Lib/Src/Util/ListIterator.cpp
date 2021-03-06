#include "pch.h"
#include <LinkedList.h>

class ListIterator : public AbstractIterator {
private:
  ListImpl  &m_list;
  ListNode  *m_next;
  ListNode  *m_current;
  size_t     m_updateCount;
public:
  ListIterator(const ListImpl *list);
  AbstractIterator *clone()         override {
    return new ListIterator(*this);
  }
  bool              hasNext() const override {
    return m_next != nullptr;
  }
  void             *next()          override;
  void              remove()        override;
};

ListIterator::ListIterator(const ListImpl *list) : m_list(*(ListImpl*)list) {
  m_updateCount = m_list.m_updateCount;
  m_next        = m_list.m_first;
  m_current     = nullptr;
}

void *ListIterator::next() {
  if(m_next == nullptr) {
    noNextElementError(__TFUNCTION__);
  }
  __assume(m_next);
  if(m_updateCount != m_list.m_updateCount) {
    concurrentModificationError(__TFUNCTION__);
  }
  m_current = m_next;
  m_next    = m_current->m_next;
  return m_current->m_data;
}

void ListIterator::remove() {
  if(m_current == nullptr) {
    noCurrentElementError(__TFUNCTION__);
  }
  m_list.removeNode(m_current);
  m_current     = nullptr;
  m_updateCount = m_list.m_updateCount;
}

AbstractIterator *ListImpl::getIterator() const {
  return new ListIterator(this);
}
