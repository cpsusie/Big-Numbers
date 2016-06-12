#pragma once

#include "MyUtil.h"
#include <CompactArray.h>
#include "Set.h"

class HashSetNode : public AbstractKey {
private:
  void        *m_key;
  HashSetNode *m_next, **m_prev;        // next/previous node vith same hash-index
  HashSetNode *m_nextLink, *m_prevLink; // for iterator
public:
  const void *key() const {
    return m_key;
  }
  HashSetNode();
  friend class HashSetImpl;
  friend class HashSetTable;
  friend class HashSetIterator;
};

typedef unsigned long (*HashFunction)(const void *);

class HashSetTable {
private:
  DECLARECLASSNAME;
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
  const AbstractKey *select() const;
  size_t getCapacity() const {
    return m_capacity;
  }
  size_t size() const {
    return m_size;
  }
  friend class HashSetImpl;
  friend class HashSetIterator;
};

class HashSetImpl : public AbstractSet {
private:
  DECLARECLASSNAME;
  AbstractObjectManager *m_objectManager;
  AbstractComparator    *m_comparator;
  HashFunction           m_hash;
  HashSetTable          *m_table;
protected:
  virtual HashSetNode *allocateNode() const;
  virtual HashSetNode *createNode(const void *key) const;
  virtual HashSetNode *cloneNode(HashSetNode *n) const;
  virtual void deleteNode(HashSetNode *n) const;
  virtual bool insertNode(HashSetNode *n);
  const void  *select() const; // return key*
  void *select();
  void resize(size_t newCapacity);

  HashSetNode *findNode(const void *key);
  const HashSetNode *findNode(const void *key) const;

  AbstractObjectManager *getObjectManager() {
    return m_objectManager;
  }
  const AbstractObjectManager *getObjectManager() const {
    return m_objectManager;
  }
//  void checktable(const char *label) const;
public:
  HashSetImpl(const AbstractObjectManager &objectManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity);
  virtual ~HashSetImpl();
  HashFunction getHashFunction() const {
    return m_hash;
  }
  bool add(const void *key);
  bool remove(const void *key);
  bool contains(const void *key) const;
  size_t size() const {  // return number of elements
    return m_table->size();
  }
  void clear() {
    m_table->clear();
  }
  bool hasOrder() const {
    return false;
  }
  AbstractComparator *getComparator() {
    return m_comparator;
  }
  const AbstractComparator *getComparator() const {
    return m_comparator;
  }
  const void *getMin() const;
  const void *getMax() const;

  AbstractCollection *clone(bool cloneData) const;
  AbstractIterator *getIterator();

  CompactIntArray getLength() const {
    return m_table->getLength();
  }
  size_t getCapacity() const {  // return capacity, NOT number of elements
    return m_table->getCapacity();
  }
  int getMaxChainLength() const {
    return m_table->getMaxChainLength();
  }

  friend class HashSetIterator;
  friend class HashSetTable;    
};

class HashSetIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  HashSetImpl        *m_set;
  HashSetNode        *m_next, *m_current;
  size_t              m_updateCount;

  void checkUpdateCount() const;
protected:
  HashSetImpl &getSet() {
    return *m_set;
  }
  HashSetNode *nextNode();
public:
  HashSetIterator(HashSetImpl &set);
  AbstractIterator *clone();
  virtual void *next() {
    return (void*)(nextNode()->key());
  }
  bool hasNext() const;
  void remove();
};

template <class T> class HashSet : public Set<T> {
public:
  HashSet(unsigned long (*hash)(const T &key), int (*comparator)(const T &key1, const T &key2), size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, FunctionComparator<T>(comparator), capacity))
  {
  }
  HashSet(unsigned long (*hash)(const T &key), int (*comparator)(const T *key1, const T *key2), size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, FunctionComparator<T>(comparator), capacity))
  {
  }
  HashSet(unsigned long (*hash)(const T &key), Comparator<T> &comparator, size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, comparator, capacity))
  {
  }
  HashSet<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  HashFunction getHashFunction() {
    return ((HashSetImpl*)m_collection)->getHashFunction();
  }

  CompactIntArray getLength() const {
    return ((HashSetImpl*)m_collection)->getLength();
  }

  size_t getCapacity() const {
    return ((HashSetImpl*)m_collection)->getCapacity();
  }

  int getMaxChainLength() const {
    return ((HashSetImpl*)m_collection)->getMaxChainLength();
  }
};

class ShortHashSet : public HashSet<short> {
public:
  ShortHashSet(size_t capacity=31) : HashSet<short>(shortHash, shortHashCmp, capacity) {
  }
};

class UShortHashSet : public HashSet<unsigned short> {
public:
  UShortHashSet(size_t capacity=31) : HashSet<unsigned short>(ushortHash, ushortHashCmp, capacity) {
  }
};

class IntHashSet : public HashSet<int> {
public:
  IntHashSet(size_t capacity=31) : HashSet<int>(intHash, intHashCmp, capacity) {
  }
};

class UIntHashSet : public HashSet<unsigned int> {
public:
  UIntHashSet(size_t capacity=31) : HashSet<unsigned int>(uintHash, uintHashCmp, capacity) {
  }
};

class LongHashSet : public HashSet<long> {
public:
  LongHashSet(size_t capacity=31) : HashSet<long>(longHash, longHashCmp, capacity) {
  }
};

class ULongHashSet : public HashSet<unsigned long> {
public:
  ULongHashSet(size_t capacity=31) : HashSet<unsigned long>(ulongHash, ulongHashCmp, capacity) {
  }
};

class Int64HashSet : public HashSet<__int64> {
public:
  Int64HashSet(size_t capacity=31) : HashSet<__int64>(int64Hash, int64HashCmp, capacity) {
  }
};

class UInt64HashSet : public HashSet<unsigned __int64> {
public:
  UInt64HashSet(size_t capacity=31) : HashSet<unsigned __int64>(uint64Hash, uint64HashCmp, capacity) {
  }
};

class FloatHashSet : public HashSet<float> {
public:
  FloatHashSet(size_t capacity=31)  : HashSet<float>(floatHash, floatHashCmp, capacity) {
  }
};

class DoubleHashSet : public HashSet<double> {
public:
  DoubleHashSet(size_t capacity=31)  : HashSet<double>(doubleHash, doubleHashCmp, capacity) {
  }
};

class StrHashSet : public HashSet<const TCHAR*> {
public:
  StrHashSet(size_t capacity=31) : HashSet<const TCHAR*>(strHash, strHashCmp, capacity) {
  }
};

class StrIHashSet : public HashSet<const TCHAR*> {
public:
  StrIHashSet(size_t capacity=31) : HashSet<const TCHAR*>(striHash, striHashCmp, capacity) {
  }
};

class StringHashSet : public HashSet<String> {
public:
  StringHashSet(size_t capacity=31) : HashSet<String>(stringHash, stringHashCmp, capacity) {
  }
};

class StringIHashSet : public HashSet<String> {
public:
  StringIHashSet(size_t capacity=31) : HashSet<String>(stringiHash, stringiHashCmp, capacity) {
  }
};
