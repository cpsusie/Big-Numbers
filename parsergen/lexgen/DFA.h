#pragma once

#include "NFA.h"
#include <MarginFile.h>

typedef enum {
	CPP
 ,JAVA
} Language;

typedef int DFAtrans[MAX_CHARS];     // transitions for each DFAstate

class DFAstate {
public:
  int           m_id;
  AcceptAction *m_accept;            // accept action if accept state
  BitSet       *m_NFAset;            // Set of NFA states
  DFAtrans      m_transition;        // next state for each character

  DFAstate(int id, BitSet *NFAset = NULL, AcceptAction *action = NULL);
  void print(MarginFile &f) const;
};

#define FAILURE -1                   // if no transition on c, then m_transition[c] = FAILURE

class DFA {
private:
  const NFA      &m_NFA;
  Array<BitSet>   m_groups;          // groups with equal AcceptAction in m_states
  Array<int>      m_inGroup;         // indexed by DFAstate-number. Holds the index of the group, a state belongs to
  bool            m_verbose;
  Language        m_language;
  MarginFile     &m_verboseFile;

  BitSet *newNFAset() const {        // creates a set of NFA-states to be put in DFAstate
    return new BitSet(m_NFA.size());
  }

  int         stateExist(const BitSet &NFAset) const;
  BitSet     *transition(BitSet &NFAset, int c) const;
  void        epsClosure(BitSet &NFAset, AcceptAction *&accept) const;
  void        makeTransitions();
  void        makeInitialGroups();
  void        fixupTransitions();
  void        minimize();
  void        reduce(int *rowMap, int *columnMap, BitSet &rowSave, BitSet &columnSave) const;
  bool        columnsEqual(size_t col1, size_t col2) const;
  bool        rowsEqual(size_t row1, size_t row2) const;
  void        minmaxElement(BitSet &rowSave, BitSet &columnSave, int &minElement, int &maxElement) const;
  const TCHAR *findTransisitionType(BitSet &rowSave, BitSet &columnSave) const;
  void        printGroups(   MarginFile &f);
  void        printCharMap(  MarginFile &f) const;
  void        printCharMap(  MarginFile &f, const int *map) const;
  void        printStateMap( MarginFile &f, const int *map) const;
  void        printTransitionMatrix(MarginFile &f, BitSet &rowSave, BitSet &columnSave) const;
  void        printAcceptTable(MarginFile &f) const;
public:
  Array<DFAstate> m_states;
  DFA(const NFA &nfa, Language language, bool verbose = false);
  void printStates(MarginFile &f) const;
  void printTables(MarginFile &f) const;
  Language getLanguage() const { return m_language; }
  const SourceText getHeader()     const { return m_NFA.m_header;     }
  const SourceText getDriverHead() const { return m_NFA.m_driverHead; }
  const SourceText getDriverTail() const { return m_NFA.m_driverTail; }
};
