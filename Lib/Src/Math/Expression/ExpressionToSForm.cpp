#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

#ifdef __NEVER__

Expression &Expression::toStandardForm() {
  if((getTreeForm() == TREEFORM_STANDARD) || (getRoot() == NULL)) {
    return *this;
  }
  initDynamicOperations(TREEFORM_STANDARD);
  setRoot(toSForm(getRoot()));
  pruneUnusedNodes();
  setTreeForm(TREEFORM_STANDARD);
  return *this;
}

// ----------------------------------------- toSForm ------------------------------------------------------------------
// Eliminate all Product- and Sum nodes
SNode Expression::toSForm(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER :
  case NAME   : return n;
  case SUM    : return toSFormSum(n);
  case PRODUCT: return toSFormProduct(n);
  case POW    : return toSFormPow(n);
  case POLY   : return toSFormPoly(n);
  default     : return toSFormTreeNode(n);
  }
}

static int compare2(SumElement * const &e1, SumElement * const &e2) {
  const bool p1 = e1->isPositive();
  const bool p2 = e2->isPositive();
  int c = p2 - p1;
  if(c) return c;
  return e1->compare(e2);
}

static int compareMany(SumElement * const &e1, SumElement * const &e2) {
  return e1->compare(e2);
}

static void sortAddentArrayStdForm(AddentArray &a) {
  if(a.size() == 2) {
    a.sort(compare2);
  } else {
    a.sort(compareMany);
  }
}

SNode Expression::toSFormSum(ExpressionNode *n) {
  AddentArray a = n->getAddentArray();
  if(a.size() == 0) {
    return getZero();
  } else {
    sortAddentArrayStdForm(a);
    SNode result = toSForm(a[0]->getNode()); // not createExpressionNode here. We'll get infinite recursion
    if(!a[0]->isPositive()) result = -result;
    for(size_t i = 1; i < a.size(); i++) {
      SumElement *e = a[i];
      SNode ne = toSForm(e->getNode());
      if(e->isPositive()) result += ne; else result -= ne;
    }
    return result;
  }
}

SNode Expression::toSFormProduct(ExpressionNode *n) {
  Rational constant = 1;

  const FactorArray &a = n->getFactorArray();
  FactorArray newArray;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f = a[i];
    Rational r;
    if(reducesToRationalConstant(f, &r)) {
      constant *= r;
    } else {
      newArray.add(f);
    }
  }
  FactorArray p,q;
  if(constant.getNumerator() != 1) {
    p.add(numberExpression(constant.getNumerator()));
  }
  if(constant.getDenominator() != 1) {
    q.add(numberExpression(constant.getDenominator()), getMinusOne());
  }

  p.addAll(newArray.selectConstantPositiveExponentFactors());
  p.addAll(newArray.selectNonConstantExponentFactors());
  q.addAll(newArray.selectConstantNegativeExponentFactors());

  return toSFormFactorArray(p, false) / toSFormFactorArray(q, true);
}

SNode Expression::toSFormFactorArray(FactorArray &a, bool changeExponentSign) {
  SNodeArray a1;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f = a[i];
    if(changeExponentSign) {
      a1.add(reciprocal(toSFormPow(f)));
    } else {
      a1.add(toSFormPow(f));
    }
  }
  if(a1.size() == 0) {
    return getOne();
  } else {
    SNode result = a1[0];
    for(size_t i = 1; i < a1.size(); i++) result *= a1[i];
    return result;
  }
}

SNode Expression::toSFormPow(ExpressionNode *n) {
  const SNode base     = toSForm(n->left());
  const SNode exponent = toSForm(n->right());

  Rational expoR;
  if(reducesToRationalConstant(exponent, &expoR)) {
    const __int64 &num = expoR.getNumerator();
    if(expoR.isInteger()) {
      switch(num) {
      case  0 : return _1();
      case  1 : return base;
      case -1:
        switch(base.getSymbol()) {
        case COS: return sec(base.left());
        case SIN: return csc(base.left());
        case TAN: return cot(base.left());
        default : return reciprocal(base);
        }
      case  2: return sqr(base);
      case -2: return reciprocal(sqr(base));
      default:
        if(num > 0) {
          return pow(base, numberExpression(expoR.getNumerator()));
        } else {
          return reciprocal(pow(base, numberExpression(-num)));
        }
      }
    } else { // r not integer
      const __int64 &den = expoR.getDenominator(); // > 0
      if(abs(num) == 1) {
        if(den == 2) {
          if(num == 1) {
            return sqrt(base);
          } else { // num == -1
            return reciprocal(sqrt(base));
          }
        } else {
          if(num == 1) {
            return root(base, numberExpression(den));
          } else { // num == -1
            return reciprocal(root(base, numberExpression(den)));
          }
        }
      } else { // num != 1 and num != -1 and r not integer
        if(num > 0) {
          return root(pow(base, numberExpression(num)), numberExpression(den));
        } else {
          return reciprocal(root(pow(base, numberExpression(-num)), numberExpression(den)));
        }
      }
    }
  } else { // exponent not rational constant
    if(exponent.getSymbol() == QUOT) {
      return root(pow(base, exponent.left()), exponent.right());
    } else {
      return pow(base, exponent);
    }
  }
}

SNode Expression::toSFormPoly(ExpressionNode *n) {
  const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
  ExpressionNode            *argument         = n->getArgument();

  ExpressionNodeArray newCoefficientArray(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    newCoefficientArray.add(toSForm(coefficientArray[i]));
  }
  return getPoly(n, newCoefficientArray, toSForm(argument));
}

SNode Expression::toSFormTreeNode(ExpressionNode *n) {
  const ExpressionNodeArray &a = n->getChildArray();
  ExpressionNodeArray        newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(toSForm(a[i]));
  }
  return getTree(n, newChildArray);
}

// -------------------------------------------------------------------------------------------------------

class StandardFormChecker : public ExpressionNodeHandler {
private:
  BitSet m_illegalSymbolSet;
  String m_error;
  bool   m_ok;
public:
  StandardFormChecker();
  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

StandardFormChecker::StandardFormChecker() : m_illegalSymbolSet(ExpressionTables->getTerminalCount() + 1) {
  static const ExpressionInputSymbol table[] = {
    PRODUCT
   ,SUM
  };
  for(int i = 0; i < ARRAYSIZE(table); i++) {
    m_illegalSymbolSet.add(table[i]);
  }
  m_ok = true;
}

bool StandardFormChecker::handleNode(ExpressionNode *n, int level) {
  if(m_illegalSymbolSet.contains(n->getSymbol())) {
    m_error = format(_T("Illegal symbol in standard form:<%s>. node=<%s>"), n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void Expression::checkIsStandardForm() {
  StandardFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

#endif
