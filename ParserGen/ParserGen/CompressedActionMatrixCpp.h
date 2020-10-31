#pragma once

#include <TreeMap.h>
#include "GrammarTables.h"

namespace ActionMatrix {

class SameReduceActionInfo {
private:
  const UINT m_prod;
  BitSet     m_termSet;
public:
  SameReduceActionInfo(UINT terminalCount, UINT prod, UINT token)
    : m_prod(prod)
    , m_termSet(terminalCount)
  {
    m_termSet.add(token);
  }
  inline UINT getProduction() const {
    return m_prod;
  }
  inline const BitSet &getTerminalSet() const {
    return m_termSet;
  }
  inline void addTerminal(UINT token) {
    m_termSet.add(token);
  }
};

typedef enum {
  ONEITEMCOMPRESSION
 ,REDUCEBYSAMEPRODCOMPRESSION
 ,SPLITCOMPRESSION
 ,UNCOMPRESSED
} CompressionMethod;

class StateActionInfo {
private:
  const UINT                  m_state;
  const ActionArray          &m_actionArray;
  // List of different reduceActions;
  Array<SameReduceActionInfo> m_sameReductionArray;
  ActionArray                 m_shiftActionArray;
public:
  StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray);

  inline UINT getDifferentActionCount() const {
    return (UINT)(m_sameReductionArray.size() + m_shiftActionArray.size());
  }
  inline UINT getState() const {
    return m_state;
  }
  inline const ActionArray &getActions() const {
    return m_actionArray;
  }
  inline const Array<SameReduceActionInfo> &getReduceActionArray() const {
    return m_sameReductionArray;
  }
  inline const ActionArray &getShiftActionArray() const {
    return m_shiftActionArray;
  }
  CompressionMethod getCompressionMethod() const;

  String toString() const {
    return format(_T("%2u sa, %2u ra"), (UINT)m_shiftActionArray.size(), (UINT)m_sameReductionArray.size());
  }
};

class ArrayIndex {
public:
  const UINT m_arrayIndex;   // offset into array
  int        m_commentIndex; // counter
  ArrayIndex(UINT arrayIndex) : m_arrayIndex(arrayIndex), m_commentIndex(-1) {
  }
};

inline int arrayIndexCmp(const ArrayIndex &i1, const ArrayIndex &i2) {
  return (int)i1.m_arrayIndex - (int)i2.m_arrayIndex;
}

class IndexMapValue : public ArrayIndex {
public:
  BitSet m_stateSet;
  IndexMapValue(size_t stateCount, UINT state0, UINT arrayIndex) : ArrayIndex(arrayIndex), m_stateSet(stateCount) {
    addState(state0);
  }
  void addState(UINT state) {
    m_stateSet.add(state);
  }
  String getComment() const {
    const size_t n = m_stateSet.size();
    return format(_T("Used by state%c %s"), (n == 1) ? ' ' : 's', m_stateSet.toString().cstr());
  }
};

template<typename Key> class IndexArrayEntry : public IndexMapValue {
public:
  Key m_key;
  IndexArrayEntry(const Entry<Key, IndexMapValue> &e) : IndexMapValue(e.getValue()), m_key(e.getKey()) {
  }
};

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
  void sortByIndex() {
    sort(IndexComparator<Key>());
  }
  UINT getElementCount(bool addArraySize) const {
    UINT elemCount = 0;
    for(ConstIterator<IndexArrayEntry<Key> > it = getIterator(); it.hasNext();) {
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
    ((IndexMapValue&)value).m_commentIndex = (UINT)size();
    return __super::put(key, value);
  }
  inline UINT getCount() const {
    return (UINT)size();
  }
  IndexArray<Key> getEntryArray() const {
    IndexArray<Key> a(size());
    for(ConstIterator<Entry<Key, IndexMapValue> > it = getIterator(); it.hasNext();) {
      a.add(it.next());
    }
    a.sortByIndex();
    return a;
  }
};

typedef BitSet SymbolSet;

inline int symbolSetCmp( const SymbolSet &k1, const SymbolSet &k2) {
  return bitSetCmp(k1, k2);
}

class SymbolSetIndexMap : public IndexMap<SymbolSet> {
public:
  SymbolSetIndexMap() : IndexMap<SymbolSet>(symbolSetCmp) {
  }
};

typedef IndexArray<SymbolSet> SymbolSetIndexArray;

extern int rawActionArrayCmp(const RawActionArray &a1, const RawActionArray &a2);

class RawActionArrayIndexMap : public IndexMap<RawActionArray> {
public:
  RawActionArrayIndexMap() : IndexMap<RawActionArray>(rawActionArrayCmp) {
  }
};

typedef IndexArray<RawActionArray> RawActionArrayIndexArray;

class CompressedActionMatrix {
private :
  const GrammarTables          &m_tables;
  const UINT                    m_stateCount;
  const UINT                    m_terminalCount;
  const UINT                    m_laSetSizeInBytes;
  UINT                          m_currentTermListSize;
  UINT                          m_currentActionListSize;
  UINT                          m_currentLASetArraySize;

  Array<StateActionInfo>        m_stateInfoArray;
  SymbolSetIndexMap             m_laSetMap;
  SymbolSetIndexMap             m_termListMap;
  RawActionArrayIndexMap        m_raaMap;
  mutable StringArray           m_defines;
  void addACdefine(UINT state, const String &value, const String &comment);
  void doOneItemState(         const StateActionInfo &stateInfo);
  void doReduceBySameProdState(const StateActionInfo &stateInfo);
  void doUncompressedState(    const StateActionInfo &stateInfo);

  ByteCount printTermAndActionList(   MarginFile &output) const;
  ByteCount printTermSetTable(        MarginFile &output) const;
  ByteCount printDefinesAndActionCode(MarginFile &output) const;
public:
  CompressedActionMatrix(const GrammarTables &tables);
  void generateCompressedForm();
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrix
