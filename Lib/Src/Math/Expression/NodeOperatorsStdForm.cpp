#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ---------------------------------------- Operators for StandardForm ----------------------------------------------------
// Not using SUM and PRODUCT nodes

class NodeOperatorsStdForm : public NodeOperators {
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
    return TREEFORM_STANDARD;
  }
};

class NodeOperatorsStdNumForm : public NodeOperatorsStdForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsStdForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->isZero()) {
      return n;
    } else if(n->isOne()) {
      return getMinusOne(n);
    } else if(n->isMinusOne()) {
      return getOne(n);
    } else {
      return numberExpression(n,-n->getNumber());
    }
  case MINUS:
    if(n->isUnaryMinus()) {
      return n->left();
    } else {
      return diff(n->right(), n->left());
    }
  default:
    return unaryExpression(MINUS, n);
  }
}

ExpressionNode *NodeOperatorsStdForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    { const Number &v = n->getNumber();
      if(v.isRational()) {
        return numberExpression(n,::reciprocal(v.getRationalValue()));
      }
    }
    break;

  case QUOT: return quot(n->right(), n->left());
  case EXP : return functionExpression(EXP      , minus(n->left()));
  case POW : return power(   n->left() , minus(n->right()));
  case ROOT: return root(    n->left() , minus(n->right()));
  case COS : return functionExpression(SEC, n->left());
  case SIN : return functionExpression(CSC, n->left());
  case TAN : return functionExpression(COT, n->left());
  case COT : return functionExpression(TAN, n->left());
  case SEC : return functionExpression(COS, n->left());
  case CSC : return functionExpression(SIN, n->left());
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsStdForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() + n2->getRational());
  }
  if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isUnaryMinus()) {
    return diff(n2, n1->left());
  } else if(n2->isUnaryMinus()) {
    return diff(n1, n2->left());
  } else {
    return binaryExpression(PLUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() - n2->getRational());
  }
  if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  } else if(n2->isUnaryMinus()) {
    return sum(n1, n2->left());
  } else {
    return binaryExpression(MINUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() * n2->getRational());
  }
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  } else if(n1->isOne()) {
    return n2;
  } else if(n2->isOne()) {
    return n1;
  } else if(n1->isMinusOne()) {
    return minus(n2);
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else {
    return binaryExpression(PROD, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() / n2->getRational());
  }
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else if(n1->isZero()) {
    return n1;
  } else {
    return binaryExpression(QUOT, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpression(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::sqr(ExpressionNode *n) const {
  return functionExpression(SQR, n);
}

ExpressionNode *NodeOperatorsStdForm::sqrt(ExpressionNode *n) const {
  return functionExpression(SQRT, n);
}

ExpressionNode *NodeOperatorsStdForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isInteger()) {
    return numberExpression(n1, pow(n1->getRational(), n2->getNumber().getIntValue()));
  }
  if(n1->isEulersConstant()) {
    if(n2->isZero()) {
      return getOne(n1);
    } else if(n2->isOne()) {
      return n1;
    }
    return exp(n2);
  } else if(n1->getSymbol() == POW) {
    return binaryExpression(POW, n1->left(), prod(n1->right(), n2));
  }

  if(n2->isNumber()) {
    const Number &e = n2->getNumber();
    if(e.isRational()) {
      const Rational eR = e.getRationalValue();
      if(eR.isZero()) {
        return getOne(n1);
      } else if(eR == 1) {
        return n1;
      } else if(eR == -1) {
        return reciprocal(n1);
      } else if(eR.getNumerator() == 1) {
        return root(n1, numberExpression(n1,eR.getDenominator()));
      } else if(eR.getNumerator() == -1) {
        return reciprocal(root(n1, numberExpression(n1,eR.getDenominator())));
      }
    }
  }
  return binaryExpression(POW, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return binaryExpression(ROOT, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::exp(ExpressionNode *n) const {
  return functionExpression(EXP, n);
}

ExpressionNode *NodeOperatorsStdForm::exp10(ExpressionNode *n) const {
  return functionExpression(EXP10, n);
}

ExpressionNode *NodeOperatorsStdForm::exp2(ExpressionNode *n) const {
  return functionExpression(EXP2, n);
}

ExpressionNode *NodeOperatorsStdForm::cot(ExpressionNode *n) const {
  return functionExpression(COT, n);
}

ExpressionNode *NodeOperatorsStdForm::csc(ExpressionNode *n) const {
  return functionExpression(CSC, n);
}

ExpressionNode *NodeOperatorsStdForm::sec(ExpressionNode *n) const {
  return functionExpression(SEC, n);
}

static const NodeOperatorsStdForm    stdFormOps;
static const NodeOperatorsStdNumForm stdNumFormOps;

const NodeOperators *NodeOperators::s_stdForm    = &stdFormOps;
const NodeOperators *NodeOperators::s_stdNumForm = &stdNumFormOps;
