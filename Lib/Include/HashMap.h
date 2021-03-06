#pragma once

#include "CompactArray.h"
#include "HashSet.h"
#include "Map.h"

class HashMapNode;

class HashMapImpl : public HashSetImpl, public AbstractMap {
private:
  AbstractObjectManager *m_dataManager;

  AbstractObjectManager *getKeyManager()                                const {
    return HashSetImpl::getObjectManager();
  }
  AbstractObjectManager *getDataManager()                               const {
    return m_dataManager;
  }
protected:
  HashSetNode           *allocateNode()                                 const override;
  virtual HashMapNode   *createNode(const void *key, const void *value) const;
  HashSetNode           *cloneNode( HashSetNode *n)                     const override;
  void                   deleteNode(HashSetNode *n)                     const override;
public:
  HashMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity);
  // throw unsupported exception
  AbstractCollection    *clone(   bool cloneData)                       const override;
  AbstractMap           *cloneMap(bool cloneData)                       const override;
  ~HashMapImpl()                                                              override;
  void                   clear()                                              override;
  size_t                 size()                                         const override;
  bool                   put(   const void *key, const void *value)           override;
  void                  *get(   const void *key)                        const override;
  bool                   remove(const void *key)                              override;
  AbstractEntry         *selectEntry(RandomGenerator &rnd)              const override;
  AbstractEntry         *getMinEntry()                                  const override;
  AbstractEntry         *getMaxEntry()                                  const override;
  AbstractComparator    *getComparator()                                const override;
  bool                   hasOrder()                                     const override;
  AbstractIterator      *getIterator()                                  const override;
  AbstractIterator      *getKeyIterator()                               const override;
};

template <typename K, typename V> class HashMap : public Map<K, V> {
public:
  HashMap(ULONG (*hash)(const K &key), int (*comparator)(const K &key1, const K &key2), size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, FunctionComparator<K>(comparator), capacity))
  {
  }
  HashMap(ULONG (*hash)(const K &key), int (*comparator)(const K *key1, const K *key2), size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, FunctionComparator<K>(comparator), capacity))
  {
  }
  HashMap(ULONG (*hash)(const K &key), Comparator<K> &comparator, size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, comparator, capacity))
  {
  }
  HashFunction     getHashFunction()   const {
    return ((HashMapImpl*)m_map)->getHashFunction();
  }
  CompactUIntArray getLength()         const {
    return ((HashMapImpl*)m_map)->getLength();
  }
  HashMap         &setCapacity(size_t capacity) {
    ((HashMapImpl*)m_map)->setCapacity(capacity);
    return *this;
  }
  size_t           getCapacity()       const {
    return ((HashMapImpl*)m_map)->getCapacity();
  }
  UINT             getMaxChainLength() const {
    return ((HashMapImpl*)m_map)->getMaxChainLength();
  }
};

template <typename T> class ShortHashMap      : public HashMap<short, T> {
public:
  ShortHashMap(size_t capacity=31)  : HashMap<short, T>(shortHash, shortHashCmp, capacity) {
  }
};

template <typename T> class UShortHashMap     : public HashMap<USHORT, T> {
public:
  UShortHashMap(size_t capacity=31) : HashMap<USHORT, T>(ushortHash, ushortHashCmp, capacity) {
  }
};

template <typename T> class IntHashMap        : public HashMap<int, T> {
public:
  IntHashMap(size_t capacity=31)    : HashMap<int, T>(intHash, intHashCmp, capacity) {
  }
};

template <typename T> class UIntHashMap       : public HashMap<UINT, T> {
public:
  UIntHashMap(size_t capacity=31)   : HashMap<UINT, T>(uintHash, uintHashCmp, capacity) {
  }
};

template <typename T> class LongHashMap       : public HashMap<long, T> {
public:
  LongHashMap(size_t capacity=31)   : HashMap<long, T>(longHash, longHashCmp, capacity) {
  }
};

template <typename T> class ULongHashMap      : public HashMap<ULONG, T> {
public:
  ULongHashMap(size_t capacity=31)  : HashMap<ULONG, T>(ulongHash, ulongHashCmp, capacity) {
  }
};

template <typename T> class Int64HashMap      : public HashMap<INT64, T> {
public:
  Int64HashMap(size_t capacity=31)  : HashMap<INT64, T>(int64Hash, int64HashCmp, capacity) {
  }
};

template <typename T> class UInt64HashMap     : public HashMap<UINT64, T> {
public:
  UInt64HashMap(size_t capacity=31) : HashMap<UINT64, T>(uint64Hash, uint64HashCmp, capacity) {
  }
};

template <typename T> class StrHashMap        : public HashMap<const TCHAR*, T> {
public:
  StrHashMap(size_t capacity=31)    : HashMap<const TCHAR*, T>(strHash, strHashCmp, capacity) {
  }
};

template <typename T> class StrIHashMap       : public HashMap<const TCHAR*, T> {
public:
  StrIHashMap(size_t capacity=31)   : HashMap<const TCHAR*, T>(striHash, striHashCmp, capacity) {
  }
};

template <typename T> class StringHashMap     : public HashMap<String, T> {
public:
  StringHashMap(size_t capacity=31) : HashMap<String, T>(stringHash, stringHashCmp, capacity) {
  }
};

template <typename T> class StringIHashMap    : public HashMap<String, T> {
public:
  StringIHashMap(size_t capacity=31) : HashMap<String, T>(stringiHash, stringiHashCmp, capacity) {
  }
};

template <typename K, typename V> class PointerHashMap : public HashMap<K, V> {
public:
  PointerHashMap(size_t capacity=31) : HashMap<K, V>(pointerHash, pointerHashCmp, capacity) {
  }
};
