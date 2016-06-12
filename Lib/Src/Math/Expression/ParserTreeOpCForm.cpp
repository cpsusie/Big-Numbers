#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// -------------------------------------Operators for CanonicalForm-------------------------------------------------------------------------
// Not using PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQR,SQRT,EXP,COT,CSC,SEC
const ExpressionNode *ParserTree::minusC(const ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    return numberExpression(-n->getNumber());
  case MINUS:
    assert(n->isUnaryMinus());
    return n->left();

  case SUM       :
    { const AddentArray &a = n->getAddentArray();
      AddentArray newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) { // change sign for all elements in list
        const SumElement *e = a[i]; 
        newAddentArray.add(e->getNode(),!e->isPositive());
      }
      return getSum(newAddentArray);
    }

  case PRODUCT   :
    { const FactorArray &factors = n->getFactorArray();
      const int          index   = factors.findFactorWithChangeableSign();
      FactorArray        newFactors(factors.size());
      for(size_t i = 0; i < factors.size(); i++) {
        const ExpressionFactor *factor = factors[i];
        if(i != index) {
          newFactors.add(factor);
        } else {
          newFactors.add(minusC(factor->base()),factor->exponent());
        }
      }
      if(index < 0) {
        newFactors.add(getMinusOne());
      }
      return getProduct(newFactors);
    }

  case POLY      :
    { const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
      ExpressionNodeArray       newCoefficientArray(coefficientArray.size());
      for(size_t i = 0; i < coefficientArray.size(); i++) {
        newCoefficientArray.add(minusC(coefficientArray[i]));
      }
      const ExpressionNode *x = n->getArgument();
      return fetchPolyNode(newCoefficientArray, x);
    }

  case INDEXEDSUM:
    return indexedSum(n->child(0), n->child(1), minusC(n->child(2)));
  }

  return unaryMinus(n);
}

const ExpressionNode *ParserTree::reciprocalC(const ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER :
    if(n->isRational()) {
      return numberExpression(reciprocal(n->getRational()));
    }
    break;

  case PRODUCT       :
    { const FactorArray &factors = n->getFactorArray();
      FactorArray newFactors;
      for(size_t i = 0; i < factors.size(); i++) {
        const ExpressionFactor *factor = factors[i];
        newFactors.add(factor->base(), minusC(factor->exponent()));
      }
      return getProduct(newFactors);
    }
  case INDEXEDPRODUCT:
    return indexedProduct(n->child(0), n->child(1), reciprocalC(n->child(2)));
  case POW    : // reciprocal(l^r) = l^-r
    return powerC(n->left(), minusC(n->right()));
  }
  return quotientC(getOne(), n);
}

const ExpressionNode *ParserTree::sumC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() + n2->getRational());
  } else if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  }
  AddentArray a;
  a.add(n1, true);
  a.add(n2, true);
  return getSum(a);
}

const ExpressionNode *ParserTree::differenceC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() - n2->getRational());
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minusC(n2);
  }
  AddentArray a;
  a.add(n1, true );
  a.add(n2, false);
  return getSum(a);
}

const ExpressionNode *ParserTree::productC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isZero() || n2->isZero()) {
    return getZero();
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() * n2->getRational());
  }
  FactorArray a;
  a.add(n1);
  a.add(n2);
  return getProduct(a);
}

const ExpressionNode *ParserTree::quotientC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isZero()) {
    return getZero();
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() / n2->getRational());
  } else if(n2->isOne()) {
    return n1;
  }
  FactorArray a;
  a.add(n1);
  if(n2->getSymbol() != PRODUCT) {
    a.add((n2->getSymbol() == POW) ? fetchFactorNode(n2->left(), minusC(n2->right())) : fetchFactorNode(n2, getMinusOne()));
  } else {
    const FactorArray &a2 = n2->getFactorArray();
    for(size_t i = 0; i < a2.size(); i++) {
      const ExpressionFactor *f = a2[i];
      a.add(f->base(), minusC(f->exponent()));
    }
  }
  return getProduct(a);
}

const ExpressionNode *ParserTree::modulusC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  return binaryExpression(MOD, n1, n2);
}

const ExpressionNode *ParserTree::sqrC(const ExpressionNode *n) const {
  return powerC(n, getTwo());
}

const ExpressionNode *ParserTree::sqrtC(const ExpressionNode *n) const {
  return rootC(n, getTwo());
}

const ExpressionNode *ParserTree::powerC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n2->isZero()) {
    return getOne();
  } else if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocalC(n1);
  } else if(n1->getSymbol() == POW) {
    return binaryExpression(POW, n1->left(), productC(n1->right(), n2));
  } else {
    return binaryExpression(POW, n1, n2);
  }
}

const ExpressionNode *ParserTree::rootC(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocalC(n1);
  } 
  return powerC(n1, reciprocalC(n2));
}

const ExpressionNode *ParserTree::expC(const ExpressionNode *n) const {
  return powerC(constExpression(_T("e")), n);
}

const ExpressionNode *ParserTree::cotC(const ExpressionNode *n) const {
  return reciprocalC(functionExpression(TAN, n));
}

const ExpressionNode *ParserTree::cscC(const ExpressionNode *n)  const {
  return reciprocalC(functionExpression(SIN, n));
}

const ExpressionNode *ParserTree::secC(const ExpressionNode *n)  const {
  return reciprocalC(functionExpression(COS, n));
}
