#pragma once

#include "Array.h"
#include "Exception.h"
#include "Comparator.h"

template <class T> class PriorityQueue {
private:
  Array<T>                     m_a;
  bool                         m_reverseOrder;
  AbstractComparator          *m_comparator;
  FunctionComparator<T>        m_functionComparator;

  bool lessThan(const T &e1, const T &e2) {
    return m_reverseOrder ? (m_comparator->cmp(&e2, &e1) <  0) : (m_comparator->cmp(&e1, &e2) <  0);
  }

  bool lessOrEqual(const T &e1, const T &e2) {
    return m_reverseOrder ? (m_comparator->cmp(&e2, &e1) <= 0) : (m_comparator->cmp(&e1, &e2) <= 0);
  }
public:
  PriorityQueue(int (*cmp)(const T &key1, const T &key2), bool reverseOrder = false) : m_functionComparator(cmp) {
    m_comparator   = &m_functionComparator;
    m_reverseOrder =  reverseOrder;
  }

  PriorityQueue(int (*cmp)(const T *key1, const T *key2), bool reverseOrder = false) : m_functionComparator(cmp) {
    m_comparator   = &m_functionComparator;
    m_reverseOrder =  reverseOrder;
  }

  PriorityQueue(Comparator<T> &comparator, bool reverseOrder = false) {
    m_comparator   = comparator.clone(); TRACE_NEW(m_comparator);
    m_reverseOrder = reverseOrder;
  }

  virtual ~PriorityQueue() {
    if(m_comparator != &m_functionComparator) {
      SAFEDELETE(m_comparator);
    }
  }

  void add(const T &e) {
    size_t s = size();
    m_a.add(e);
    if(s > 0) {
      for(size_t parent = s/2;; s = parent, parent >>= 1) {
        if(lessOrEqual(m_a[parent], m_a[s])) {
          return;
        }
        m_a.swap(parent, s);
      }
    }
  }

  T remove() { // Always removes the lowest element, according to order and m_reverseOrder
    if(isEmpty()) {
      throwException(_T("%s:Cannot delete from empty queue"), __TFUNCTION__);
    }

    T result = m_a[0];
    const size_t last = size()-1;
    m_a.swap(0, last);
    if(last > 1) {
      for(size_t s = 0, child = 1; child < last; s = child, child <<= 1) { // Bubble base[0] down to its place
        if((child+1 < last) && lessThan(m_a[child+1], m_a[child])) {     // s got 2 children. Use the smallest to bubble
          child++;
        }
        if(lessOrEqual(m_a[s], m_a[child])) {
          break;                                         // s is on its place
        }
        m_a.swap(s, child);
      }
    }
    m_a.removeIndex(last);
    return result;
  }

  bool isEmpty() const {
    return m_a.size() == 0;
  }

  size_t size() const {
    return m_a.size();
  }

  void clear() {
    m_a.clear();
  }

  String toString() const {
    return m_a.toString();
  }
};
