#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

Expression &Expression::toCanonicalForm() {
  if((getTreeForm() == TREEFORM_CANONICAL) || (getRoot() == NULL)) {
    return *this;
  }
  initDynamicOperations(TREEFORM_CANONICAL);
  setRoot(toCForm(getRoot()));
  pruneUnusedNodes();
  setTreeForm(TREEFORM_CANONICAL);
  return *this;
}

// ----------------------------------------- toCForm ------------------------------------------------------------------

// Replace PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQRT,SQR,EXP,SEC,CSC,COT and negative constanst,
// with combinations of SUM, (unary MINUS), PRODUCT, POW, SIN,COS,TAN, and positive constants
SNode Expression::toCForm(const ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER         :
  case NAME           : return n;

  case MINUS          : if(n->isUnaryMinus()) return -toCForm(n->left());
                        // NB continue case

  case PLUS           :
  case SUM            : return toCFormSum(n);

  case PRODUCT        :
  case PROD           :
  case QUOT           : 
  case POW            : 
  case SQR            : 
  case ROOT           : 
  case SQRT           : 
  case EXP            : return toCFormProduct(n);

  case SEC            : return sec(toCForm(n->left()));
  case CSC            : return csc(toCForm(n->left()));
  case COT            : return cot(toCForm(n->left()));

  case POLY           : return toCFormPoly(n);

  default             : return toCFormTreeNode(n);
  }
}

SNode Expression::toCFormSum(const ExpressionNode *n) const {
  AddentArray a;
  return getSum(toCFormSum(a, n, true));
}

AddentArray &Expression::toCFormSum(AddentArray &result, const ExpressionNode *n, bool positive) const {
  switch(n->getSymbol()) {
  case SUM:
    { const AddentArray &a = n->getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        toCFormSum(result, e->getNode(), e->isPositive() == positive);
      }
    }
    break;
  case PLUS:
    toCFormSum(toCFormSum(result, n->left(), positive), n->right(), positive);
    break;
  case MINUS:
    if(n->isUnaryMinus()) {
      toCFormSum(result, n->left() , !positive);
    } else {
      toCFormSum(toCFormSum(result, n->left(), positive), n->right(), !positive);
    }
    break;
  default:
    result.add(toCForm(n), positive);
    break;
  }
  return result;
}

SNode Expression::toCFormProduct(const ExpressionNode *n) const {
  FactorArray a; 
  return getProduct(toCFormProduct(a, n, _1()));
}

FactorArray &Expression::toCFormProduct(FactorArray &result, const ExpressionNode *n, const SNode &exponent) const {
  switch(n->getSymbol()) {
  case PRODUCT:
    { const FactorArray &a = n->getFactorArray();
      if(exponent.isOne()) {
        result.addAll(a);
      } else {
        for(size_t i = 0; i < a.size(); i++) toCFormProduct(result, a[i], exponent);
      }
    }
    break;
  case PROD:
    toCFormProduct(toCFormProduct(result, n->left() , exponent), n->right(), exponent);
    break;
  case QUOT:
    toCFormProduct(toCFormProduct(result, n->left() , exponent), n->right(), -exponent);
    break;
  case POW :
    toCFormPower(result, n, exponent);
    break;
  case SQR:
    toCFormProduct(result, sqr(toCForm(n->left())), exponent);
    break;
  case EXP:
    toCFormProduct(result, exp(toCForm(n->left())), exponent);
    break;
  case ROOT:
    toCFormRoot(result, n, exponent);
    break;
  case SQRT:
    toCFormProduct(result, sqrt(toCForm(n->left())), exponent);
    break;
  case NUMBER:
    result.add(n, exponent);
    break;
  default:
    result.add(toCForm(n), exponent);
    break;
  }
  return result;
}

// n.symbol == POW, 
FactorArray &Expression::toCFormPower(FactorArray &result, const ExpressionNode *n, const SNode &exponent) const {
  DEFINEMETHODNAME;
  const SNode base = toCForm(n->left());
  const SNode expo = toCForm(n->right());

  switch(base.getSymbol()) {
  case POW :
    result.add(base.left(), base.right() * exponent * expo);
    break;
  case ROOT:
    throwMethodInvalidArgumentException(s_className, method, _T("ROOT in canonical form. n:<%s>"), n->toString().cstr());
    break;
  case PRODUCT:
    { const FactorArray &factors = base.getFactorArray();
      const SNode        newExpo = exponent * expo;
      for(size_t i = 0; i < factors.size(); i++) toCFormPower(result, factors[i], newExpo);
    }
    break;
  default                   :
    result.add(base, exponent * expo);
    break;
  }
  return result;
}

// n.symbol = ROOT
FactorArray &Expression::toCFormRoot(FactorArray &result, const ExpressionNode *n, const SNode &exponent) const {
  DEFINEMETHODNAME;
  const SNode rad  = toCForm(n->left());
  const SNode root = toCForm(n->right());

  switch(rad.getSymbol()) {
  case POW :
    result.add(rad.left(), rad.right() * exponent / root);
    break;
  case ROOT:
    throwMethodInvalidArgumentException(s_className, method, _T("ROOT in canonical form. n:<%s>"), n->toString().cstr());
    break;
  case PRODUCT:
    { const FactorArray &factors = rad.getFactorArray();
      const SNode        newExpo = exponent / root;
      for(size_t i = 0; i < factors.size(); i++) toCFormPower(result, factors[i], newExpo);
    }
    break;
  default                   :
    result.add(rad, exponent / root);
    break;
  }
  return result;
}

SNode Expression::toCFormPoly(const ExpressionNode *n) const {
  const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
  ExpressionNodeArray newCoefficientArray(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    newCoefficientArray.add(toCForm(coefficientArray[i]));
  }
  const ExpressionNode *newArgument = toCForm(n->getArgument());
  return getPoly(n, newCoefficientArray, newArgument);
}

SNode Expression::toCFormTreeNode(const ExpressionNode *n) const {
  const ExpressionNodeArray &a = n->getChildArray();
  ExpressionNodeArray newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(toCForm(a[i]));
  }
  return getTree(n, newChildArray);
}

// -------------------------------------------------------------------------------------------------------

class CanonicalFormChecker : public ExpressionNodeHandler {
private:
  BitSet m_illegalSymbolSet;
  String m_error;
  bool   m_ok;
public:
  CanonicalFormChecker();
  bool handleNode(const ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

CanonicalFormChecker::CanonicalFormChecker() : m_illegalSymbolSet(ExpressionTables->getTerminalCount() + 1) {
  static const ExpressionInputSymbol table[] = {
    PLUS
   ,PROD
   ,QUOT
   ,ROOT
   ,SQRT
   ,SQR
   ,EXP
   ,SEC
   ,CSC
   ,COT
  };
  for(int i = 0; i < ARRAYSIZE(table); i++) {
    m_illegalSymbolSet.add(table[i]);
  }
  m_ok = true;
}

bool CanonicalFormChecker::handleNode(const ExpressionNode *n, int level) {
  if(m_illegalSymbolSet.contains(n->getSymbol()) || n->isBinaryMinus()) {
    m_error = format(_T("Illegal symbol in canonical form:<%s>. node=<%s>"), n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void Expression::checkIsCanonicalForm() const {
  CanonicalFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}
