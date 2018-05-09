#include "pch.h"
#include <Math/Expression/ParserTree.h>

Real &ExpressionNode::doAssignment() const {
  switch(getSymbol()) {
  case ASSIGN:
//    printf(_T("doasign:<%s> = %le\n"),n->left()->getName().cstr(),evaluateReal(n->right()));
    { ExpressionVariable &var = left()->getVariable();
      Real &ref = m_tree.getValueRef(var);
      if(!var.isConstant()) {
        ref = right()->evaluateReal();
      }
      return ref;
    }
    break;
  default:
    throwUnknownSymbolException(__TFUNCTION__);
  }
  static Real dummy;
  return dummy;
}
