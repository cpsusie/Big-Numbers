#pragma once

#include "NFAState.h"

class NFA : public CompactArray<NFAState*> {
private:
  NFAStatePool &m_statePool;
  int           m_idCounter;
  void        addIfNotMarked(NFAState *s);
  static void unmarkAll(     NFAState *s);
  NFA(const NFA &src);      // not defined
  NFA &operator=(NFA &src); // not defined
  friend class PatternParser;
public:
  NFA(NFAStatePool &statePool) : m_statePool(statePool) {}
  NFA(NFAStatePool &statePool, NFAState *start);
  void create(NFAState *start);
  ~NFA(); // All elements in array will be released
          // clear and remove will NOT release the elements
};
