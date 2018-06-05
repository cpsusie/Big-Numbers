#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

SNodeArray &StmtList::removeUnusedAssignments() {
  DISABLEDEBUGSTRING(*this);
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
