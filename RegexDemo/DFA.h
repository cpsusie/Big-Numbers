#pragma once

#include <TreeMap.h>
#include "NFA.h"
#include "DFARegex.h"

typedef int DFAtrans[MAX_CHARS];     // transitions for each DFAState

#ifdef _DEBUG
typedef IntTreeMap<CharacterSet> DFATransitionMap;
#endif

class DFAState {
private:
  inline void initTransisition() {
    memset(m_transition, -1, sizeof(m_transition));
  }

public:
  const UINT    m_id;
  AcceptType    m_accept;
  BitSet        m_NFAset;            // Set of NFA states
  DFAtrans      m_transition;        // Next state for each character

  inline DFAState(UINT id, const BitSet &NFAset) : m_id(id), m_NFAset(NFAset) {
    initTransisition();
  }
  inline DFAState(UINT id, const BitSet &NFAset, AcceptType accept) : m_id(id), m_NFAset(NFAset) {
    initTransisition();
    m_accept = accept;
  }

#ifdef _DEBUG
  void getTransitionMap(DFATransitionMap &map) const;
  String toString(bool showNFASet=false) const;
#endif
};

#define FAILURE -1                   // if no transition on c, then m_transition[c] = FAILURE
                                    
class DFA {
private:
  const NFA      &m_NFA;
  Array<DFAState> m_states;
  Array<BitSet>   m_groups;          // Groups with equal AcceptAction in m_states
  CompactIntArray m_inGroup;         // Indexed by DFAState-number. Holds the index of the group, a state belongs to
  BitSet          getNFASetForGroup(UINT g) const;
#ifdef _DEBUG
  String          m_logString;
  BitSet      getPatternIndexSet(UINT stateIndex, int maxPatternCharIndex) const;
#endif
  BitSet     *transition(BitSet &dst, BitSet &NFAset, int c) const;
  void        epsClosure(BitSet &NFAset, AcceptType &accept) const;
  void        makeTransitions();
  void        makeInitialGroups();
  void        fixupTransitions();
  void        minimize();
  bool        columnsEqual(size_t col1, size_t col2) const;
  bool        rowsEqual(size_t row1, size_t row2) const;
  void        reduce(DFATables &tables, BitSet &rowSave, BitSet &columnSave) const;
  void        getTransitionMatrix(DFATables &tables, const BitSet &rowSave, const BitSet &columnSave) const;
  void        getAcceptTable(DFATables &tables) const;
public:
  DFA(const NFA &nfa);
  void construct();
  void getDFATables(DFATables &table) const;

#ifdef _DEBUG
  DFA(const DFATables &tables, const NFA &dummy);
  String toString(bool showNFASets = false, _DFADbgInfo *dbgInfo=NULL) const;
  String groupsToString() const;
  const Array<DFAState> &getStateArray() const {
    return m_states;
  }
  size_t getStateCount() const {
    return m_states.size();
  }
  void paint(CWnd *wnd, int currentState = -1, int lastAcceptState = -1) const;
#endif
};
