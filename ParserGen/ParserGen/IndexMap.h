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

inline int termSetCmp(const TermSet &s1, const TermSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class TermSetIndexMap : public IndexMap<TermSet> {
public:
  TermSetIndexMap() : IndexMap(termSetCmp) {
  }
};

typedef IndexArray<TermSet> TermSetIndexArray;

class ActionArrayIndexMap : public IndexMap<ActionArray> {
public:
  ActionArrayIndexMap() : IndexMap(actionArrayCmp) {
  }
};

typedef IndexArray<ActionArray> ActionArrayIndexArray;

inline int stateSetCmp(const StateSet &s1, const StateSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class StateSetIndexMap : public IndexMap<StateSet> {
public:
  StateSetIndexMap() : IndexMap(stateSetCmp) {
  }
};

typedef IndexArray<StateSet> StateSetIndexArray;

class StateArrayIndexMap : public IndexMap<StateArray> {
public:
  StateArrayIndexMap() : IndexMap(stateArrayCmp) {
  }
};

typedef IndexArray<StateArray> StateArrayIndexArray;

class UIntPermutation : public CompactUIntArray {
protected:
  UIntPermutation(size_t capacity);
  UIntPermutation();
  // Make an array, with size and capacity = capacity, all elements set to -1
  void init(UINT capacity);
public:
  inline UINT getOldCapacity() const {
    return (UINT)size();
  }
  // Find index of the specified element. throw exception if invalid argument or not found
  UINT findIndex(UINT v) const;
  // Check that all elements are distinct, and in range [0..size-1]
  // throw exception, if not
  virtual void validate() const;
};

// find permutation of elements in this, so that all 1-bits will be put first in bitset
class OptimizedBitSetPermutation : public UIntPermutation {
private:
  UINT m_newCapacity;
  // Return this
  OptimizedBitSetPermutation &addSet(const BitSet &s, UINT &v);
  OptimizedBitSetPermutation &setNewCapacity(UINT v);
public:
  OptimizedBitSetPermutation() : m_newCapacity(0) {
  }
  OptimizedBitSetPermutation(const BitSet &bitSet);
  inline UINT getNewCapacity() const {
    return m_newCapacity;
  }
  ByteCount getSavedBytesByOptimizedBitSets(UINT bitSetCount) const;
  void validate() const override;
};

class OptimizedBitSetPermutation2 : public UIntPermutation {
private:
  BitSetInterval m_interval[2];
  BitSetInterval &getInterval(BYTE index);
  // Return this
  OptimizedBitSetPermutation2 &addSet(const BitSet &s, UINT &v             );
  // Return this
  OptimizedBitSetPermutation2 &setStart(   BYTE index, UINT start          );
  // Return this
  OptimizedBitSetPermutation2 &setEnd(     BYTE index, UINT end            );
  // Return this
  OptimizedBitSetPermutation2 &setInterval(BYTE index, UINT start, UINT end);
public:
  OptimizedBitSetPermutation2() {
  }
  // Find permutation of elements.
  // Assume A.capacity == B.capacity
  // First find S0 = A - B, S1 = A & B, S2 = B - A, S3 = not(A | B). notice: S0..3 are all mutually disjoint, and union(S0..3) == {0,1,...capacity-1}
  // If S0 is empty, put elements from S1, then S2,S4
  // If S2 is empty, put elements from S1, then S0,S4
  // If S3 is empty, no permutation is done because no space can be saved...all bits are used somewhere
  // If all are non-empty, put elements S0 first, then elements S1,S2,S3
  OptimizedBitSetPermutation2(const BitSet &A, const BitSet &B);
  inline const BitSetInterval &getInterval(BYTE index) const {
    assert(index < ARRAYSIZE(m_interval));
    return m_interval[index];
  }
  inline UINT getCapacity(BYTE index) const {
    return getInterval(index).getCapacity();
  }
  ByteCount getSavedBytesByOptimizedBitSets(UINT Acount, UINT Bcount) const;
  void validate() const override;
};
