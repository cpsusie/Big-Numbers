#pragma once

#include <CompactArray.h>

class RandomGenerator;
class AbstractKey;
class HashSetNode;

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
  UINT chainLength(size_t index) const;
  UINT getMaxChainLength()       const;
  CompactUIntArray getLength()   const;
public:
  HashSetTable(const HashSetImpl &owner, size_t capacity);
  ~HashSetTable();
  void          clear();
  inline size_t size() const {
    return m_size;
  }
  void          insert(size_t index, HashSetNode *n);
  void          remove(HashSetNode *n);
  AbstractKey  *select(RandomGenerator &rnd) const;
  inline size_t getCapacity() const {
    return m_capacity;
  }
};
