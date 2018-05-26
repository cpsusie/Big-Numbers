#include "pch.h"
#include <Math/Expression/Expression.h>

namespace Expr {

Expression Expression::getDerived(const String &name, bool optimize /*=true*/) const {
  if(getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Cannot get derived of an expression returning boolean"));
  }
  Expression result = *this;
  result.setTreeForm(TREEFORM_STANDARD);
  result.setMachineCode(false);
  SNode e(result.getRoot());
  result.setRoot(e.D(name).node());
  result.pruneUnusedNodes();
  result.buildSymbolTable();
  result.setState(PS_DERIVED);

  if(optimize) {
    result.reduce();
  }
  if(isMachineCode()) {
    result.setTreeForm(TREEFORM_STANDARD);
    result.setMachineCode(true);
  }
  return result;
}

}; // namespace Expr
