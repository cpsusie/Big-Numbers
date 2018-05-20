#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeStmtList::ExpressionNodeStmtList(ParserTree *tree, const SNodeArray &childArray) 
  : ExpressionNodeTree(tree, STMTLIST, childArray)
  , m_returnType(childArray.last().getReturnType())
{
}
ExpressionNodeStmtList::ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeStmtList *src)
  : ExpressionNodeTree(tree, src)
  , m_returnType(src->getReturnType())
{
}

ExpressionNode *ExpressionNodeStmtList::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeStmtList(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeStmtList::evaluateReal() const {
  if(m_returnType != EXPR_RETURN_REAL) UNSUPPORTEDOP();
  SNode last = doAssignments();
  return last.evaluateReal();
}

bool ExpressionNodeStmtList::evaluateBool() const {
  if(m_returnType != EXPR_RETURN_BOOL) UNSUPPORTEDOP();
  SNode last = doAssignments();
  return last.evaluateBool();
}

SNode ExpressionNodeStmtList::doAssignments() const {
  const SNodeArray &stmtList = getChildArray();

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    stmtList[i].doAssignment();
  }
  return stmtList.last();
}

String ExpressionNodeStmtList::toString() const {
  String result;
  const SNodeArray &list = getChildArray();
  for(size_t i = 0; i < list.size(); i++) {
    result += list[i].toString();
    result += _T(";\n");
  }
  return result;
}

}; // namespace Expr
