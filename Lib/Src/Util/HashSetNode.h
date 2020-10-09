#pragma once

#include <Set.h>

class HashSetNode : public AbstractKey {
  friend class HashSetImpl;
  friend class HashSetTable;
  friend class HashSetIterator;
private:
  void        *m_key;
  HashSetNode *m_next, **m_prev;        // next/previous node vith same hash-index
  HashSetNode *m_nextLink, *m_prevLink; // for iterator
public:
  inline HashSetNode()
    :m_key     ( nullptr)
    ,m_next    ( nullptr)
    ,m_prev    ( nullptr)
    ,m_nextLink( nullptr)
    ,m_prevLink( nullptr)
  {
  }
  const void *key() const override {
    return m_key;
  }
};
