#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ---------------------------------------- Operators for StandardForm ----------------------------------------------------
// Not using SUM and PRODUCT nodes
ExpressionNode *ParserTree::minusS(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->isZero()) {
      return n;
    } else if(n->isOne()) {
      return getMinusOne();
    } else if(n->isMinusOne()) {
      return getOne();
    } else {
      return numberExpression(-n->getNumber());
    }
  case MINUS:
    if(n->isUnaryMinus()) {
      return n->left();
    } else {
      return differenceS(n->right(), n->left());
    }
  default:
    return unaryExpression(MINUS, n);
  }
}

ExpressionNode *ParserTree::reciprocalS(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER:
    { const Number &v = n->getNumber();
      if(v.isRational()) {
        return numberExpression(reciprocal(v.getRationalValue()));
      }
    }
    break;

  case QUOT: return quotientS(n->right(), n->left());
  case EXP : return functionExpression(EXP      , minusS(n->left()));
  case POW : return powerS(   n->left() , minusS(n->right()));
  case ROOT: return rootS(    n->left() , minusS(n->right()));
  case COS : return functionExpression(SEC, n->left());
  case SIN : return functionExpression(CSC, n->left());
  case TAN : return functionExpression(COT, n->left());
  case COT : return functionExpression(TAN, n->left());
  case SEC : return functionExpression(COS, n->left());
  case CSC : return functionExpression(SIN, n->left());
  }
  return quotientS(getOne(), n);
}

ExpressionNode *ParserTree::sumS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() + n2->getRational());
  }
  if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isUnaryMinus()) {
    return differenceS(n2, n1->left());
  } else if(n2->isUnaryMinus()) {
    return differenceS(n1, n2->left());
  } else {
    return binaryExpression(PLUS, n1, n2);
  }
}

ExpressionNode *ParserTree::differenceS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() - n2->getRational());
  }
  if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minusS(n2);
  } else if(n2->isUnaryMinus()) {
    return sumS(n1, n2->left());
  } else {
    return binaryExpression(MINUS, n1, n2);
  }
}

ExpressionNode *ParserTree::productS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() * n2->getRational());
  }
  if(n1->isZero() || n2->isZero()) {
    return getZero();
  } else if(n1->isOne()) {
    return n2;
  } else if(n2->isOne()) {
    return n1;
  } else if(n1->isMinusOne()) {
    return minusS(n2);
  } else if(n2->isMinusOne()) {
    return minusS(n1);
  } else {
    return binaryExpression(PROD, n1, n2);
  }
}

ExpressionNode *ParserTree::quotientS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1->getRational() / n2->getRational());
  }
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return minusS(n1);
  } else if(n1->isZero()) {
    return n1;
  } else {
    return binaryExpression(QUOT, n1, n2);
  }
}

ExpressionNode *ParserTree::modulusS(ExpressionNode *n1, ExpressionNode *n2) {
  return binaryExpression(MOD, n1, n2);
}

ExpressionNode *ParserTree::sqrS(ExpressionNode *n) {
  return functionExpression(SQR, n);
}

ExpressionNode *ParserTree::sqrtS(ExpressionNode *n) {
  return functionExpression(SQRT, n);
}

ExpressionNode *ParserTree::powerS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isInteger()) {
    return numberExpression(pow(n1->getRational(), n2->getNumber().getIntValue()));
  }
  if(n1->isEulersConstant()) {
    if(n2->isZero()) {
      return getOne();
    } else if(n2->isOne()) {
      return n1;
    }
    return expS(n2);
  } else if(n1->getSymbol() == POW) {
    return binaryExpression(POW, n1->left(), productS(n1->right(), n2));
  }

  if(n2->isNumber()) {
    const Number &e = n2->getNumber();
    if(e.isRational()) {
      const Rational eR = e.getRationalValue();
      if(eR.isZero()) {
        return getOne();
      } else if(eR == 1) {
        return n1;
      } else if(eR == -1) {
        return reciprocalS(n1);
      } else if(eR.getNumerator() == 1) {
        return rootS(n1, numberExpression(eR.getDenominator()));
      } else if(eR.getNumerator() == -1) {
        return reciprocalS(rootS(n1, numberExpression(eR.getDenominator())));
      }
    }
  }
  return binaryExpression(POW, n1, n2);
}

ExpressionNode *ParserTree::rootS(ExpressionNode *n1, ExpressionNode *n2) {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocalS(n1);
  }
  return binaryExpression(ROOT, n1, n2);
}

ExpressionNode *ParserTree::expS(ExpressionNode *n) {
  return functionExpression(EXP, n);
}

ExpressionNode *ParserTree::cotS(ExpressionNode *n)  {
  return functionExpression(COT, n);
}

ExpressionNode *ParserTree::cscS(ExpressionNode *n)  {
  return functionExpression(CSC, n);
}

ExpressionNode *ParserTree::secS(ExpressionNode *n)  {
  return functionExpression(SEC, n);
}
