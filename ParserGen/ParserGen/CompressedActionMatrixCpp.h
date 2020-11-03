#pragma once

#include <TreeMap.h>
#include "GrammarTables.h"
#include "StateActionInfoArray.h"

class ArrayIndex {
public:
  const UINT m_arrayIndex;   // offset into array
  int        m_commentIndex; // used for index of element in comment
  ArrayIndex(UINT arrayIndex) : m_arrayIndex(arrayIndex), m_commentIndex(-1) {
  }
};

class IndexMapValue : public ArrayIndex {
private:
  BitSet m_stateSet;
public:
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
    ((IndexMapValue &)value).m_commentIndex = getCount();
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
    return a.sortByIndex();
  }
};

class SymbolSetIndexMap : public IndexMap<SymbolSet> {
public:
  SymbolSetIndexMap() : IndexMap<SymbolSet>(symbolSetCmp) {
  }
};

typedef IndexArray<SymbolSet> SymbolSetIndexArray;

// --------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------

namespace ActionMatrix {

extern int rawActionArrayCmp(const RawActionArray &a1, const RawActionArray &a2);

class RawActionArrayIndexMap : public IndexMap<RawActionArray> {
public:
  RawActionArrayIndexMap() : IndexMap<RawActionArray>(rawActionArrayCmp) {
  }
};

typedef IndexArray<RawActionArray> RawActionArrayIndexArray;

class Macro {
private:
  mutable BitSet m_stateSet;
  mutable UINT   m_stateSetSize;
  int            m_index;     // index in array m_actionCode
  String         m_name;
  const String   m_value, m_comment;
public:
  Macro(UINT stateCount, UINT state0, const String &value, const String &comment)
    : m_stateSet(stateCount)
    , m_stateSetSize(0)
    , m_index(  -1         )
    , m_value(  value      )
    , m_comment(comment    )
  {
    addState(state0);
  }
  inline int getIndex() const {
    return m_index;
  }
  Macro &setIndex(UINT index) {
    m_index = index;
    return *this;
  }
  Macro &setName(const String &name) {
    m_name = name;
    return *this;
  }
  inline const String &getName() const {
    return m_name;
  }
  inline const String &getValue() const {
    return m_value;
  }
  String getComment() const;
  inline const BitSet &getStateSet() const {
    return m_stateSet;
  }
  inline UINT getStateSetSize() const {
    return m_stateSetSize;
  }
  inline void addState(UINT state) const {
    m_stateSet.add(state);
    m_stateSetSize++;
  }
};

inline bool operator==(const Macro &m1, const Macro &m2) {
  return (m1.getName() == m2.getName()) && (m1.getValue() == m2.getValue());
}

inline int macroCmpByName(const Macro &m1, const Macro &m2) {
  return stringCmp(m1.getName(), m2.getName());
}

inline int macroCmpByIndex(const Macro &m1, const Macro &m2) {
  return m1.getIndex() - m2.getIndex();
}

class CompressedActionMatrix {
private:
  const GrammarTables          &m_tables;
  const UINT                    m_stateCount;
  const UINT                    m_terminalCount;
  const UINT                    m_laSetSizeInBytes;
  UINT                          m_currentTermListSize;
  UINT                          m_currentActionListSize;
  UINT                          m_currentLASetArraySize;
  UINT                          m_currentSplitNodeCount;
  const IntegerType             m_terminalType, m_actionType;
  StateActionInfoArray          m_stateInfoArray;
  SymbolSetIndexMap             m_laSetMap;
  SymbolSetIndexMap             m_termListMap;
  RawActionArrayIndexMap        m_raaMap;
  Array<Macro>                  m_macroArray;
  StringHashMap<UINT>           m_macroMap; // map macro-value -> index into macroArray
  inline UINT getTerminalCount() const {
    return m_terminalCount;
  }
  inline UINT getStateCount() const {
    return m_stateCount;
  }
  inline const TCHAR *getSymbolName(UINT symbolIndex) const {
    return m_tables.getSymbolName(symbolIndex);
  }
  // Return point to macro with same value if it exist, or nullptr if not
  const Macro *findMacroByValue( const String &macroValue) const;
  void   addMacro(               const Macro &m);
  Macro  doStateActionInfo(      const StateActionInfo &stateInfo);
  Macro  doUncompressedState(    const StateActionInfo &stateInfo);
  Macro  doSplitCompression(     const StateActionInfo &stateInfo);
  Macro  doOneItemState(         const StateActionInfo &stateInfo);
  Macro  doReduceBySameProdState(const StateActionInfo &stateInfo);
  void   generateCompressedForm();

  ByteCount printMacroesAndActionCode(MarginFile &output) const;
  ByteCount printTermAndActionList(   MarginFile &output) const;
  ByteCount printTermSetTable(        MarginFile &output) const;

public:
  CompressedActionMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrix
