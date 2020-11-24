#pragma once

#include <TreeMap.h>
#include "Grammar.h"

class ArrayIndex {
public:
  const UINT m_arrayIndex;   // offset into array
  int        m_commentIndex; // used for index of element in comment
  ArrayIndex(UINT arrayIndex) : m_arrayIndex(arrayIndex), m_commentIndex(-1) {
  }
};

class IndexMapValue : public ArrayIndex {
private:
  UsedByBitSet m_usedBySet;
public:
  IndexMapValue(const BitSetParam &usedByParam, UINT usedByV0, UINT arrayIndex) : ArrayIndex(arrayIndex), m_usedBySet(usedByParam) {
    addUsedByValue(usedByV0);
  }
  void addUsedByValue(UINT v) {
    m_usedBySet.add(v);
  }
  String getUsedByComment() const {
    return m_usedBySet.toString();
  }
};

template<typename Key> class IndexArrayEntry : public IndexMapValue {
public:
  Key m_key;
  IndexArrayEntry(const Entry<Key, IndexMapValue> &e) : IndexMapValue(e.getValue()), m_key(e.getKey()) {
  }
};

inline int arrayIndexCmp(const ArrayIndex &i1, const ArrayIndex &i2) {
  return (int)i1.m_arrayIndex - (int)i2.m_arrayIndex;
}

template<typename Key> class IndexComparator : public Comparator<IndexArrayEntry<Key> > {
public:
  int compare(const IndexArrayEntry<Key> &kv1, const IndexArrayEntry<Key> &kv2) override {
    return arrayIndexCmp(kv1, kv2);
  }
  AbstractComparator *clone() const override {
    return new IndexComparator<Key>;
  }
};

template<typename Key> class IndexArray : public Array<IndexArrayEntry<Key> > {
public:
  explicit IndexArray(size_t capacity) : Array<IndexArrayEntry<Key> >(capacity) {
  }
  IndexArray &sortByIndex() {
    sort(IndexComparator<Key>());
    return *this;
  }
  UINT getElementCount(bool addArraySize) const {
    UINT elemCount = 0;
    for(auto it = getIterator(); it.hasNext();) {
      const IndexArrayEntry<Key> &e = it.next();
      elemCount += (UINT)e.m_key.size();
    }
    return elemCount + (addArraySize ? (UINT)size() : 0);
  }
};

template<typename Key> class IndexMap : public TreeMap<Key, IndexMapValue> {
public:
  IndexMap(int (*cmp)(const Key &, const Key &)) : TreeMap<Key, IndexMapValue>(cmp) {
  }
  bool put(const Key &key, const IndexMapValue &value) override {
    ((IndexMapValue &)value).m_commentIndex = getCount();
    return __super::put(key, value);
  }
  inline UINT getCount() const {
    return (UINT)size();
  }
  IndexArray<Key> getEntryArray() const {
    IndexArray<Key> a(size());
    for(auto it = getIterator(); it.hasNext();) {
      a.add(it.next());
    }
    return a.sortByIndex();
  }
};

class OptimizedBitSetPermutation : public CompactUIntArray {
private:
  UINT m_newCapacity;
public:
  OptimizedBitSetPermutation() : m_newCapacity(0) {
  }
  OptimizedBitSetPermutation(const BitSet &bitSet);
  inline UINT getOldCapacity() const {
    return (UINT)size();
  }
  inline UINT getNewCapacity() const {
    return m_newCapacity;
  }
  ByteCount getSavedBytesByOptimizedBitSets(UINT bitSetCount) const;

};
