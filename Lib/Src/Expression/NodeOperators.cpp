#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNode *NodeOperators::getZero(ExpressionNode *n) { // static
  return n->getTree()->getZero();
}

ExpressionNode *NodeOperators::getOne(ExpressionNode *n) { // static
  return n->getTree()->getOne();
}

ExpressionNode *NodeOperators::getMinusOne(ExpressionNode *n) { // static
  return n->getTree()->getMinusOne();
}

ExpressionNode *NodeOperators::getTwo(ExpressionNode *n) { // static
  return n->getTree()->getTwo();
}

ExpressionNode *NodeOperators::getTen(ExpressionNode *n) { // static
  return n->getTree()->getTen();
}

ExpressionNode *NodeOperators::numberExpression(ExpressionNode *n, const Number &v) { // static
  return n->getTree()->numberExpression(v);
}

ExpressionNode *NodeOperators::numberExpression(ExpressionNode *n, INT64 v) { // static
  return n->getTree()->numberExpression(v);
}

ExpressionNode *NodeOperators::unaryExpr(ExpressionInputSymbol symbol, ExpressionNode *n) { // static
  return n->getTree()->unaryExpr(symbol, n);
}

ExpressionNode *NodeOperators::binaryExpr(ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2) { // static
  return n1->getTree()->binaryExpr(symbol,n1,n2);
}

ExpressionNode *NodeOperators::functionExpr(ExpressionInputSymbol symbol, ExpressionNode *n) { // static
  return n->getTree()->functionExpr(symbol, n);
}

}; // namespace Expr
