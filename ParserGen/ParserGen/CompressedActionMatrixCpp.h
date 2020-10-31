#pragma once

#include <TreeMap.h>
#include "GrammarTables.h"

class ArrayIndex {
public:
  const UINT m_arrayIndex;   // offset into array
  int        m_commentIndex; // used for index of element in comment
  ArrayIndex(UINT arrayIndex) : m_arrayIndex(arrayIndex), m_commentIndex(-1) {
  }
};

inline int arrayIndexCmp(const ArrayIndex &i1, const ArrayIndex &i2) {
  return (int)i1.m_arrayIndex - (int)i2.m_arrayIndex;
}

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

typedef enum {
  UNCOMPRESSED
 ,SPLITCOMPRESSION
 ,ONEITEMCOMPRESSION
 ,REDUCEBYSAMEPRODCOMPRESSION
 ,MAXCOMPRESSIONVALUE = SPLITCOMPRESSION
} CompressionMethod;

inline UINT encodeCompressMethod(CompressionMethod method) {
  return (((UINT)method) << 15);
}

void checkMax15Bits(const TCHAR *method, int line, int v, const TCHAR *varName);

#define CHECKMAX15BITS(v) checkMax15Bits(__TFUNCTION__,__LINE__,v,_T(#v))

#if defined(_DEBUG)

// check that bits, used for encoding the compression method are all zero, give error-message and exit(-1) if this is not the case
void checkCodeBits( const TCHAR *method, UINT v, const TCHAR *varName);

#define CHECKCODEBITS(v) checkCodeBits(__TFUNCTION__,v,_T(#v))

#else

#define CHECKCODEBITS(v)

#endif // _DEBUG

inline UINT encodeValue(UINT v, CompressionMethod method) {
  CHECKCODEBITS(v);
  return (v | encodeCompressMethod(method));
}

// ------------------------------------------------------------------------------------

namespace ActionMatrix {

class SameReduceActionInfo {
private:
  const UINT m_prod;
  SymbolSet  m_termSet;
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
  inline const SymbolSet &getTerminalSet() const {
    return m_termSet;
  }
  inline void addTerminal(UINT token) {
    m_termSet.add(token);
  }
};

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
  void doUncompressedState(    const StateActionInfo &stateInfo);
  void doSplitCompression(     const StateActionInfo &stateInfo);
  void doOneItemState(         const StateActionInfo &stateInfo);
  void doReduceBySameProdState(const StateActionInfo &stateInfo);

  ByteCount printTermAndActionList(   MarginFile &output) const;
  ByteCount printTermSetTable(        MarginFile &output) const;
  ByteCount printDefinesAndActionCode(MarginFile &output) const;
public:
  CompressedActionMatrix(const GrammarTables &tables);
  void generateCompressedForm();
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrix
