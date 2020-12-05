#pragma once

#include "SymbolDataPair.h"

class NTermNewStatePair {
private:
  union {
    SymbolDataPair m_data;
    UINT           m_hashCode;
  };
public:
  inline NTermNewStatePair() : m_hashCode(0) {
  }
  inline NTermNewStatePair(UINT nterm, UINT newState)
    : m_data(nterm, PA_NEWSTATE, newState)
  {
  }
  inline ULONG hashCode() const {
    return m_hashCode;
  }
  inline bool operator==(const NTermNewStatePair &ntns) const {
    return hashCode() == ntns.hashCode();
  }
  inline bool operator!=(const NTermNewStatePair &ntns) const {
    return !(*this == ntns);
  }
  inline ActionType getType()             const { return m_data.getType();             }
  inline UINT       getNTerm()            const { return m_data.getSymbol();           }
  inline UINT       getNewState()         const { return m_data.getNewState();         }
  inline void       setNewState(UINT newState) {
    m_data.setNewState(newState);
  }
  inline String     toString(const AbstractSymbolNameContainer &nameContainer) const {
    return m_data.toString(nameContainer);
  }
};

inline int ntermNewStatePairCompareNTerm(const NTermNewStatePair &s1, const NTermNewStatePair &s2) {
  return (int)s1.getNTerm() - (int)s2.getNTerm();
}
