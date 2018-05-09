#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

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

void ReductionStack::reset(ParserTree *tree) {
  const int oldHeight = getHeight();
  Stack::clear();
  const int newHeight = getHeight();
  if(newHeight != oldHeight) {
    notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
  }
}

void ParserTree::clearAllBreakPoints() {
  for (size_t i = 0; i < m_nodeTable.size(); i++) {
    m_nodeTable[i]->clearBreakPoint();
  }
}

#endif // TRACE_REDUCTION_CALLSTACK
