#include "stdafx.h"
#include <string.h>
#include "DFA.h"

DFA::DFA(const NFA &nfa, Language language, bool verbose) : m_NFA(nfa), m_verboseFile(stdoutMarginFile) {
  m_language = language;
  m_verbose  = verbose;
  makeTransitions();

  if(m_verbose) {
    _tprintf(_T("Unminimized DFA table:\n"));
    for(size_t i = 0; i < m_states.size(); i++) {
      m_states[i].print(m_verboseFile);
    }
  }
  minimize();

  if(m_verbose) {
    _tprintf(_T("Minimized DFA table:\n"));
    for(size_t i = 0; i < m_states.size(); i++) {
      m_states[i].print(m_verboseFile);
    }
  }
}

DFAstate::DFAstate(int id, BitSet *NFAset, AcceptAction *action) {
  m_id     = id;
  m_accept = action;
  m_NFAset = NFAset;
  memset(m_transition, 0, sizeof(m_transition));
}

void DFA::makeTransitions() {
  BitSet *NFAset = newNFAset();           // set of NFA states that defines the next DFA state.
  NFAset->add(0);
  DFAstate startState(0);
  epsClosure(*NFAset, startState.m_accept);
  startState.m_NFAset = NFAset;

  m_states.add(startState);
  for(size_t index = 0; index < m_states.size(); index++) {
    DFAstate *current = &m_states[index];
    if(m_verbose) {
      m_verboseFile.printf(_T("DFA state %3d. NFA states:"), current->m_id);
      printSet(m_verboseFile, *current->m_NFAset);
      m_verboseFile.printf(_T("\n"));
    }
    for(int c = 0; c < MAX_CHARS ; c++) {
      int nextDFAstate = FAILURE;         // go to nextDFAstate on char c
      if(NFAset = transition(*current->m_NFAset, c)) {
        AcceptAction *accept;             // if current DFA state is an acceptstate, this is the AcceptAction
        epsClosure(*NFAset, accept);

        if((nextDFAstate = stateExist(*NFAset)) < 0) {
          nextDFAstate = (int)m_states.size();
          m_states.add(DFAstate(nextDFAstate, NFAset, accept));
        } else {
          delete NFAset;
        }
      }
      current->m_transition[c] = nextDFAstate;
    }
  }
}

int DFA::stateExist(const BitSet &NFAset) const {
  for(size_t i = 0; i < m_states.size(); i++) {
    if(*m_states[i].m_NFAset == NFAset) {
      return (int)i;
    }
  }
  return -1;
}

void DFA::epsClosure(BitSet &NFAset, AcceptAction *&accept) const {
  // NFAset   is the set of start states to examine. also used as output
  // *accept  is modified to point to the AcceptAction associated with an accepting
  //          state (or nullptr if the state isn't an accepting state).
  //
  // Computes the epsilon closure set for the NFAset. This set
  // will contain all states that can be reached by making epsilon transitions
  // from all NFA states in the original set. Returns an empty set if the
  // set or the closure set is empty. Modifies *accept to point to the
  // accepting String with LOWEST lineno, if one of the NFA states in the output set is an
  // accepting state.
  // The algorithm is:
  //
  // 1:  push all NFA-states in NFAset set onto stateStack
  // 2:  while(stateStack is not empty)
  // 3:      pop the top element p
  // 4       if p is an accept state and p.accept.lineno < accept.lineno
  //            accept = p.accept
  // 5:      if(there's an epsilon transition from p to q)
  // 6:         if(q isn't in NFAset)
  // 7:              add q to NFAset
  // 8:              push q onto stateStack


  Stack<int> stateStack; // stack of NFA-states remaining to be tested

  for(Iterator<size_t> it = NFAset.getIterator(); it.hasNext(); ) {                   // 1
    stateStack.push((int)it.next());
  }

  accept = nullptr;
  while(!stateStack.isEmpty()) {                                                   // 2
    const NFAstate &p = *m_NFA[stateStack.pop()];                                  // 3
    if(p.m_accept && (accept == nullptr || p.m_accept->m_pos.getLineNumber() < accept->m_pos.getLineNumber())) {     // 4
      accept      = p.m_accept;
    }

    if(p.m_edge == EDGE_EPSILON) {                                                 // 5
      if(p.m_next1) {
        int next = p.m_next1->getID();
        if(!NFAset.contains(next)) {                                               // 6
          NFAset.add(next);                                                        // 7
          stateStack.push(next);                                                   // 8
        }
      }
      if(p.m_next2) {
        int next = p.m_next2->getID();
        if(!NFAset.contains(next)) {                                               // 6
          NFAset.add(next);                                                        // 7
          stateStack.push(next);                                                   // 8
        }
      }
    }
  }
}

BitSet *DFA::transition(BitSet &NFAset, int c) const {
  BitSet *result = nullptr;

  for(Iterator<size_t> it = NFAset.getIterator(); it.hasNext();) {
    int i = (int)it.next();
    const NFAstate *p = m_NFA[i]->successor(c);
    if(p) {
      if(result == nullptr) {
        result = newNFAset();
      }
      result->add(p->getID());
    }
  }
/*
  printf(_T("transitions on "));
  printchar(c);
  printf(_T(" from {"));
  printSet(inp_set);
  printf(_T("}="));
  if(result != nullptr)
    printSet(*result);
  else
    printf(_T("nullptr"));
  printf(_T("\n"));
*/
  return result;
}

void DFA::printGroups(MarginFile &f) {
  for(size_t i = 0; i < m_groups.size(); i++) {
    f.printf(_T("Group %2d:"), (int)i);
    printSet(f, m_groups[i]);
    f.printf(_T("\n"));
  }
}

void DFA::printStates(MarginFile &f) const {
  for(size_t i = 0; i < m_states.size(); i++) {
    m_states[i].print(f);
  }
}

void DFA::makeInitialGroups() {
  for(size_t i = 0; i < m_states.size(); i++) {
    m_inGroup.add(-1);
  }

  for(size_t i = 0; i < m_states.size(); i++) {
    BitSet newset(m_states.size());
    for(size_t j = 0; j < i; j++) {
      // Check to see if a group already exists, ie. that has the same
      // accepting String as the current state. If so, add the current
      // state to the already existing group and skip past the code that
      // would create a new group. Note that since all nonAccepting states
      // have nullptr accept strings, this loop puts all of these together
      // into a single group.

      if(m_states[i].m_accept == m_states[j].m_accept) {
        m_groups[m_inGroup[j]].add(i);
        m_inGroup[i] = m_inGroup[j];
        goto Continue;
      }
    }

    // Create a new group and put the current state into it.

    newset.add(i);
    m_inGroup[i] = (int)m_groups.size(); // state[i] belongs to group m_groups.size()
    m_groups.add(newset);

Continue:; // Group already exists.
  }

  if(m_verbose) {
    m_verboseFile.printf(_T("Initial groupings:\n"));
    printGroups(m_verboseFile);
  }
}

void DFA::fixupTransitions() {
  Array<DFAstate> newStates;

  for(size_t g = 0; g < m_groups.size(); g++) {
    Iterator<size_t> it = m_groups[g].getIterator();
    size_t state = it.next();       // there is at least one state in each group
    newStates.add(DFAstate((int)g));
    DFAstate &newState = newStates.last();
    DFAstate &oldState = m_states[state];
    newState.m_accept  = oldState.m_accept;
    for(int c = 0; c < MAX_CHARS; c++) {
      int trans = oldState.m_transition[c];
      newState.m_transition[c] = (trans == FAILURE) ? FAILURE : m_inGroup[trans];
    }
  }
  m_states = newStates;
}

void DFA::minimize() {
  bool stable;                                   // did we anything in this pass

  makeInitialGroups();
  do {
    stable = true;
    for(size_t i = 0; i < m_groups.size(); i++) {
      BitSet &current = m_groups[i];
      if(current.size() <= 1) {
        continue;
      }

      BitSet newset(m_states.size());
      Iterator<size_t> it = current.getIterator();
      int first = (int)it.next();                 // state number of first element of current group
      while(it.hasNext()) {
        int next = (int)it.next();                // state number of next  element of current group
        for(int c = 0; c < MAX_CHARS; c++) {
          int firstSuccessor = m_states[first].m_transition[c];
          int nextSuccessor  = m_states[next ].m_transition[c];

          if(firstSuccessor != nextSuccessor      // if successor-states differ or belong to different groups
             && (   firstSuccessor == FAILURE
                 || nextSuccessor  == FAILURE
                 || m_inGroup[firstSuccessor] != m_inGroup[nextSuccessor]
                )
            ) {
            current.remove(next);                 // move the state to newset
            newset.add(next);
            m_inGroup[next] = (int)m_groups.size();
            break;
          }
        } // for
      } // while
      if(!newset.isEmpty()) {
        m_groups.add(newset);
        stable = false;
      }
    } // for
  } while(!stable);

  if(m_verbose) {
    m_verboseFile.printf(_T("\nStates grouped as follows after minimization:\n"));
    printGroups(m_verboseFile);
  }
  fixupTransitions();
}

#if !defined(USE_COMPACT_DFAFORMAT)

void DFAstate::print(MarginFile &f) const {
  if(m_accept == nullptr) {
    f.printf(_T("// DFA State %3d [nonAccepting]"), m_id );
  } else {
    f.printf(_T("// DFA State %3d %s"), m_id, m_accept->dumpFormat().cstr());
  }

  int chars_printed = f.getLeftMargin();
  int last_transition = FAILURE;
  for(int j = 0; j < MAX_CHARS; j++) {
    if(m_transition[j] != FAILURE) {
      if(m_transition[j] != last_transition) {
        f.printf(_T("\n//   goto %2d on "), m_transition[j]);
        chars_printed = f.getLeftMargin();
      }
      String tmp = binToAscii(j);
      if(f.getCurrentLineLength() + tmp.length() > RMARGIN) {
        f.printf(_T("\n//              "));
        chars_printed = f.getLeftMargin();
      }
      f.printf(_T("%s"), tmp.cstr() );
      chars_printed += (int)tmp.length();
      last_transition = m_transition[j];
    }
  }
  f.printf(_T("\n"));
}

#else // USE_COMPACT_DFAFORMAT

#define FORMATCHAR(ch) binToAscii(ch)
#define _FLUSHRANGE()                                                           \
{ String tmp;                                                                   \
  if(delim) tmp += delim; else delim = ","    ;                                 \
  if(first == last) {                                                           \
    tmp = FORMATCHAR(first);                                                    \
  } else {                                                                      \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");    \
    tmp = format(formatStr, FORMATCHAR(first).cstr(), FORMATCHAR(last).cstr()); \
  }                                                                             \
  if(charsPrinted + tmp.length() > RMARGIN) {                                   \
    f.printf(_T("\n//              "));                                         \
    charsPrinted = f.getLeftMargin();                                           \
  }                                                                             \
  f.printf(_T("%s"), tmp.cstr());                                               \
  charsPrinted += tmp.length();                                                 \
}

#define FLUSHRANGE() { if(first <= last) _FLUSHRANGE(); }
#define NEWTRANS() { first = 1; last = 0; delim = nullptr; }

void DFAstate::print(MarginFile &f) const {
  if(m_accept == nullptr) {
    f.printf(_T("// DFA State %3d [nonAccepting]"), m_id );
  } else {
    f.printf(_T("// DFA State %3d %s"), m_id, m_accept->dumpFormat().cstr());
  }

  int charsPrinted   = f.getLeftMargin();
  int lastTransition = FAILURE;
  unsigned int first,last;
  const TCHAR *delim;

  NEWTRANS();
  for(int ch = 0; ch < MAX_CHARS; ch++) {
    if(m_transition[ch] != FAILURE) {
      if(m_transition[ch] != lastTransition) {
        FLUSHRANGE(); NEWTRANS();
        f.printf(_T("\n//   goto %2d on "), m_transition[ch]);
        charsPrinted = f.getLeftMargin();
      }
      if(first > last) {
        first = last = ch;
      } else if(ch == last+1) {
        last = ch;
      } else {
        FLUSHRANGE();
        first = last = ch;
      }
      lastTransition = m_transition[ch];
    }
  }
  FLUSHRANGE();
  f.printf(_T("\n"));
}

#endif
