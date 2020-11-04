#pragma once

#include <NumberInterval.h>
#include "NFA.h"

// transitions for each DFAstate
typedef int DFAtrans[MAX_CHARS];

class DFAstate {
public:
  int           m_id;
  // accept action if accept state
  AcceptAction *m_accept;
  // Set of NFA states
  BitSet       *m_NFAset;
  // next state for each character
  DFAtrans      m_transition;

  DFAstate(int id, BitSet *NFAset = nullptr, AcceptAction *action = nullptr);
  void print(MarginFile &f) const;
};

// if no transition on c, then m_transition[c] = FAILURE
#define FAILURE -1

class DFA {
private:
  const NFA      &m_NFA;
  // groups with equal AcceptAction in m_states
  Array<BitSet>   m_groups;
  // indexed by DFAstate-number. Holds the index of the group, a state belongs to
  CompactIntArray m_inGroup;
  bool            m_verbose;
  Language        m_language;
  MarginFile     &m_verboseFile;

  // creates a set of NFA-states to be put in DFAstate
  BitSet *newNFAset() const {
    return new BitSet(m_NFA.size());
  }

  // If there's a DFA-state with m_NFAset identical to NFA_set, return the
  // index of the state entry, else return -1.
  int         stateExist(const BitSet &NFAset) const;
  // Return a set that contains all NFA states that can be reached by making
  // transitions on "c" from any NFA state in NFAset. Returns nullptr if
  // there are no such transitions.
  BitSet     *transition(BitSet &NFAset, int c) const;
  void        epsClosure(BitSet &NFAset, AcceptAction *&accept) const;
  // Initially m_states contains a single, start state formed by
  // taking the epsilon closure of the NFA start state. m_states[0]
  // is the DFA start state.
  void        makeTransitions();
  // Put states with equal AcceptAction into the same group.
  // Note that all non-accept-states go to the same group
  void         makeInitialGroups();
  // Reduce the size of the m_states to the number of groups.
  // Consider the first element of each group (state) to be a
  // "representative" state. Insert this state to the new transition table,
  // modifying all the transitions, so that the successors
  // are the groups within which the old state is found.
  void         fixupTransitions();
  void         minimize();
  void         reduce(int *rowMap, int *columnMap, BitSet &rowSave, BitSet &columnSave) const;
  // Return true if two columns in transitionmatrix are equal, else return false
  bool         columnsEqual(size_t col1, size_t col2) const;
  // Return true if two rows in transitionmatrix are equal, else return false
  bool         rowsEqual(size_t row1, size_t row2) const;
  IntInterval  minmaxElement(        const BitSet &rowSave, const BitSet &columnSave) const;
  void         printGroups(          MarginFile &f);
  void         printCharMap(         MarginFile &f) const;
  void         printCharMap(         MarginFile &f, const int *map) const;
  void         printStateMap(        MarginFile &f, const int *map) const;
  void         printTransitionMatrix(MarginFile &f, const BitSet &rowSave, const BitSet &columnSave) const;
  void         printAcceptTable(     MarginFile &f) const;
public:
  Array<DFAstate> m_states;
  DFA(const NFA &nfa, Language language, bool verbose = false);
  void printStates(MarginFile &f) const;
  // Compress and output transitionmatrix eliminating equal columns and rows
  void printTables(MarginFile &f) const;
  Language getLanguage() const { return m_language; }
  const SourceText getHeader()     const { return m_NFA.m_header;     }
  const SourceText getDriverHead() const { return m_NFA.m_driverHead; }
  const SourceText getDriverTail() const { return m_NFA.m_driverTail; }
};
