#include "stdafx.h"
#include <CompactStack.h>
#include <HashMap.h>
#include "DFA.h"

#ifdef _DEBUG
#define DBG_addToDFALog(s)        { if(m_NFA.m_stepHandler) { m_logString += s; } }
#define DBG_callDFAStepHandler(s) { DBG_addToDFALog(s); DBG_callCompilerHandler(m_logString, EMPTYSTRING,EMPTYSTRING,-1); }
#define DBG_resetDFALog()           m_logString = EMPTYSTRING
#else
#define DBG_addToDFALog(s)
#define DBG_callDFAStepHandler(s)
#define DBG_resetDFALog()
#endif

DFA::DFA(const NFA &nfa) : m_NFA(nfa) {
}

void DFA::construct() {
  DBG_resetDFALog();
  if(m_NFA.isEmpty()) return;

  makeTransitions();
  minimize();

  DBG_callDFAStepHandler(format(_T("Minimized DFA table:\n%s\n"), toString().cstr()));
}

static ULONG bitSetHashCode(BitSet * const &s) {
  return s->hashCode();
}

static int bitSetCompare(BitSet * const &s1, BitSet * const &s2) {
  return bitSetCmp(*s1, *s2);
}

void DFA::makeTransitions() {
  DBG_callDFAStepHandler(format(_T("%sConstructing unminimized DFA\n"), m_NFA.toString().cstr()));
  HashMap<BitSet*, int> bitsetMap(bitSetHashCode, bitSetCompare, 991);

  BitSet NFAset(m_NFA.size());
  NFAset.add(0);
  DFAState startState(0, NFAset);
  epsClosure(NFAset, startState.m_accept);
  startState.m_NFAset = NFAset;


  m_states.add(startState);
  bitsetMap.put(&m_states.last().m_NFAset, 0);

  BitSet transitionSet(m_NFA.size());
  for(size_t index = 0; index < m_states.size(); index++) {
    DFAState &current = m_states[index];

#ifdef UNICODE
    CharacterSet outgoingCharacters;
    for(Iterator<size_t> it = current.m_NFAset.getIterator(); it.hasNext();) {
      NFAState *nfaState = m_NFA[it.next()];
      switch(nfaState->getEdge()) {
      case EDGE_EPSILON:
        break;
      case EDGE_CHCLASS:
        outgoingCharacters += nfaState->getCharacterSet();
        break;
      case EDGE_UNUSED :
        throwException(_T("Unexpected edge for NFA-state:Unused"));
      default          :
        outgoingCharacters.add(nfaState->getEdge());
        break;
      }
    }

    for(Iterator<size_t> it = outgoingCharacters.getIterator(); it.hasNext();) {
      int c = (int)it.next();
#else

    for(int c = 0; c < MAX_CHARS ; c++) {

#endif

      int nextDFAState = FAILURE;         // go to nextDFAState on char c
      BitSet *newSet = transition(transitionSet, current.m_NFAset, c);
      if(newSet != NULL) {
        AcceptType accept;                // if current DFA state is an acceptstate, this is the AcceptAction
        epsClosure(*newSet, accept);

        const int *succ = bitsetMap.get(newSet);
        if(succ) {
          nextDFAState = *succ;
        } else {
          nextDFAState = (int)m_states.size();
          m_states.add(DFAState(nextDFAState, *newSet, accept));
          bitsetMap.put(&m_states.last().m_NFAset, nextDFAState);
        }
      }
      current.m_transition[c] = nextDFAState;
    }
    DBG_callDFAStepHandler(current.toString(true));
  }
}

// NFAset  is the set of start states to examine. also used as output
// *accept is modified to point to the AcceptAction associated with an accepting
//         state (or NULL if the state isn't an accepting state).
//
// Computes the epsilon closure set for the NFAset. This set will contain all states
// that can be reached by making epsilon transitions from all NFA states in the original set.
// Returns an empty set if the set or the closure set is empty.
// Modifies *accept to point to the
// accepting String with LOWEST lineno, if one of the NFA states in the output set is an
// accepting state.
void DFA::epsClosure(BitSet &NFAset, AcceptType &accept) const {
// The algorithm is:
//
//   push all NFA-states in NFAset set onto stateStack
//   while(stateStack is not empty) {
//     pop the top element p
//     if(p is an accept state) {
//       accept = p.accept
//     }
//     if(there's an epsilon transition from p to q) {
//       if(q isn't in NFAset) {
//         add q to NFAset
//         push q onto stateStack
//       }
//     }
//   }

  CompactStack<int> stateStack; // stack of NFA-states remaining to be tested

  for(Iterator<size_t> it = NFAset.getIterator(); it.hasNext(); ) {                // 1
    stateStack.push((int)it.next());
  }

  while(!stateStack.isEmpty()) {                                                   // 2
    const NFAState &p = *m_NFA[stateStack.pop()];                                  // 3
    if(p.m_accept.isAccepting()) {                                                 // 4
      accept = p.m_accept;
    }

    if(p.getEdge() == EDGE_EPSILON) {                                              // 5
      if(p.m_next) {
        int next = p.m_next->getID();
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

// Return a set that contains all NFA states that can be reached by making
// transitions on "c" from any NFA state in NFAset. Returns NULL if no
// such transition exist.
BitSet *DFA::transition(BitSet &dst, BitSet &NFAset, int c) const {
  BitSet *result = NULL;

  for(Iterator<size_t> it = NFAset.getIterator(); it.hasNext();) {
    int i = (int)it.next();
    const NFAState *p = m_NFA[i]->getSuccessor(c);
    if(p) {
      if(result == NULL) {
        dst.clear();
        result = &dst;
      }
      result->add(p->getID());
    }
  }
  return result;
}

// Put states with equal AcceptAction into the same group.
// Note that all non-accept-states go to the same group
void DFA::minimize() {
  bool stable;                                          // Did we anything in this pass

  makeInitialGroups();
  do {
    stable = true;
    for(size_t i = 0; i < m_groups.size(); i++) {
      BitSet &current = m_groups[i];
      if(current.size() <= 1) {
        continue;
      }

      BitSet newSet(m_states.size());
      Iterator<size_t> it = current.getIterator();
      int first = (int)it.next();                            // State number of first element of current group
      while(it.hasNext()) {
        const int next = (int)it.next();                     // State number of next  element of current group
        for(int c = 0; c < MAX_CHARS; c++) {
          const int firstSuccessor = m_states[first].m_transition[c];
          const int nextSuccessor  = m_states[next ].m_transition[c];

          if(firstSuccessor != nextSuccessor            // If successor-states differ or belong to different groups
             && (   firstSuccessor == FAILURE
                 || nextSuccessor  == FAILURE
                 || m_inGroup[firstSuccessor] != m_inGroup[nextSuccessor]
                )
            ) {
            current.remove(next);                       // Move the state to newset
            newSet.add(next);
            m_inGroup[next] = (int)m_groups.size();
            break;
          }
        } // for
      } // while
      if(!newSet.isEmpty()) {
        m_groups.add(newSet);
        stable = false;
      }
    } // for
  } while(!stable);

  DBG_callDFAStepHandler(format(_T("All DFA groups:\n%s"), groupsToString().cstr()));

  fixupTransitions();
}

void DFA::makeInitialGroups() {
  for(size_t i = 0; i < m_states.size(); i++) {
    m_inGroup.add(-1);
  }

  for(size_t i = 0; i < m_states.size(); i++) {
    BitSet newSet(m_states.size());
    for(size_t j = 0; j < i; j++) {
      // Check to see if a group already exists, ie. that has the same
      // accepting String as the current state. If so, add the current
      // state to the already existing group and skip past the code that
      // would create a new group. Note that since all nonAccepting states
      // have NULL accept strings, this loop puts all of these together
      // into a single group.

      if(m_states[i].m_accept == m_states[j].m_accept) {
        m_groups[m_inGroup[j]].add(i);
        m_inGroup[i] = m_inGroup[j];
        goto Continue;
      }
    }

    // Create a new group and put the current state into it.

    newSet.add(i);
    m_inGroup[i] = (int)m_groups.size(); // state[i] belongs to group m_groups.size()
    m_groups.add(newSet);

Continue:; // Group already exists.
  }
  DBG_callDFAStepHandler(format(_T("Initial DFA groups:\n%s"), groupsToString().cstr()));
}

  // Reduce the size of the m_states to the number of groups.
  // Consider the first element of each group (state) to be a
  // "representative" state. Insert this state to the new transition table,
  // modifying all the transitions, so that the successors
  // are the groups within which the old state is found.
void DFA::fixupTransitions() {
  Array<DFAState> newStates;

  DBG_addToDFALog(_T("Constructing minimized DFA\n"));

  for(UINT g = 0; g < m_groups.size(); g++) {
    Iterator<size_t> it = m_groups[g].getIterator();
    UINT state = (UINT)it.next();       // there is at least one state in each group
    newStates.add(DFAState(g, getNFASetForGroup(g)));
    DFAState &newState = newStates.last();
    DFAState &oldState = m_states[state];
    newState.m_accept  = oldState.m_accept;
    for(int c = 0; c < MAX_CHARS; c++) {
      int trans = oldState.m_transition[c];
      newState.m_transition[c] = (trans == FAILURE) ? FAILURE : m_inGroup[trans];
    }
    DBG_callDFAStepHandler(newState.toString(true));
  }
  m_states = newStates;
}

BitSet DFA::getNFASetForGroup(UINT g) const {
#ifdef _DEBUG
  BitSet result(m_NFA.size());
  for(Iterator<size_t> it = ((BitSet&)m_groups[g]).getIterator(); it.hasNext();) {
    result += m_states[it.next()].m_NFAset;
  }
  return result;
#else
  return BitSet(8); // Not needed in nodebug.
#endif
}

#ifdef _DEBUG

DFA::DFA(const DFATables &tables, const NFA &dummy) : m_NFA(dummy) {
  if (tables.isEmpty()) {
    return;
  }
  Array<CompactIntArray> inverseCharMap;
  for(size_t c = 0; c < tables.m_columnCount; c++) {
    inverseCharMap.add(CompactIntArray());
  }
  for(int ch = 0; ch < MAX_CHARS; ch++) {
    const int c = tables.m_charMap[ch];
    if(c) inverseCharMap[c].add(ch);
  }
  Array<CompactIntArray> inverseStateMap;
  for(size_t r = 0; r < tables.m_rowCount; r++) {
    inverseStateMap.add(CompactIntArray());
  }
  for(size_t s = 0; s < tables.m_stateCount; s++) {
    const int r = tables.m_stateMap[s];
    inverseStateMap[r].add((int)s);
  }

  for(UINT s = 0; s < tables.m_stateCount; s++) {
    AcceptType accept;
    const BYTE anchor = tables.m_acceptTable[s];
    if(anchor) {
      accept.setAccepting(anchor);
    }
    m_states.add(DFAState(s, BitSet(8), accept));
  }

  for(UINT r = 0; r < tables.m_rowCount; r++) {
    DFAtrans transition;
    memset(transition, -1, sizeof(transition));

    for(UINT c = 0; c < tables.m_columnCount; c++) {
      const short tr = tables.transition(r, c);
      if(tr != FAILURE) {
        const CompactIntArray &ca = inverseCharMap[c];
        for(size_t i = 0; i < ca.size(); i++) {
          transition[ca[i]] = tr;
        }
      }
      const CompactIntArray &stateArray = inverseStateMap[r];
      for(size_t i = 0; i < stateArray.size(); i++) {
        memcpy(m_states[stateArray[i]].m_transition, transition, sizeof(DFAtrans));
      }
    }
  }
}

static int countNewLines(const String &s) {
  int count = 0;
  for(const TCHAR *cp = s.cstr(); *cp;) {
    if(*(cp++) == _T('\n')) {
      count++;
    }
  }
  return count;
}


BitSet DFA::getPatternIndexSet(UINT stateIndex, int maxPatternCharIndex) const {
  BitSet result(maxPatternCharIndex+1);
  const DFAState &state = m_states[stateIndex];
  for(Iterator<size_t> it = ((BitSet&)(state.m_NFAset)).getIterator(); it.hasNext();) {
    const NFAState *nfaState = m_NFA[it.next()];
    if((nfaState->getEdge() != EDGE_EPSILON) && nfaState->m_patternCharIndex >= 0) {
      result.add(nfaState->m_patternCharIndex);
    }
  }
  return result;
}

String DFA::toString(bool showNFASets, _DFADbgInfo *dbgInfo) const {
  String result;
  if(dbgInfo) {
    int lineCount = 0;
    const int maxPatternCharIndex = m_NFA.getMaxCharIndex();
    dbgInfo->clear();
    for(size_t i = 0; i < m_states.size(); i++) {
      const String stateStr = m_states[i].toString(showNFASets);
      result += stateStr;
      dbgInfo->add(_DFADbgStateInfo(lineCount, getPatternIndexSet((UINT)i, maxPatternCharIndex)));
      lineCount += countNewLines(stateStr);
    }
  } else {
    for(size_t i = 0; i < m_states.size(); i++) {
      result += m_states[i].toString(showNFASets);
    }
  }
  return result;
}

static int getDigitCount(size_t n) {
  if(n < 10) {
    return 1;
  } else if(n < 100) {
    return 2;
  } else {
    return 3;
  }
}

String DFA::groupsToString() const {
  String result;
  const int digits = getDigitCount(m_groups.size());
  for(size_t i = 0; i < m_groups.size(); i++) {
    result += format(_T("DFA Group[%*u]:%s\n"), digits, (UINT)i, m_groups[i].toString().cstr());
  }
  result += format(_T("In group:%s\n"), m_inGroup.toStringBasicType().cstr());
  return result;
}

#endif
