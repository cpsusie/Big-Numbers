#pragma once

#include "SymbolDataPair.h"

class TermActionPair {
private:
  union {
    SymbolDataPair m_data;
    UINT           m_hashCode;
  };
public:
  inline TermActionPair() : m_hashCode(0) {
  }
  inline TermActionPair(UINT term, ActionType type, UINT action)
    : m_data(term, type, action)
  {
    assert(m_data.isValidActionType());
  }
  inline TermActionPair(UINT term, Action a)
    : m_data(term, a)
  {
  }
  inline ULONG hashCode() const {
    return m_hashCode;
  }
  inline bool operator==(const TermActionPair &a) const {
    return hashCode() == a.hashCode();
  }
  inline bool operator!=(const TermActionPair &a) const {
    return !(*this == a);
  }
  inline ActionType getType()             const { return m_data.getType();             }
  inline bool       isShiftAction()       const { return m_data.isShiftAction();       }
  inline bool       isReduceAction()      const { return m_data.isReduceAction();      }
  inline bool       isAcceptAction()      const { return m_data.isAcceptAction();      }
  inline UINT       getTerm()             const { return m_data.getSymbol();           }
  inline UINT       getIndex()            const { return m_data.getIndex();            }
  inline UINT       getReduceProduction() const { return m_data.getReduceProduction(); }
  inline UINT       getNewState()         const { return m_data.getNewState();         }
  inline Action     getAction()           const { return m_data.getAction();           }
  // Assume old symbol and term are both terminal-symbols
  inline void       setTerm(UINT term, const AbstractSymbolNameContainer &nameContainer) {
    m_data.setTerm(term, nameContainer);
  }
  // Assume this is a shift action
  inline void       setNewState(UINT newState) {
    m_data.setNewState(newState);
  }
  inline String     toString(const AbstractSymbolNameContainer &nameContainer) const {
    return m_data.toString(nameContainer);
  }
};

inline int termActionPairCompareTerm(const TermActionPair &p1, const TermActionPair &p2) {
  return (int)p1.getTerm() - (int)p2.getTerm();
}
