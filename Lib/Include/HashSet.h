#pragma once

#include "CompactArray.h"
#include "CommonHashFunctions.h"
#include "Set.h"

class HashSetNode;

typedef ULONG (*HashFunction)(const void *);

class HashSetImpl : public AbstractSet {
  friend class HashSetIterator;
  friend class HashSetTable;
private:
  AbstractObjectManager *m_objectManager;
  AbstractComparator    *m_comparator;
  HashFunction           m_hash;
  HashSetTable          *m_table;
protected:
  virtual HashSetNode                *allocateNode()                const;
  virtual HashSetNode                *createNode(const void *key )  const;
  virtual HashSetNode                *cloneNode( HashSetNode *n  )  const;
  virtual void                        deleteNode(HashSetNode *n  )  const;
  virtual bool                        insertNode(HashSetNode *n  );
   // return key*
  void                               *select(RandomGenerator &rnd)  const override;
  void                                resize(size_t newCapacity);

  HashSetNode                        *findNode(const void    *key)  const;

  inline       AbstractObjectManager *getObjectManager()            const {
    return m_objectManager;
  }

//#define __HASHSET_CHECK_INTEGRITY

#if defined(__HASHSET_CHECK_INTEGRITY)
  void checktable(const TCHAR *label) const;
#endif
public:
  HashSetImpl(const AbstractObjectManager &objectManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity);
  AbstractCollection        *clone(bool cloneData)        const override;
  ~HashSetImpl()                                                override;
  void                       clear()                            override;
  // return number of elements
  size_t                     size()                       const override;
  bool                       add(     const void *key)          override;
  bool                       remove(  const void *key)          override;
  bool                       contains(const void *key)    const override;
  inline AbstractComparator *getComparator()              const override {
    return m_comparator;
  }
  inline HashFunction        getHashFunction()            const {
    return m_hash;
  }
  void                      *getMin()                     const override;
  void                      *getMax()                     const override;

  AbstractIterator          *getIterator()                const override;
  bool                       hasOrder()                   const override {
    return false;
  }

  CompactIntArray            getLength()                  const;
  // return capacity, NOT number of elements
  size_t                     getCapacity()                const;
  int                        getMaxChainLength()          const;
};

template <typename T> class HashSet : public Set<T> {
public:
  HashSet(ULONG (*hash)(const T &key), int (*comparator)(const T &key1, const T &key2), size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, FunctionComparator<T>(comparator), capacity))
  {
  }
  HashSet(ULONG (*hash)(const T &key), int (*comparator)(const T *key1, const T *key2), size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, FunctionComparator<T>(comparator), capacity))
  {
  }
  HashSet(ULONG (*hash)(const T &key), Comparator<T> &comparator, size_t capacity = 31)
    : Set<T>(new HashSetImpl(ObjectManager<T>(), (HashFunction)hash, comparator, capacity))
  {
  }
  HashSet<T> &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
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

class UShortHashSet : public HashSet<USHORT> {
public:
  UShortHashSet(size_t capacity=31) : HashSet<USHORT>(ushortHash, ushortHashCmp, capacity) {
  }
};

class IntHashSet : public HashSet<int> {
public:
  IntHashSet(size_t capacity=31) : HashSet<int>(intHash, intHashCmp, capacity) {
  }
};

class UIntHashSet : public HashSet<UINT> {
public:
  UIntHashSet(size_t capacity=31) : HashSet<UINT>(uintHash, uintHashCmp, capacity) {
  }
};

class LongHashSet : public HashSet<long> {
public:
  LongHashSet(size_t capacity=31) : HashSet<long>(longHash, longHashCmp, capacity) {
  }
};

class ULongHashSet : public HashSet<ULONG> {
public:
  ULongHashSet(size_t capacity=31) : HashSet<ULONG>(ulongHash, ulongHashCmp, capacity) {
  }
};

class Int64HashSet : public HashSet<INT64> {
public:
  Int64HashSet(size_t capacity=31) : HashSet<INT64>(int64Hash, int64HashCmp, capacity) {
  }
};

class UInt64HashSet : public HashSet<UINT64> {
public:
  UInt64HashSet(size_t capacity=31) : HashSet<UINT64>(uint64Hash, uint64HashCmp, capacity) {
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

template<typename T> class PointerHashSet : public HashSet<T> {
public:
  PointerHashSet(size_t capacity=31) : HashSet<T>(pointerHash, pointerHashCmp, capacity) {
  }
};
