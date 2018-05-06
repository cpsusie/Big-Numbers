#include "pch.h"
#include <Math/Expression/Expression.h>

Expression Expression::getDerived(const String &name, bool reduceResult /*=false*/) const {
  if(getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Cannot get derived of an expression returning boolean"));
  }
  Expression result = *this;
  result.toStandardForm();
  result.setMachineCode(false);
  SNode e(result.getRoot());
  result.setRoot(e.D(name));
  result.pruneUnusedNodes();
  result.buildSymbolTable();
  result.setState(PS_DERIVED);

  if(reduceResult) {
    result.reduce();
  }
  if(isMachineCode()) {
    result.toStandardForm();
    result.setMachineCode(true);
  }
  return result;
}
