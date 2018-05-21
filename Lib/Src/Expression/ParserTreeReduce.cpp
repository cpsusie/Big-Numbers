#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>
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

/*
class RationalPowersReducer : public ExpressionTransformer {
private:
  ParserTree *m_expr;
public:
  RationalPowersReducer(ParserTree *expr) : m_expr(expr) {
  }
  const ExpressionNode *transform(const ExpressionNode *n) {
    return m_expr->replaceRationalPowers(n);
  }
  ExpressionState getState() const {
    return EXPR_RATIONALPOWERSREDUCTION;
  }
};
*/

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

    mainTransformer.setState(PS_MAINREDUCTION2);
    setRoot(SNode(getRoot()).multiplyParentheses().node());
    iterateTransformation(mainTransformer);

  /*
    tmp = replaceRationalFactors(iterateTransformation(getRoot(), RationalPowersReducer(this)));
    if(!treesEqual(getRoot(), tmp, false)) {
      setRoot(tmp);
      checkIsCanonicalForm();
    }
  */

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

void ParserTree::iterateTransformation(ParserTreeTransformer &transformer) {
  DEFINEMETHODNAME;

  const int                  maxIterations = 3;
  SNode                      oldRoot       = getRoot();
  ParserTree                 bestReduction = *this;
  const ParserTreeComplexity startComplexity(bestReduction);
  ParserTreeComplexity       bestComplexity(startComplexity);
  STARTREDUCTION(this);

  setReduceIteration(0);
  setRoot(oldRoot.node());
  pruneUnusedNodes();

  setState(transformer.getState());
  checkIsCanonicalForm();

  SNode n    = getRoot();
  bool  done = false;

  for(setReduceIteration(1); getReduceIteration() < maxIterations; setReduceIteration(getReduceIteration()+1)) {
    SNode n1 = transformer.transform(n);

    setRoot(n1.node());
    setState(transformer.getState());
    checkIsCanonicalForm();

    if(n1 == n) {
      done = true;
      break;
    } else {
      const ParserTreeComplexity complexity(*this);
      if(complexity < bestComplexity) {
        bestReduction  = *this;
        bestComplexity = complexity;
      }
      n = n1;
    }
  }
  setRoot(oldRoot.node());
  if(!done) {
    throwException(_T("%s:Maxiterations reached for expression <%s>"), method, n.toString().cstr());
  }

  pruneUnusedNodes();

  if(bestComplexity < startComplexity) {
    *this = bestReduction;
  }
}

}; // namespace Expr
