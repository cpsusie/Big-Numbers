#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNode *NodeOperators::getZero(ExpressionNode *n) { // static
  return numberExpr(n,0);
}

ExpressionNode *NodeOperators::getOne(ExpressionNode *n) { // static
  return numberExpr(n,1);
}

ExpressionNode *NodeOperators::getMinusOne(ExpressionNode *n) { // static
  return numberExpr(n,-1);
}

ExpressionNode *NodeOperators::getTwo(ExpressionNode *n) { // static
  return numberExpr(n,2);
}

ExpressionNode *NodeOperators::getHalf(ExpressionNode *n) { // static
  return n->getTree().numberExpr(Rational(1,2));
}

ExpressionNode *NodeOperators::getTen(ExpressionNode *n) { // static
  return numberExpr(n,10);
}

ExpressionNode *NodeOperators::numberExpr(ExpressionNode *n, const Number &v) { // static
  return n->getTree().numberExpr(v);
}

ExpressionNode *NodeOperators::numberExpr(ExpressionNode *n, INT64 v) { // static
  return n->getTree().numberExpr(v);
}

ExpressionNode *NodeOperators::unaryExpr(ExpressionInputSymbol symbol, ExpressionNode *n) { // static
  return n->getTree().unaryExpr(symbol, n);
}

ExpressionNode *NodeOperators::binaryExpr(ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2) { // static
  return n1->getTree().binaryExpr(symbol,n1,n2);
}

ExpressionNode *NodeOperators::functionExpr(ExpressionInputSymbol symbol, ExpressionNode *n) { // static
  return n->getTree().functionExpr(symbol, n);
}

}; // namespace Expr
