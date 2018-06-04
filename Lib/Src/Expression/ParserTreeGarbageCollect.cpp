#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

class SetMark : public ExpressionNodeHandler {
private:
  const bool m_setMark;
public:
  SetMark(bool setMark) : m_setMark(setMark) {
  }
  bool handleNode(ExpressionNode *n, int level);
};

bool SetMark::handleNode(ExpressionNode *n, int level) {
  if(m_setMark) {
    n->mark();
  } else {
    n->unMark();
  }
  return true;
}

void ParserTree::pruneUnusedNodes() {
  debugLog(_T("Garbage collection. Before.(#Nodes:%6d.\n"), getNodeTableSize());
  unmarkAll();

  traverseTree(SetMark(true));

  markSimpleConstants();
  deleteUnmarked();
  unmarkAll();
  debugLog(_T(" After:%3d\n"), getNodeTableSize());
}

#define MARKCONSTANT(n) if(n) n->mark()

void ParserTree::markSimpleConstants() {
  MARKCONSTANT(m_minusOne);
  MARKCONSTANT(m_zero    );
  MARKCONSTANT(m_one     );
  MARKCONSTANT(m_two     );
  MARKCONSTANT(m_ten     );
  MARKCONSTANT(m_half    );
  MARKCONSTANT(m_false   );
  MARKCONSTANT(m_true    );
}

void ParserTree::unmarkAll() {
  for(size_t i = 0; i < m_nodeTable.size(); i++) m_nodeTable[i]->unMark();
  m_symbolTable.unmarkAllReferencedNodes();
}

void ParserTree::deleteUnmarked() {
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

}; // namespace Expr
