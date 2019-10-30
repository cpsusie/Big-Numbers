#pragma once

#include "TreeSet.h"
#include "Map.h"

class TreeMapNode : public TreeSetNode, public AbstractEntry {
private:
  void *m_value;
public:
  const void *key() const {
    return TreeSetNode::key();
  }
  void *value() {
    return m_value;
  }
  const void *value() const {
    return m_value;
  }
  friend class TreeMapImpl;
};

class TreeMapImpl : public TreeSetImpl, public AbstractMap {
private:
  AbstractObjectManager *m_dataManager;
protected:
  virtual TreeSetNode *allocateNode() const;
  virtual TreeSetNode *createNode(const void *key, const void *value) const;
  virtual TreeSetNode *cloneNode(TreeSetNode *n) const;
  virtual void deleteNode(TreeSetNode *n) const;
  virtual void swapContent(TreeSetNode *n1, TreeSetNode *n2);
public:
  TreeMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, const AbstractComparator &comparator);
  virtual ~TreeMapImpl();
  bool put(const void *key, void *value);
  bool put(const void *key, const void *value);
  bool remove(const void *key);
  void *get(const void *key);
  const void *get(const void *key) const;
  AbstractEntry *selectEntry(RandomGenerator &rnd) const;
  const AbstractEntry *getMinEntry() const;
  const AbstractEntry *getMaxEntry() const;
  size_t size() const {
    return TreeSetImpl::size();
  }
  void clear() {
    TreeSetImpl::clear();
  }
  bool hasOrder() const {
    return TreeSetImpl::hasOrder();
  }
  AbstractComparator *getComparator() {
    return TreeSetImpl::getComparator();
  }
  const AbstractComparator *getComparator() const {
    return TreeSetImpl::getComparator();
  }
  AbstractCollection *clone(bool cloneData) const {
    throwUnsupportedOperationException(__TFUNCTION__);
    return NULL;
  }
  AbstractMap *cloneMap(bool cloneData) const;
  AbstractIterator *getIterator();
  AbstractIterator *getKeyIterator() {
    return TreeSetImpl::getIterator();
  }
};

template <class K, class V> class TreeMap : public Map<K, V> {
public:
  TreeMap(int (*comparator)(const K &key1, const K &key2))
    : Map<K, V>(new TreeMapImpl(ObjectManager<K>(), ObjectManager<V>(), FunctionComparator<K>(comparator)))
  {
  }
  TreeMap(int (*comparator)(const K *key1, const K *key2))
    : Map<K, V>(new TreeMapImpl(ObjectManager<K>(), ObjectManager<V>(), FunctionComparator<K>(comparator)))
  {
  }
  TreeMap(Comparator<K> &comparator)
    : Map<K, V>(new TreeMapImpl(ObjectManager<K>(), ObjectManager<V>(), comparator))
  {
  }
};

template <class T> class ShortTreeMap : public TreeMap<short, T>  {
public:
  ShortTreeMap() : TreeMap<short, T>(shortHashCmp) {
  }
};

template <class T> class UShortTreeMap : public TreeMap<USHORT, T>  {
public:
  UShortTreeMap() : TreeMap<USHORT, T>(ushortHashCmp) {
  }
};

template <class T> class IntTreeMap : public TreeMap<int, T>    {
public:
  IntTreeMap() : TreeMap<int, T>(intHashCmp) {
  }
};

template <class T> class UIntTreeMap : public TreeMap<UINT, T>    {
public:
  UIntTreeMap() : TreeMap<UINT, T>(uintHashCmp) {
  }
};

template <class T> class LongTreeMap : public TreeMap<long, T> {
public:
  LongTreeMap() : TreeMap<long, T>(longHashCmp) {
  }
};

template <class T> class ULongTreeMap : public TreeMap<ULONG, T> {
public:
  ULongTreeMap() : TreeMap<ULONG, T>(ulongHashCmp) {
  }
};

template <class T> class Int64TreeMap       : public TreeMap<INT64, T> {
public:
  Int64TreeMap()   : TreeMap<INT64, T>(int64HashCmp) {
  }
};

template <class T> class UInt64TreeMap      : public TreeMap<UINT64, T> {
public:
  UInt64TreeMap()   : TreeMap<UINT64, T>(uint64HashCmp) {
  }
};

template <class T> class FloatTreeMap      : public TreeMap<float, T> {
public:
  FloatTreeMap()  : TreeMap<float, T>(floatHashCmp) {
  }
};

template <class T> class DoubleTreeMap      : public TreeMap<double, T> {
public:
  DoubleTreeMap()  : TreeMap<double, T>(doubleHashCmp) {
  }
};

template <class T> class StrTreeMap : public TreeMap<const TCHAR*, T> {
public:
  StrTreeMap() : TreeMap<const TCHAR*, T>(strHashCmp) {
  }
};

template <class T> class StrITreeMap : public TreeMap<const TCHAR*, T> {
public:
  StrITreeMap() : TreeMap<const TCHAR*, T>(striHashCmp) {
  }
};

template <class T> class StringTreeMap : public TreeMap<String, T> {
public:
  StringTreeMap() : TreeMap<String, T>(stringHashCmp) {
  }
};

template <class T> class StringITreeMap : public TreeMap<String, T> {
public:
  StringITreeMap() : TreeMap<String, T>(stringiHashCmp) {
  }
};
