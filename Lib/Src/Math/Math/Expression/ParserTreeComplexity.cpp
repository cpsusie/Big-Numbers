#include "pch.h"
#include <Math/Expression/ParserTree.h>

ParserTreeComplexity::ParserTreeComplexity(const ParserTree &tree) {
  m_nodeCount = tree.getNodeCount(true, EOI);
  m_nameCount = tree.getNodeCount(true, NAME,EOI);
  m_treeDepth = tree.getRoot() ? tree.getRoot()->getMaxTreeDepth() : 0;
}

int parserTreeComplexityCmp(const ParserTreeComplexity &rs1, const ParserTreeComplexity &rs2) {
  int c = (rs1.m_nodeCount + 2 * rs1.m_nameCount) - (rs2.m_nodeCount + 2 * rs2.m_nameCount);
  if(c) return c;
  return rs1.m_treeDepth - rs2.m_treeDepth;
}

ParserTreeComplexity ParserTree::getComplexity() const {
  return ParserTreeComplexity(*this);
}
