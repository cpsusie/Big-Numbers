#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

#ifdef TRACE_REDUCTION_CALLSTACK

void ReductionStack ::push(const TCHAR *method, const String &s, const ExpressionNode *n) {
  const int oldHeight = getHeight();
  __super::push(ReductionStackElement(method, s,n));
  const int newHeight = getHeight();
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ReductionStack::pop(const TCHAR *method) {
  const int oldHeight = getHeight();
  __super::pop();
  const int newHeight = getHeight();
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ReductionStack::clear() {
  const int oldHeight = getHeight();
  Stack::clear();
  const int newHeight = getHeight();
  if(newHeight != oldHeight) {
    notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
  }
}

const TCHAR *ReductionStack::getRawName(const TCHAR *str) { // static
  const TCHAR *s = _tcsrchr(str,':');
  return s ? s+1 : str;
}

void ParserTree::clearAllBreakPoints() {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    m_nodeTable[i]->clearBreakPoint();
  }
}

#endif // TRACE_REDUCTION_CALLSTACK

}; // namespace Expr
