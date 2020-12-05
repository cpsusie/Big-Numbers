#pragma once

#include <AbstractParserTables.h>

class SymbolDataPair {
private:
  ActionType m_type   :  3;
  UINT       m_symbol : 14;
  UINT       m_index  : 15; // either new state    (m_type = PA_SHIFT or PA_NEWSTATE)
                            // or production index (m_type = PA_REDUCE)
public:
  inline SymbolDataPair() {
  }
  inline SymbolDataPair(UINT symbol, ActionType type, UINT index)
    : m_symbol(symbol)
    , m_type(  type  )
    , m_index( index )
  {
    assert(m_type == type);
  }
  inline SymbolDataPair(UINT symbol, Action a)
    : m_symbol(symbol      )
    , m_type(  a.getType() )
    , m_index( a.getIndex())
  {
  }

  ACTIONRESULTTRAITS()

  inline UINT     getSymbol()    const { return m_symbol;                      }
  inline Action   getAction()    const { return Action(getType(), getIndex()); }

  // Assume old symbol and term are both terminal-symbols and type is PA_SHIFT or PA_REDUCE
  void            setTerm(UINT term, const AbstractSymbolNameContainer &nameContainer);
  // Assume type == PA_SHIFT or PA_NEWSTATE
  void            setNewState(UINT newState);
  String toString(const AbstractSymbolNameContainer &nameContainer) const;
};
