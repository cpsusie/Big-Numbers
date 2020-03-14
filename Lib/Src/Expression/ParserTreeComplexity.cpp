#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/ParserTreeComplexity.h>

namespace Expr {

ParserTreeComplexity::ParserTreeComplexity(const ParserTree &tree) {
  m_nodeCount = tree.getNodeCount(true);
  m_nameCount = tree.getNodeCount(true, &ExpressionSymbolSet(NAME,EOI));
  m_treeDepth = tree.getRoot() ? tree.getRoot()->getMaxTreeDepth() : 0;
}

int ParserTreeComplexity::compare(const ParserTreeComplexity &tc) const {
  const int c = (int)(m_nodeCount + 2 * m_nameCount) - (int)(tc.m_nodeCount + 2 * tc.m_nameCount);
  if(c) return c;
  return (int)m_treeDepth - (int)tc.m_treeDepth;
}

String ParserTreeComplexity::toString() const {
  return format(_T("#nodes:%3u, #names:%2u, treedepth:%u\n"), m_nodeCount, m_nameCount, m_treeDepth);
}

}; // namespace Expr
