#pragma once

#include <TreeMap.h>
#include "NFA.h"
#include "DFARegex.h"

// Transitions for each DFAState
typedef int DFAtrans[MAX_CHARS];

class DFAState {
private:
  inline void initTransisition() {
    memset(m_transition, -1, sizeof(m_transition));
  }

public:
  const UINT    m_id;
  int           m_acceptIndex;
  // Set of NFA states
  BitSet        m_NFAset;
  // Next state for each character
  DFAtrans      m_transition;

  inline DFAState(UINT id) : m_id(id), m_NFAset(8) {
    initTransisition();
  }
  inline DFAState(UINT id, const BitSet &NFAset, int acceptIndex) : m_id(id), m_NFAset(NFAset) {
    initTransisition();
    m_acceptIndex = acceptIndex;
  }
};

#define FAILURE -1                   // if no transition on c, then m_transition[c] = FAILURE

class DFA {
private:
  const NFA      &m_NFA;
  Array<DFAState> m_states;
  // Groups with equal AcceptAction in m_states
  Array<BitSet>   m_groups;
  // Indexed by DFAState-number. Holds the index of the group, a state belongs to
  CompactIntArray m_inGroup;
  BitSet         *transition(BitSet &dst, BitSet &NFAset, int c) const;
  void            epsClosure(BitSet &NFAset, int &acceptIndex) const;

  // Initially m_states contains a single, start state formed by
  // taking the epsilon closure of the NFA start state. m_states[0]
  // is the DFA start state.
  void construct();
  void makeTransitions();
  void makeInitialGroups();
  void fixupTransitions();
  void minimize();
  bool columnsEqual(size_t col1, size_t col2) const;
  bool rowsEqual(size_t row1, size_t row2) const;
  void reduce(DFATables &tables, BitSet &rowSave, BitSet &columnSave) const;
  void getTransitionMatrix(DFATables &tables, const BitSet &rowSave, const BitSet &columnSave) const;
  void getAcceptTable(DFATables &tables) const;
public:
  DFA(const NFA &nfa);
  void getDFATables(DFATables &tables) const;
};
