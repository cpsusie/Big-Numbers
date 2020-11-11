#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionSymbolTable.h>

namespace Expr {

class SetMark : public ExpressionNodeHandler {
private:
  const bool m_setMark;
public:
  SetMark(bool setMark) : m_setMark(setMark) {
  }
  bool handleNode(ExpressionNode *n) override;
};

bool SetMark::handleNode(ExpressionNode *n) {
  if(m_setMark) {
    n->mark();
  } else {
    n->unMark();
  }
  return true;
}

void ParserTree::pruneUnusedNodes() {
#if defined(_DEBUG)
  debugLog(_T("Garbage collection. Before.(#Nodes:%6d, rationalMap.size:%4zu).\n")
          ,getNodeTableSize(),m_rationalConstantMap.size());
#endif // _DEBUG
  unmarkAll();

  traverseTree(SetMark(true));

  markSimpleConstants();
  markNonRootNodes();
  deleteUnmarked();
  unmarkAll();
#if defined(_DEBUG)
  debugLog(_T(" After:(#Nodes:%6d, rationalMap.size:%4zu).\n")
          ,getNodeTableSize(),m_rationalConstantMap.size());
#endif // _DEBUG
}

#define MARKCONSTANT(n) if(n) n->mark()

void ParserTree::markSimpleConstants() {
  MARKCONSTANT(m_false   );
  MARKCONSTANT(m_true    );
}

void ParserTree::addNonRootNode(ExpressionNode *n) {
  m_nonRootNodes.add(n);
}

void ParserTree::markNonRootNodes() {
  for(size_t i = 0; i < m_nonRootNodes.size(); i++) {
    m_nonRootNodes[i]->traverseExpression(SetMark(true));
  }
}

void ParserTree::unmarkAll() const {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    m_nodeTable[i]->unMark();
  }
  getSymbolTable().unmarkAllReferencedNodes();
}

void ParserTree::deleteUnmarked() {
  m_rationalConstantMap.removeUnmarked();
  CompactArray<ExpressionNode*> tmp = m_nodeTable;
  m_nodeTable.clear();
  for(size_t i = 0; i < tmp.size(); i++) {
    ExpressionNode *n = tmp[i];
    if(n->isMarked()) {
      m_nodeTable.add(n);
    } else {
      SAFEDELETE(n);
    }
  }
  tmp.clear();
}

void RationalConstantMap::removeUnmarked() {
  for(auto it = getIterator(); it.hasNext();) {
    Entry<Rational, ExpressionNodeNumber*> &e = it.next();
    if(!e.getValue()->isMarked()) {
      it.remove();
    }
  }
}

}; // namespace Expr
