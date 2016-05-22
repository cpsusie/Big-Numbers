#pragma once

#include "ExpressionNode.h"

class ExpressionFactor : public ExpressionNodeTree {
private:
  DECLARECLASSNAME;
public:
  ExpressionFactor(const ExpressionNode *base, const ExpressionNode *exponent)
    : ExpressionNodeTree(base->getTree(), POW, base, exponent?exponent:base->getTree()->getOne(), NULL) {
  }

  inline const ExpressionNode *base() const {
    return child(0);
  }

  inline const ExpressionNode *exponent() const {
    return child(1);
  }

  bool hasOddExponent() const {
    return exponent()->isNumber() && exponent()->getNumber().isOdd();
  }

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEFACTOR;
  }

  void dumpNode(String &s, int level) const;
};
