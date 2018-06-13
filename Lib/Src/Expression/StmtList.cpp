#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

class NameSubstituter : public ExpressionNodeHandler {
private:
  const String m_fromName,m_toName;
public:
  NameSubstituter(const String &from, const String to) : m_fromName(from), m_toName(to) {
  }
  bool handleNode(ExpressionNode *n, int level);
};

bool NameSubstituter::handleNode(ExpressionNode *n, int level) {
  if(n->isMarked()) return true;
  if(n->isName() && (n->getName() == m_fromName)) {
    n->setName(m_toName);
  }
  return true;
}

SNodeArray &StmtList::removeUnusedAssignments() {
  DISABLEDEBUGSTRING(*this);
  bool stable;
  do {
    stable = true;
    const size_t childCount = size(), stmtCount = childCount-1;
    for(size_t i = 0; i < stmtCount; i++) {
      SNode assign = (*this)[i];
      SNode expr   = assign.right();
      if(expr.isName()) {
        getTree().unmarkAll();
        assign.left().mark();
        getTree().traverseTree(NameSubstituter(assign.left().getName(), expr.getName()));
        getTree().unmarkAll();
        remove(i);
        stable = false;
        break;
      }
    }
  } while(!stable);

  for(int i = (int)size()-1; i--;) { // Remove unused assignments
    const SNode  &stmt = (*this)[i];
    const String &varName = stmt.left().getName();
    bool isUsed = false;
    for(size_t j = i+1; j < size(); j++) {
      if((*this)[j].dependsOn(varName)) {
        isUsed = true;
        break;
      }
    }
    if(!isUsed) {
      remove(i);
    }
  }
  ENABLEDEBUGSTRING(*this);
  return *this;
}

}; // namespace Expr
