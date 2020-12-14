#include "stdafx.h"
#include "Grammar.h"

LR1Item::LR1Item(const Grammar *grammar, bool kernelItem, UINT prod, UINT dot, const TermSet &la)
: m_grammar(*grammar)
, m_la(m_grammar.getTermCount())
{
  setValues(kernelItem, prod, dot, la);
}

// Is item = "A -> alfa . a beta [la]"
// if so, set term to a
bool LR1Item::isShiftItem(UINT &term) const {
  const Production &prod = m_grammar.getProduction(m_prod);
  if(m_dot == prod.getLength()) {
    return false;
  }
  const UINT symbol = prod.m_rightSide[m_dot];
  if(!m_grammar.isTerminal(symbol)) {
    return false;
  } else {
    term = symbol;
    return true;
  }
}

// Is item = "A -> alfa . a beta [la]"
bool LR1Item::isShiftItem() const {
  const Production &prod = m_grammar.getProduction(m_prod);
  return (m_dot < prod.getLength()) && m_grammar.isTerminal(prod.m_rightSide[m_dot]);
}

// Is item = "A -> alfa . [la]"
bool LR1Item::isReduceItem() const {
  return m_dot == m_grammar.getProduction(m_prod).getLength();
}

// Assume item = "A -> alfa . x beta [la]".
// Return x (terminal or nonterminal)
UINT LR1Item::getShiftSymbol() const {
  return m_grammar.getProduction(m_prod).m_rightSide[m_dot].m_index;
}

// Assume item = A -> alfa . B beta [la]
// Computes first1(beta la)
TermSet LR1Item::first1() const {
  const Production &prod   = m_grammar.getProduction(m_prod);
  const UINT        length = prod.getLength();
  TermSet           result(m_grammar.getTermCount());
  for(UINT k = m_dot+1; k < length; k++) {
    const UINT symbol = prod.m_rightSide[k];
    if(m_grammar.isTerminal(symbol)) {
      result += symbol;
      return result;
    } else { // nonterminal
      const GrammarSymbol &nt = m_grammar.getSymbol(symbol);
      result += nt.m_first1;
      if(!nt.m_deriveEpsilon) {
        return result;
      }
    }
  }
  return result += m_la;
}

String LR1Item::toString(UINT flags) const {
  String result;
  const Production &prod = m_grammar.getProduction(m_prod);
  result = format(_T(" (%3u)%c %-15s -> "), m_prod, m_kernelItem?'K':' ', m_grammar.getSymbolName(prod.m_leftSide).cstr());
  for(UINT i = 0; i < m_dot; i++) {
    result += m_grammar.getSymbolName(prod.m_rightSide[i]);
    result += ' ';
  }
  result += '.';
  const UINT n     = prod.getLength();
  TCHAR      delim = 0;
  for(UINT i = m_dot; i < n; i++, delim = ' ') {
    if(delim) result += delim;
    result += m_grammar.getSymbolName(prod.m_rightSide[i]);
  }
  if(flags & DUMP_LOOKAHEAD) {
    result += m_grammar.symbolSetToString(m_la);
  }
  if((flags & DUMP_SUCC) && (m_newState >= 0)) {
    result += format(_T(" -> %d"), getNewState()); // ie not reduce-item
  }
  return result;
}
