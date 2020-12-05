#pragma once

class Grammar;
class StatePairArray;

class StatePairBitSet {
private:
  const Grammar &m_grammar;
  StateSet       m_fromStateSet;     // set of states having newState as successor
  UINT           m_fromStateCount;
  const UINT     m_newState;
  const bool     m_shiftText;
public:
  StatePairBitSet( const Grammar &grammar, UINT newState, UINT fromState0, bool shiftText);
  inline const StateSet  &getFromStateSet()     const {
    return m_fromStateSet;
  }
  inline UINT             getFromStateCount()   const {
    return m_fromStateCount;
  }
  // = 1
  inline UINT             getNewStateCount()    const {
    return 1;
  }
  inline UINT             getNewState()         const {
    return m_newState;
  }
  inline void             addFromState(UINT fromState) {
    m_fromStateSet.add(fromState);
    m_fromStateCount++;
  }
  operator                StatePairArray()      const;
  String                  toString()            const;
};
