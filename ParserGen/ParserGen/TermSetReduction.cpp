#include "stdafx.h"
#include "TermSetReduction.h"

TermSetReduction::operator ParserActionArray() const {
  ParserActionArray result(getLegalTermCount());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

String TermSetReduction::toString() const {
  return format(_T("Reduce by %u on %s (%u terminals)"), m_prod, m_grammar.symbolSetToString(m_termSet).cstr(), m_setSize);
}

UINT TermSetReductionArray::getLegalTermCount() const {
  UINT sum = 0;
  for(auto it = getIterator(); it.hasNext();) {
    sum += it.next().getLegalTermCount();
  }
  return sum;
}

static inline int legalTermCountReverseCmp(const TermSetReduction &i1, const TermSetReduction &i2) {
  return (int)i2.getLegalTermCount() - (int)i1.getLegalTermCount();
}

void TermSetReductionArray::sortByLegalTermCount() {
  if(size() > 1) {
    sort(legalTermCountReverseCmp);
  }
}

String TermSetReductionArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += format(_T("   %s\n"), it.next().toString().cstr());
  }
  return result;
}
