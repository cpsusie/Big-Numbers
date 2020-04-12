#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionSymbolTable.h>
#include <Math/Expression/ParserTreeComplexity.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

class MainReducer : public ParserTreeTransformer {
private:
  ParserTree     &m_tree;
  ParserTreeState m_state;
public:
  MainReducer(ParserTree *tree) : m_tree(*tree) {
    m_state = PS_MAINREDUCTION1;
  }
  SNode transform(SNode n) {
    return n.reduce();
  }
  ParserTreeState getState() const {
    return m_state;
  }
  void setState(ParserTreeState state) {
    m_state = state;
  }
};

// Minimal number of nodes in a valid syntax-tree. stmtlist + returnvalue
#define MINNODECOUNT 2

ParserTree &ParserTree::reduce() {
  try {
    const ExpressionVariableArray oldVariables = getSymbolTable().getAllVariables();

    const ParserTreeForm startTreeForm = getTreeForm();
    if(startTreeForm != TREEFORM_CANONICAL) {
      setTreeForm(TREEFORM_CANONICAL);
    }
    checkIsCanonicalForm();

    MainReducer mainTransformer(this);

    iterateTransformation(mainTransformer);
    if(getNodeCount() > MINNODECOUNT) {
      mainTransformer.setState(PS_MAINREDUCTION2);
      setRoot(SNode(getRoot()).multiplyParentheses().node());
      iterateTransformation(mainTransformer);
    }

    if(startTreeForm == TREEFORM_STANDARD) {
      setTreeForm(TREEFORM_STANDARD);
    }

    pruneUnusedNodes();
    buildSymbolTable(&oldVariables);

    setState(PS_REDUCTIONDONE);
  } catch(...) {
    releaseAll();
    throw;
  }
  return *this;
}

class ExpressionWithScore : public Expression {
private:
  const ParserTreeComplexity m_complexity;
public:
  ExpressionWithScore(const ExpressionWithScore &src)
    : Expression(src)
    , m_complexity(src.m_complexity)
  {
  }
  ExpressionWithScore(const ParserTree &tree)
    : Expression(tree.getExpression())
    , m_complexity(tree)
  {
  }
  const ParserTreeComplexity &getComplexity() const {
    return m_complexity;
  }
};

class ReductionArray : public Array<ExpressionWithScore> {
public:
  int find(const Expression &expr) const;
};

int ReductionArray::find(const Expression &expr) const {
  for(UINT i = 0; i < size(); i++) {
    if(expr.equal((*this)[i])) {
      return i;
    }
  }
  return -1;
}

void ParserTree::iterateTransformation(ParserTreeTransformer &transformer) {
  checkIsCanonicalForm();
  pruneUnusedNodes();

  const int      maxIterations = 30;
  size_t         bestIndex     = 0;
  ReductionArray reductionArray;
  reductionArray.add(ExpressionWithScore(*this));

  STARTREDUCTION();

  setReduceIteration(0);
  setState(transformer.getState());

  for(setReduceIteration(1); getReduceIteration() < maxIterations; setReduceIteration(getReduceIteration()+1)) {
    setRoot(transformer.transform(getRoot()).node());
    setState(transformer.getState());
    checkIsCanonicalForm();
    if(reductionArray.find(getExpression()) >= 0) {
      break;
    } else {
      reductionArray.add(*this);
      if(reductionArray.last().getComplexity() < reductionArray[bestIndex].getComplexity()) {
        bestIndex = reductionArray.size() - 1;
      }
    }
    pruneUnusedNodes();
    if(reductionArray[bestIndex].getComplexity().getNodeCount() <= MINNODECOUNT) { // cannot reduce further
      break;
    }
  }
  *this = *(reductionArray[bestIndex].getTree());
}

}; // namespace Expr
