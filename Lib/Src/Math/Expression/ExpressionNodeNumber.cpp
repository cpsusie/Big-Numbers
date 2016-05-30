#include "pch.h"
#include <Math/Expression/ParserTree.h>

int ExpressionNodeNumber::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return numberCmp(getNumber(), n->getNumber());
}

const ExpressionNode *ExpressionNodeNumber::clone(const ParserTree *tree) const {
  return new ExpressionNodeNumber(tree, m_number);
}

bool ExpressionNodeNumber::traverseExpression(ExpressionNodeHandler &handler, int level) const {
  return handler.handleNode(this, level);
}

void ExpressionNodeNumber::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NUMBER:%s\n"), getNumber().toString().cstr());
}

int ExpressionNodeBoolean::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return getBool() - n->getBool();
}

const ExpressionNode *ExpressionNodeBoolean::clone(const ParserTree *tree) const {
  return new ExpressionNodeBoolean(tree, getBool());
}

bool ExpressionNodeBoolean::traverseExpression(ExpressionNodeHandler &handler, int level) const {
  return handler.handleNode(this, level);
}

void ExpressionNodeBoolean::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("BOOLEAN:%s\n"), boolToStr(getBool()));
}

#ifdef _DEBUG

void ExpressionNodeNumber::initDebugString() {
  m_debugString = getNumber().toString();
}

void ExpressionNodeBoolean::initDebugString() {
  m_debugString = toString();
}

#endif

