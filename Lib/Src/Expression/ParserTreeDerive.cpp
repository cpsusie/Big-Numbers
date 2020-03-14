#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ParserTree &ParserTree::getDerived(const String &name) {
  setTreeForm(TREEFORM_STANDARD);
  m_expression.setMachineCode(false);
  SNode e(getRoot());
  setRoot(e.D(name).node());
  pruneUnusedNodes();
  buildSymbolTable();
  setState(PS_DERIVED);
  return *this;
}

}; // namespace Expr
