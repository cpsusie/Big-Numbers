#pragma once

#include <HashSet.h>

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

class HashSetTable {
  friend class HashSetImpl;
  friend class HashSetIterator;
private:
  const HashSetImpl  &m_owner;
  const size_t        m_capacity;
  HashSetNode       **m_table;
  HashSetNode        *m_firstLink, *m_lastLink;
  size_t              m_size;
  size_t              m_updateCount;
  int chainLength(size_t index) const;
  int getMaxChainLength() const;
  CompactIntArray getLength() const;
public:
  HashSetTable(const HashSetImpl &owner, size_t capacity);
  ~HashSetTable();
  void insert(size_t index, HashSetNode *n);
  void remove(HashSetNode *n);
  void clear();
  const AbstractKey *select(RandomGenerator &rnd) const;
  inline size_t getCapacity() const {
    return m_capacity;
  }
  inline size_t size() const {
    return m_size;
  }
};
