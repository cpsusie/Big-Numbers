#include "stdafx.h"
#include "Grammar.h"
#include "UIntPermutation.h"

// used by hashmap
class StateCoreComparator : public Comparator<const LR1State *> {
public:
  int compare(const LR1State * const &s1, const LR1State * const &s2) final {
    return s1->hasSameCore(*s2) ? 0 : 1;
  }
  AbstractComparator *clone() const final {
    return new StateCoreComparator();
  }
};

static ULONG stateCoreHash(const LR1State * const &s) { // used by hashmap
  return s->hashCode();
}

StateHashMap::StateHashMap(size_t capacity) : HashMap(stateCoreHash, StateCoreComparator(), capacity) {
}

UINT StateMap::add(LR1State *state) {
  const UINT stateIndex = getStateCount();
  __super::add(state);
  m_map.put(state, stateIndex);
  return stateIndex;
}

static int stateCmpByIndex(LR1State * const &s1, LR1State * const &s2) {
  return (int)(s1->m_index) - (int)(s2->m_index);
}

void StateMap::reorderStates(const UIntPermutation &permutation) {
  permutation.validate();
  assert(permutation.size() == getStateCount());
  for(auto statep : *this) {
    const UINT oldIndex = statep->m_index;
    statep->m_index     = permutation[statep->m_index];
    for(auto itemp : statep->m_items) {
      if(itemp->m_newState >= 0) {
        itemp->m_newState = permutation[itemp->m_newState];
      }
    }
    UINT *indexp = m_map.get(statep);
    assert(*indexp == oldIndex);
    *indexp = permutation[*indexp];
  }
  sort(stateCmpByIndex);
}
