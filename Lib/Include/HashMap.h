#pragma once

#include "Array.h"
#include "CompactArray.h"
#include "MyString.h"
#include "HashSet.h"
#include "Map.h"

class HashMapNode : public HashSetNode, public AbstractEntry {
private:
  void *m_value;
public:
  const void *key() const {
    return HashSetNode::key();
  }
  void *value() { 
    return m_value;
  }
  const void *value() const {
    return m_value;
  }
  friend class HashMapImpl;
};

class HashMapImpl : public HashSetImpl, public AbstractMap {
private:
  DECLARECLASSNAME;
  AbstractObjectManager *m_dataManager;

  AbstractObjectManager *getKeyManager() {
    return HashSetImpl::getObjectManager();
  }
  const AbstractObjectManager *getKeyManager() const {
    return HashSetImpl::getObjectManager();
  }
  AbstractObjectManager *getDataManager() {
    return m_dataManager;
  }
  const AbstractObjectManager *getDataManager() const {
    return m_dataManager;
  }
protected:
  virtual HashSetNode *allocateNode() const;
  virtual HashMapNode *createNode(const void *key, const void *value) const;
  virtual HashSetNode *cloneNode(HashSetNode *n) const;
  virtual void deleteNode(HashSetNode *n) const;
public:
  HashMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, HashFunction hash, const AbstractComparator &comparator, size_t capacity);
  virtual ~HashMapImpl();
  bool put(const void *key, void *value);
  bool put(const void *key, const void *value);
  bool remove(const void *key);
  void *get(const void *key);
  const void *get(const void *key) const;
  AbstractEntry *selectEntry() const;
  const AbstractEntry *getMinEntry() const;
  const AbstractEntry *getMaxEntry() const;
  size_t size() const;
  void clear();
  bool hasOrder() const {
    return HashSetImpl::hasOrder();
  }
  AbstractComparator *getComparator() {
    return HashSetImpl::getComparator();
  }
  const AbstractComparator *getComparator() const {
    return HashSetImpl::getComparator();
  }
  AbstractCollection *clone(bool cloneData) const {
    throwException("HashMap::clone::Unsupported operation.");
    return NULL;
  }
  AbstractMap *cloneMap(bool cloneData) const;
  AbstractIterator *getIterator();
  AbstractIterator *getKeyIterator() {
    return HashSetImpl::getIterator();
  }
};

template <class K, class V> class HashMap : public Map<K, V> {
public:
  HashMap(unsigned long (*hash)(const K &key), int (*comparator)(const K &key1, const K &key2), size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, FunctionComparator<K>(comparator), capacity))
  {
  }
  HashMap(unsigned long (*hash)(const K &key), int (*comparator)(const K *key1, const K *key2), size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, FunctionComparator<K>(comparator), capacity))
  {
  }
  HashMap(unsigned long (*hash)(const K &key), Comparator<K> &comparator, size_t capacity = 31)
    : Map<K, V>(new HashMapImpl(ObjectManager<K>(), ObjectManager<V>(), (HashFunction)hash, comparator, capacity))
  {
  }
  HashFunction getHashFunction() {
    return ((HashMapImpl*)m_map)->getHashFunction();
  }
  CompactIntArray getLength() const {
    return ((HashMapImpl*)m_map)->getLength();
  }
  size_t getCapacity() const {
    return ((HashMapImpl*)m_map)->getCapacity();
  }
  int getMaxChainLength() const {
    return ((HashMapImpl*)m_map)->getMaxChainLength();
  }
};

template <class T> class ShortHashMap      : public HashMap<short, T> {
public:
  ShortHashMap(size_t capacity=31)  : HashMap<short, T>(shortHash, shortHashCmp, capacity) {
  }
};

template <class T> class UShortHashMap     : public HashMap<unsigned short, T> {
public:
  UShortHashMap(size_t capacity=31) : HashMap<unsigned short, T>(ushortHash, ushortHashCmp, capacity) {
  }
};

template <class T> class IntHashMap        : public HashMap<int, T> {
public:
  IntHashMap(size_t capacity=31)    : HashMap<int, T>(intHash, intHashCmp, capacity) {
  }
};

template <class T> class UIntHashMap       : public HashMap<unsigned int, T> {
public:
  UIntHashMap(size_t capacity=31)   : HashMap<unsigned int, T>(uintHash, uintHashCmp, capacity) {
  }
};

template <class T> class LongHashMap       : public HashMap<long, T> {
public:
  LongHashMap(size_t capacity=31)   : HashMap<long, T>(longHash, longHashCmp, capacity) {
  }
};

template <class T> class ULongHashMap      : public HashMap<unsigned long, T> {
public:
  ULongHashMap(size_t capacity=31)  : HashMap<unsigned long, T>(ulongHash, ulongHashCmp, capacity) {
  }
};

template <class T> class Int64HashMap      : public HashMap<__int64, T> {
public:
  Int64HashMap(size_t capacity=31)  : HashMap<__int64, T>(int64Hash, int64HashCmp, capacity) {
  }
};

template <class T> class UInt64HashMap     : public HashMap<unsigned __int64, T> {
public:
  UInt64HashMap(size_t capacity=31) : HashMap<unsigned __int64, T>(uint64Hash, uint64HashCmp, capacity) {
  }
};

template <class T> class FloatHashMap      : public HashMap<float, T> {
public:
  FloatHashMap(size_t capacity=31)  : HashMap<float, T>(floatHash, floatHashCmp, capacity) {
  }
};

template <class T> class DoubleHashMap      : public HashMap<double, T> {
public:
  DoubleHashMap(size_t capacity=31)  : HashMap<double, T>(doubleHash, doubleHashCmp, capacity) {
  }
};

template <class T> class StrHashMap        : public HashMap<const TCHAR*, T> {
public:
  StrHashMap(size_t capacity=31)    : HashMap<const TCHAR*, T>(strHash, strHashCmp, capacity) {
  }
};

template <class T> class StrIHashMap       : public HashMap<const TCHAR*, T> {
public:
  StrIHashMap(size_t capacity=31)   : HashMap<const TCHAR*, T>(striHash, striHashCmp, capacity) {
  }
};

template <class T> class StringHashMap     : public HashMap<String, T> {
public:
  StringHashMap(size_t capacity=31) : HashMap<String, T>(stringHash, stringHashCmp, capacity) {
  }
};

template <class T> class StringIHashMap    : public HashMap<String, T> {
public:
  StringIHashMap(size_t capacity=31): HashMap<String, T>(stringiHash, stringiHashCmp, capacity) {
  }
};
