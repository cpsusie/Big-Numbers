#pragma once

class StatePair {
public:
  static constexpr UINT NoFromStateCheck = AbstractParserTables::_NoFromStateCheck;
  UINT m_fromState;
  UINT m_newState;
  bool m_shiftText;
  inline StatePair() : m_fromState(0), m_newState(0) {
  }
  StatePair(UINT fromState, UINT newState, bool shiftText);
  inline bool isNoFromStateCheck() const {
    return m_fromState == NoFromStateCheck;
  }
  String toString() const;
};

inline std::wostream &operator<<(std::wostream &out, const StatePair &p) {
  out << p.toString();
  return out;
}

class StatePairArray : public CompactArray<StatePair> {
public:
  inline StatePairArray() {
  }
  inline StatePairArray(UINT capacity) : CompactArray(capacity) {
  }
  StatePairArray &sortByFromState();
  StatePairArray &sortByNewState();
  // Return all legal from-states in array as a BitSet
  StateSet        getFromStateSet(UINT stateCount) const;
  // Return all (no neccessary distinct) newstates in array
  StateArray      getNewStateArray()               const;
  // = size() = number of different from-states in array
  inline UINT     getFromStateCount()              const {
    return(UINT)size();
  }
  // = size() = number of different to-states in array
  inline UINT     getNewStateCount()               const {
    return (UINT)size();
  }
  String          toString() const;
};

class StatePairBitSet {
private:
  const Grammar &m_grammar;
  StateSet       m_fromStateSet;     // set of states having newState as successor
  UINT           m_fromStateCount;
  const UINT     m_newState;
  const bool     m_shiftText;
public:
  StatePairBitSet(const Grammar &grammar, UINT newState, UINT fromState0, bool shiftText)
    : m_grammar(         grammar                )
    , m_fromStateSet(    grammar.getStateCount())
    , m_fromStateCount(  0                      )
    , m_newState(        newState               )
    , m_shiftText(       shiftText              )
  {
    addFromState(fromState0);
  }
  inline const StateSet &getFromStateSet()   const {
    return m_fromStateSet;
  }
  inline UINT            getFromStateCount() const {
    return m_fromStateCount;
  }
  // = 1
  inline UINT            getNewStateCount()  const {
    return 1;
  }
  inline UINT            getNewState()       const {
    return m_newState;
  }
  inline void            addFromState(UINT fromState) {
    m_fromStateSet.add(fromState);
    m_fromStateCount++;
  }
  operator               StatePairArray()    const;
  String                 toString()          const;
};

class StatePairBitSetArray : public Array<StatePairBitSet> {
public:
  StatePairBitSetArray() {
  }
  StatePairBitSetArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  void sortBySetSize();
  // Return sum(((*this)[i].getFromStateCount()...i=0..size-1)
  UINT getFromStateCount() const;
  // = size() = number of different to-states in array
  inline UINT    getNewStateCount() const {
    return (UINT)size();
  }
  String toString() const;
};
