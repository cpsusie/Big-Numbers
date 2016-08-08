#include "pch.h"
#include <Math/Expression/ParserTree.h>

int ExpressionNodeNumber::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return numberCmp(getNumber(), n->getNumber());
}

ExpressionNode *ExpressionNodeNumber::clone(ParserTree *tree) const {
  return new ExpressionNodeNumber(tree, m_number);
}

bool ExpressionNodeNumber::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeNumber::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NUMBER:%s valueIndex:%2d\n"), getNumber().toString().cstr(), getValueIndex());
}

int ExpressionNodeBoolean::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return getBool() - n->getBool();
}

ExpressionNode *ExpressionNodeBoolean::clone(ParserTree *tree) const {
  return new ExpressionNodeBoolean(tree, getBool());
}

bool ExpressionNodeBoolean::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeBoolean::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("BOOLEAN:%s\n"), boolToStr(getBool()));
}
