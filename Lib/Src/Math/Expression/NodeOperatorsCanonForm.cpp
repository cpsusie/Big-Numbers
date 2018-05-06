#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// -------------------------------------Operators for CanonicalForm-------------------------------------------------------------------------
// Not using PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQR,SQRT,EXP,COT,CSC,SEC

class NodeOperatorsCanonForm : public NodeOperators {
private:
  static inline ExpressionNode   *getSum(         ExpressionNode *n, AddentArray &aa) {
    return n->getTree()->getSum(aa);
  }
  static inline ExpressionNode   *getProduct(     ExpressionNode *n, FactorArray &fa) {
    return n->getTree()->getProduct(fa);
  }
  static inline ExpressionNode   *unaryMinus(ExpressionNode *n) {
    return n->getTree()->unaryMinus(n);
  }
  static inline ExpressionNode   *fetchPolyNode(ExpressionNodeArray &coef, ExpressionNode *x) {
    return x->getTree()->fetchPolyNode(coef, x);
  }
  static inline ExpressionNode   *indexedSum(ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree()->indexedSum(assign,end,expr);
  }
  static inline ExpressionNode   *indexedProduct(ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree()->indexedProduct(assign,end,expr);
  }
  static inline ExpressionFactor *fetchFactorNode(ExpressionNode *base, ExpressionNode *exponent) {
    return base->getTree()->fetchFactorNode(base,exponent);
  }
  static inline ExpressionNode   *constExpression(ExpressionNode *n, const TCHAR *name) {
    return n->getTree()->constExpression(name);
  }
public:
  ExpressionNode *minus(     ExpressionNode *n) const;
  ExpressionNode *reciprocal(ExpressionNode *n) const;
  ExpressionNode *sqr(       ExpressionNode *n) const;
  ExpressionNode *sqrt(      ExpressionNode *n) const;
  ExpressionNode *exp(       ExpressionNode *n) const;
  ExpressionNode *exp10(     ExpressionNode *n) const;
  ExpressionNode *exp2(      ExpressionNode *n) const;
  ExpressionNode *cot(       ExpressionNode *n) const;
  ExpressionNode *csc(       ExpressionNode *n) const;
  ExpressionNode *sec(       ExpressionNode *n) const;

  ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const;
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_CANONICAL;
  }
};

class NodeOperatorsCanonNumForm : public NodeOperatorsCanonForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsCanonForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    return numberExpression(n,-n->getNumber());
  case MINUS:
    assert(n->isUnaryMinus());
    return n->left();

  case SUM       :
    { const AddentArray &a = n->getAddentArray();
      AddentArray newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) { // change sign for all elements in list
        SumElement *e = a[i];
        newAddentArray.add(e->getNode(),!e->isPositive());
      }
      return getSum(n,newAddentArray);
    }

  case PRODUCT   :
    { const FactorArray &factors = n->getFactorArray();
      const int          index   = factors.findFactorWithChangeableSign();
      FactorArray        newFactors(factors.size());
      for(size_t i = 0; i < factors.size(); i++) {
        ExpressionFactor *factor = factors[i];
        if(i != index) {
          newFactors.add(factor);
        } else {
          newFactors.add(minus(factor->base()),factor->exponent());
        }
      }
      if(index < 0) {
        newFactors.add(getMinusOne(n));
      }
      return getProduct(n,newFactors);
    }

  case POLY      :
    { const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
      ExpressionNodeArray       newCoefficientArray(coefficientArray.size());
      for(size_t i = 0; i < coefficientArray.size(); i++) {
        newCoefficientArray.add(minus(coefficientArray[i]));
      }
      ExpressionNode *x = n->getArgument();
      return fetchPolyNode(newCoefficientArray, x);
    }

  case INDEXEDSUM:
    return indexedSum(n->child(0), n->child(1), minus(n->child(2)));
  }

  return unaryMinus(n);
}

ExpressionNode *NodeOperatorsCanonForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER :
    if(n->isRational()) {
      return numberExpression(n,::reciprocal(n->getRational()));
    }
    break;

  case PRODUCT       :
    { const FactorArray &factors = n->getFactorArray();
      FactorArray newFactors;
      for(size_t i = 0; i < factors.size(); i++) {
        ExpressionFactor *factor = factors[i];
        newFactors.add(factor->base(), minus(factor->exponent()));
      }
      return getProduct(n,newFactors);
    }
  case INDEXEDPRODUCT:
    return indexedProduct(n->child(0), n->child(1), reciprocal(n->child(2)));
  case POW    : // reciprocal(l^r) = l^-r
    return power(n->left(), minus(n->right()));
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() + n2->getRational());
  } else if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  }
  AddentArray a;
  a.add(n1, true);
  a.add(n2, true);
  return getSum(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() - n2->getRational());
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  }
  AddentArray a;
  a.add(n1, true );
  a.add(n2, false);
  return getSum(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() * n2->getRational());
  }
  FactorArray a;
  a.add(n1);
  a.add(n2);
  return getProduct(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() / n2->getRational());
  } else if(n2->isOne()) {
    return n1;
  }
  FactorArray a;
  a.add(n1);
  if(n2->getSymbol() != PRODUCT) {
    a.add((n2->getSymbol() == POW) ? fetchFactorNode(n2->left(), minus(n2->right())) : fetchFactorNode(n2, getMinusOne(n2)));
  } else {
    const FactorArray &a2 = n2->getFactorArray();
    for(size_t i = 0; i < a2.size(); i++) {
      ExpressionFactor *f = a2[i];
      a.add(f->base(), minus(f->exponent()));
    }
  }
  return getProduct(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpression(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsCanonForm::sqr(ExpressionNode *n) const {
  return power(n, getTwo(n));
}

ExpressionNode *NodeOperatorsCanonForm::sqrt(ExpressionNode *n) const {
  return root(n, getTwo(n));
}

ExpressionNode *NodeOperatorsCanonForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isZero()) {
    return getOne(n1);
  } else if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  } else if(n1->getSymbol() == POW) {
    return binaryExpression(POW, n1->left(), prod(n1->right(), n2));
  } else {
    return binaryExpression(POW, n1, n2);
  }
}

ExpressionNode *NodeOperatorsCanonForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return power(n1, reciprocal(n2));
}

ExpressionNode *NodeOperatorsCanonForm::exp(ExpressionNode *n) const {
  return power(constExpression(n,_T("e")), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp10(ExpressionNode *n) const {
  return power(getTen(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp2(ExpressionNode *n) const {
  return power(getTwo(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::cot(ExpressionNode *n) const {
  return reciprocal(functionExpression(TAN, n));
}

ExpressionNode *NodeOperatorsCanonForm::csc(ExpressionNode *n) const {
  return reciprocal(functionExpression(SIN, n));
}

ExpressionNode *NodeOperatorsCanonForm::sec(ExpressionNode *n) const {
  return reciprocal(functionExpression(COS, n));
}

static const NodeOperatorsCanonForm    canonFormOps;
static const NodeOperatorsCanonNumForm canonNumFormOps;

const NodeOperators *NodeOperators::s_canonForm    = &canonFormOps;
const NodeOperators *NodeOperators::s_canonNumForm = &canonNumFormOps;
