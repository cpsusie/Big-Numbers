#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

SNode SNode::reduceToPoly() const {
  if(getSymbol() == POLY) {
    return *this;
  }
  Number c;
//  TODO
 return NULL;
}

}; // namespace Expr
