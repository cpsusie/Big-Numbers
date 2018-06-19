#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionParser.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

UINT ParserTree::getTerminalCount() { // static
  return ExpressionParser::getTables().getTerminalCount();
};

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

void ParserTree::reduce() {

  try {
    const ExpressionVariableArray oldVariables = getSymbolTable().getAllVariables();

    const ParserTreeForm startTreeForm = getTreeForm();
    if(startTreeForm != TREEFORM_CANONICAL) {
      setTreeForm(TREEFORM_CANONICAL);
    }
    checkIsCanonicalForm();

    MainReducer mainTransformer(this);

    iterateTransformation(mainTransformer);
    if(getNodeCount() > 1) {
      mainTransformer.setState(PS_MAINREDUCTION2);
      setRoot(SNode(getRoot()).multiplyParentheses().node());
      iterateTransformation(mainTransformer);
    }

    if(startTreeForm == TREEFORM_STANDARD) {
      setTreeForm(TREEFORM_STANDARD);
    }

    buildSymbolTable(&oldVariables);
    pruneUnusedNodes();

    setState(PS_REDUCTIONDONE);
  } catch(...) {
    clear();
    throw;
  }
}

class TreeWithScore : public ParserTree {
private:
  const ParserTreeComplexity m_complexity;
public:
  TreeWithScore(const ParserTree &tree) : ParserTree(tree), m_complexity(tree) {
  }
  const ParserTreeComplexity &getComplexity() const {
    return m_complexity;
  }
};

class ReductionArray : public Array<TreeWithScore> {
public:
  int find(const ParserTree &tree) const;
};

int ReductionArray::find(const ParserTree &tree) const {
  for(UINT i = 0; i < size(); i++) {
    if(tree.equal((*this)[i])) {
      return i;
    }
  }
  return -1;
}

void ParserTree::iterateTransformation(ParserTreeTransformer &transformer) {
  DEFINEMETHODNAME;

  checkIsCanonicalForm();
  pruneUnusedNodes();

  const int      maxIterations = 30;
  size_t         bestIndex     = 0;
  ReductionArray reductionArray;
  reductionArray.add(TreeWithScore(*this));

  STARTREDUCTION();

  setReduceIteration(0);
  setState(transformer.getState());

  for(setReduceIteration(1); getReduceIteration() < maxIterations; setReduceIteration(getReduceIteration()+1)) {
    setRoot(transformer.transform(getRoot()).node());
    setState(transformer.getState());
    checkIsCanonicalForm();
    if(reductionArray.find(*this) >= 0) {
      break;
    } else {
      reductionArray.add(*this);
      if(reductionArray.last().getComplexity() < reductionArray[bestIndex].getComplexity()) {
        bestIndex = reductionArray.size() - 1;
      }
    }
    pruneUnusedNodes();
  }
  *this = reductionArray[bestIndex];
}

}; // namespace Expr
