#include "stdafx.h"
#include "Grammar.h"
#include "TermActionPairArray.h"
#include "TermSetReduction.h"

TermSetReduction::TermSetReduction(const Grammar &grammar, UINT prod, UINT term0)
  : m_grammar(grammar                )
  , m_termSet(grammar.getTermCount() )
  , m_prod(   prod                   )
  , m_setSize(0                      )
{
  addTerm(term0);
}


TermSetReduction::operator TermActionPairArray() const {
  TermActionPairArray result(getLegalTermCount());
  const Action        action(PA_REDUCE, m_prod);
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(TermActionPair((UINT)it.next(), action));
  }
  return result;
}

String TermSetReduction::toString() const {
  return format(_T("Reduce by %4u on %s (%u terminals)")
               ,m_prod
               ,m_grammar.symbolSetToString(m_termSet).cstr()
               ,getLegalTermCount()
               );
}
