#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

bool ExpressionNodeBoolExpr::isConstant() const {
  switch(getSymbol()) {
  case NOT     :
    return left()->isConstant();
  case AND     :
    { const bool lConst = left()->isConstant();
      const bool rConst = right()->isConstant();
      if(!lConst) {        // left not const => this is const if right is constant false.     (value = false)
        return rConst && !right()->evaluateBool();
      } else if(!rConst) { // left const && right not const => this is const if left = false. (value = false)
        return !left()->evaluateBool();
      } else {             // left const && right const => this is const
        return true;
      }
    }
  case OR      :
    { const bool lConst = left()->isConstant();
      const bool rConst = right()->isConstant();
      if(!lConst) {        // left not const => this is const if right is constant true.     (value = true)
        return rConst && right()->evaluateBool();
      } else if(!rConst) { // left const && right not const => this is const if left = true. (value = true)
        return left()->evaluateBool();
      } else {             // left const && right const => this is const
        return true;
      }
    }
  case EQ      :
  case NE      :
  case LE      :
  case LT      :
  case GE      :
  case GT      : return left()->isConstant() && right()->isConstant();
  default      : throwUnknownSymbolException(__TFUNCTION__);
  }
  return false;
}

bool ExpressionNodeBoolExpr::evaluateBool() const {
  switch(getSymbol()) {
  case NOT     : return !left()->evaluateBool();
  case AND     : return left()->evaluateBool() && right()->evaluateBool();
  case OR      : return left()->evaluateBool() || right()->evaluateBool();
  case EQ      : return left()->evaluateReal() == right()->evaluateReal();
  case NE      : return left()->evaluateReal() != right()->evaluateReal();
  case LE      : return left()->evaluateReal() <= right()->evaluateReal();
  case LT      : return left()->evaluateReal() <  right()->evaluateReal();
  case GE      : return left()->evaluateReal() >= right()->evaluateReal();
  case GT      : return left()->evaluateReal() >  right()->evaluateReal();
  default      : throwUnknownSymbolException(__TFUNCTION__);
  }
  return true;
}

}; // namespace Expr
