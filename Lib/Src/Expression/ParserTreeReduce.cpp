#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

#ifdef __NEVER__

class MainReducer : public ExpressionTransformer {
private:
  ParserTree     &m_tree;
  ExpressionState m_state;
public:
  MainReducer(ParserTree *tree) : m_tree(*tree) {
    m_state = EXPR_MAINREDUCTION1;
  }
  SNode transform(SNode n) {
    return m_expr.reduce(n);
  }
  ExpressionState getState() const {
    return m_state;
  }
  void setState(ExpressionState state) {
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
    const ExpressionVariableArray variables = getSymbolTable().getAllVariables();

    const ParserTreeForm startTreeForm = getTreeForm();
    if(startTreeForm != TREEFORM_CANONICAL) {
      toCanonicalForm();
      setState(EXPR_CANONICALFORM);
    }
    checkIsCanonicalForm();

    MainReducer mainTransformer(this);

    iterateTransformation(mainTransformer);

    mainTransformer.setState(EXPR_MAINREDUCTION2);
    setRoot(multiplyParentheses(getRoot()));
    iterateTransformation(mainTransformer);

  /*
    tmp = replaceRationalFactors(iterateTransformation(getRoot(), RationalPowersReducer(this)));
    if(!treesEqual(getRoot(), tmp, false)) {
      setRoot(tmp);
      checkIsCanonicalForm();
    }
  */

    if(startTreeForm == TREEFORM_STANDARD) {
      toStandardForm();
      setState(EXPR_STANDARDFORM);
    }

    buildSymbolTable();
    pruneUnusedNodes();

    setState(EXPR_REDUCTIONDONE);
  } catch(...) {
    clear();
    throw;
  }
}

void ParserTree::iterateTransformation(ExpressionTransformer &transformer) {
  DEFINEMETHODNAME;

  const int                  maxIterations = 30;
  SNode                      oldRoot       = getRoot();
  ParserTree                 bestReduction = *this;
  const ParserTreeComplexity startComplexity(bestReduction);
  ParserTreeComplexity       bestComplexity(startComplexity);
  STARTREDUCTION();

  setReduceIteration(0);
  setRoot(oldRoot);
  pruneUnusedNodes();

  setState(transformer.getState());
  checkIsCanonicalForm();

  SNode n    = getRoot();
  bool  done = false;

  for(setReduceIteration(1); getReduceIteration() < maxIterations; setReduceIteration(getReduceIteration()+1)) {
    SNode n1 = transformer.transform(n);

    setRoot(n1);
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
  setRoot(oldRoot);
  if(!done) {
    throwException(_T("%s:Maxiterations reached for expression <%s>"), method, n.toString().cstr());
  }

  pruneUnusedNodes();

  if(bestComplexity < startComplexity) {
    *this = bestReduction;
  }
}

#endif
