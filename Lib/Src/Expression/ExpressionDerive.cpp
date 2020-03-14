#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

Expression Expression::getDerived(const String &name, bool optimize /*=true*/) const {
  if(getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Cannot get derived of an expression returning boolean"));
  }
  if(!hasSyntaxTree()) {
    throwException(_T("Cannot get derived of an expression. No syntaxtree present"));
  }
  Expression result = *this;
  ParserTree *tree = result.m_tree;
  tree->getDerived(name);

  if(optimize) {
    tree->reduce();
  }
  if(isMachineCode()) {
    tree->setTreeForm(TREEFORM_STANDARD);
    result.setMachineCode(true);
  }
  return result;
}

}; // namespace Expr
