#include "stdafx.h"
#include <CompactStack.h>
#include <HashMap.h>
#include "DFA.h"

DFA::DFA(const NFA &nfa) : m_NFA(nfa) {
  construct();
}

void DFA::construct() {
  if(m_NFA.isEmpty()) {
    return;
  }

  makeTransitions();
  minimize();
}

static ULONG bitSetHashCode(BitSet * const &s) {
  return s->hashCode();
}

static int bitSetCompare(BitSet * const &s1, BitSet * const &s2) {
  return bitSetCmp(*s1, *s2);
}

void DFA::makeTransitions() {
  HashMap<BitSet*, int> bitsetMap(bitSetHashCode, bitSetCompare, 991);

  BitSet NFAset(m_NFA.size());
  NFAset.add(0);
  DFAState startState(0, NFAset, false);
  epsClosure(NFAset, startState.m_acceptIndex);
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
        int acceptIndex = -1;                // if current DFA state is an acceptstate, this is the AcceptAction
        epsClosure(*newSet, acceptIndex);

        const int *succ = bitsetMap.get(newSet);
        if(succ) {
          nextDFAState = *succ;
        } else {
          nextDFAState = (int)m_states.size();
          m_states.add(DFAState(nextDFAState, *newSet, acceptIndex));
          bitsetMap.put(&m_states.last().m_NFAset, nextDFAState);
        }
      }
      current.m_transition[c] = nextDFAState;
    }
  }
}

// NFAset is the set of start states to examine. also used as output
// acceptIndex is modified, if one of the NFA-states in the epsclosure is an acceptState
// if several NFA-states are acceptstates, acceptIndex will receive the lowest acceptIndex
// among the accpting NFA-states
//
// Computes the epsilon closure set for the NFAset. This set will contain all states
// that can be reached by making epsilon transitions from all NFA states in the original set.
// Returns an empty set if the set or the closure set is empty.
void DFA::epsClosure(BitSet &NFAset, int &acceptIndex) const {
// The algorithm is:
//
//   push all NFA-states in NFAset set onto stateStack
//   while(stateStack is not empty) {
//     pop the top element p
//     if(p is an accept state) {
//       acceptState = true
//     }
//     if(there's an epsilon transition from p to q) {
//       if(q isn't in NFAset) {
//         add q to NFAset
//         push q onto stateStack
//       }
//     }
//   }

  CompactStack<int> stateStack; // stack of NFA-states remaining to be tested

  for(Iterator<size_t> it = NFAset.getIterator(); it.hasNext(); ) {                       // 1
    stateStack.push((int)it.next());
  }

  acceptIndex = -1;
  while(!stateStack.isEmpty()) {                                                          // 2
    const NFAState &p = *m_NFA[stateStack.pop()];                                         // 3
    if(p.isAcceptState() && ((acceptIndex < 0) || (p.getAcceptIndex() < acceptIndex))) {  // 4
      acceptIndex = p.getAcceptIndex();
    }

    if(p.getEdge() == EDGE_EPSILON) {                                                     // 5
      if(p.m_next) {
        int next = p.m_next->getID();
        if(!NFAset.contains(next)) {                                                      // 6
          NFAset.add(next);                                                               // 7
          stateStack.push(next);                                                          // 8
        }
      }
      if(p.m_next2) {
        int next = p.m_next2->getID();
        if(!NFAset.contains(next)) {                                                      // 6
          NFAset.add(next);                                                               // 7
          stateStack.push(next);                                                          // 8
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
  fixupTransitions();
}

void DFA::makeInitialGroups() {
  for(size_t i = 0; i < m_states.size(); i++) {
    m_inGroup.add(-1);
  }

  for(size_t i = 0; i < m_states.size(); i++) {
    BitSet newSet(m_states.size());
    for(size_t j = 0; j < i; j++) {
      if(m_states[i].m_acceptIndex == m_states[j].m_acceptIndex) {
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
}

  // Reduce the size of the m_states to the number of groups.
  // Consider the first element of each group (state) to be a
  // "representative" state. Insert this state to the new transition table,
  // modifying all the transitions, so that the successors
  // are the groups within which the old state is found.
void DFA::fixupTransitions() {
  Array<DFAState> newStates;

  for(UINT g = 0; g < m_groups.size(); g++) {
    Iterator<size_t> it = m_groups[g].getIterator();
    UINT state = (UINT)it.next();       // there is at least one state in each group
    newStates.add(DFAState(g));
    DFAState &newState     = newStates.last();
    DFAState &oldState     = m_states[state];
    newState.m_acceptIndex = oldState.m_acceptIndex;
    for(int c = 0; c < MAX_CHARS; c++) {
      int trans = oldState.m_transition[c];
      newState.m_transition[c] = (trans == FAILURE) ? FAILURE : m_inGroup[trans];
    }
  }
  m_states = newStates;
}
