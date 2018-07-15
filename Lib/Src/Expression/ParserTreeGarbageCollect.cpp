#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

class SetMark : public ExpressionNodeHandler {
private:
  const bool m_setMark;
public:
  SetMark(bool setMark) : m_setMark(setMark) {
  }
  bool handleNode(ExpressionNode *n);
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
#ifdef _DEBUG
  debugLog(_T("Garbage collection. Before.(#Nodes:%6d, rationalMap.size:%4zu).\n")
          ,getNodeTableSize(),m_rationalConstantMap.size());
#endif // _DEBUG
  unmarkAll();

  traverseTree(SetMark(true));

  markSimpleConstants();
  deleteUnmarked();
  unmarkAll();
#ifdef _DEBUG
  debugLog(_T(" After:(#Nodes:%6d, rationalMap.size:%4zu).\n")
          ,getNodeTableSize(),m_rationalConstantMap.size());
#endif // _DEBUG
}

#define MARKCONSTANT(n) if(n) n->mark()

void ParserTree::markSimpleConstants() {
  MARKCONSTANT(m_false   );
  MARKCONSTANT(m_true    );
}

void ParserTree::unmarkAll() const {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    m_nodeTable[i]->unMark();
  }
  m_symbolTable.unmarkAllReferencedNodes();
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
  for(Iterator<Entry<Rational, ExpressionNodeNumber*> > it = getEntryIterator(); it.hasNext();) {
    Entry<Rational, ExpressionNodeNumber*> &e = it.next();
    if(!e.getValue()->isMarked()) {
      it.remove();
    }
  }
}

}; // namespace Expr
