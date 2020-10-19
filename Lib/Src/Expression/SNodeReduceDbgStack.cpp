#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

#if defined(TRACE_REDUCTION_CALLSTACK)

void ReductionStack ::push(const TCHAR *method, const String &s, const ExpressionNode *n) {
  const ReductionStackElement *oldTop    = topPointer();
  const int                    oldHeight = getHeight();

  __super::push(ReductionStackElement(oldHeight, method, s,n));

  const ReductionStackElement *newTop    = topPointer();
  const int                    newHeight = getHeight();
  notifyPropertyChanged(REDUCTION_STACKTOP  ,  oldTop   ,  newTop   );
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ReductionStack::pop(const TCHAR *method) {
  const ReductionStackElement *oldTop    = topPointer();
  const int                    oldHeight = getHeight();
  const ReductionStackElement *newTop    = topPointer(1);
  notifyPropertyChanged(REDUCTION_STACKTOP  ,  oldTop   ,  newTop   );

  __super::pop();

  const int                    newHeight  = getHeight();
  notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
}

void ReductionStack::clear() {
  const ReductionStackElement *oldTop    = topPointer();
  const int                    oldHeight = getHeight();
  const ReductionStackElement *newTop    = nullptr;
  if(newTop != oldTop) {
    notifyPropertyChanged(REDUCTION_STACKTOP  ,  oldTop   ,  newTop   );
  }

  Stack::clear();

  const int                     newHeight = 0;
  if(newHeight != oldHeight) {
    notifyPropertyChanged(REDUCTION_STACKHIGHT, &oldHeight, &newHeight);
  }
}

const ReductionStackElement *ReductionStack::topPointer(UINT index) const {
  return (index >= getHeight()) ? nullptr : &top(index);
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
