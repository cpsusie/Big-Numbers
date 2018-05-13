#pragma once

#include "ExpressionNode.h"

namespace Expr {

class ExpressionFactor : public ExpressionNodeTree {
public:
  ExpressionFactor(ExpressionNode *base, ExpressionNode *exponent);

  inline ExpressionNode *base() {
    return child(0);
  }
  inline const ExpressionNode *base() const {
    return child(0);
  }

  inline ExpressionNode *exponent() {
    return child(1);
  }
  inline const ExpressionNode *exponent() const {
    return child(1);
  }

  bool hasOddExponent() const {
    return exponent()->isNumber() && exponent()->getNumber().isOdd();
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEFACTOR;
  }

  void dumpNode(String &s, int level) const;
};

}; // namespace Expr
