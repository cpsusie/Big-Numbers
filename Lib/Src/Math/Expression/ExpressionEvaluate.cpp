#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

Real Expression::evaluateStatementListReal(const ExpressionNode *n) const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList((ExpressionNode*)n);

  const size_t stmtCount = stmtList.size() - 1;
  for(size_t i = 0; i < stmtCount; i++) {
    doAssignment(stmtList[i]);
  }
  const ExpressionNode *last = stmtList.last();
  switch(last->getSymbol()) {
  case RETURNREAL: return evaluateRealExpr(last->left());
  default        : throwUnknownSymbolException(method, last);
  }
  return 0;
}

bool Expression::evaluateStatementListBool(const ExpressionNode *n) const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList((ExpressionNode *)n);

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    doAssignment(stmtList[i]);
  }
  const ExpressionNode *last = stmtList.last();
  switch(last->getSymbol()) {
  case RETURNBOOL: return evaluateBoolExpr(last->left());
  default        : throwUnknownSymbolException(method, last);
  }
  return false;
}

Real Expression::evaluateRealExpr(const ExpressionNode *n) const {
  DEFINEMETHODNAME;

  switch(n->getSymbol()) {
  case NAME    : return n->getValueRef();
  case NUMBER  : return n->getReal(); // dont use getValue() as it takes the value in m_valueTable
                                      // We might be called from buildSymbolTable
                                      // before this table is filled with constants
  case PLUS     : return evaluateRealExpr(n->left()) + evaluateRealExpr(n->right());
  case MINUS    : return n->isUnaryMinus() ? -evaluateRealExpr(n->left()) : (evaluateRealExpr(n->left()) - evaluateRealExpr(n->right()));
  case PROD     : return evaluateRealExpr(n->left()) * evaluateRealExpr(n->right());
  case QUOT     : return evaluateRealExpr(n->left()) / evaluateRealExpr(n->right());
  case MOD      : return fmod(                   evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case POW      : return evaluatePow(n);
  case ROOT     : return evaluateRoot(n);
  case BINOMIAL : return binomial(               evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case CHI2DIST : return chiSquaredDistribution( evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case CHI2DENS : return chiSquaredDensity(      evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case LINCGAMMA: return lowerIncGamma(          evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case ATAN2    : return atan2(                  evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()), getTrigonometricMode());
  case MAX      : return dmax(                   evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case MIN      : return dmin(                   evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case RAND     : return random(                 evaluateRealExpr(n->left()),  evaluateRealExpr(n->right()));
  case NORMRAND : return randomGaussian(         evaluateRealExpr(n->left()), evaluateRealExpr(n->right()));
  case ABS      : return fabs(                   evaluateRealExpr(n->left()));
  case ACOS     : return acos(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ACOSH    : return acosh(                  evaluateRealExpr(n->left()));
  case ACOT     : return acot(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ACSC     : return acsc(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ASEC     : return asec(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ASIN     : return asin(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ASINH    : return asinh(                  evaluateRealExpr(n->left()));
  case ATAN     : return atan(                   evaluateRealExpr(n->left()), getTrigonometricMode());
  case ATANH    : return atanh(                  evaluateRealExpr(n->left()));
  case CEIL     : return ceil(                   evaluateRealExpr(n->left()));
  case COS      : return cos(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case COSH     : return cosh(                   evaluateRealExpr(n->left()));
  case COT      : return cot(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case CSC      : return csc(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case ERF      : return errorFunction(          evaluateRealExpr(n->left()));
  case EXP      : return exp(                    evaluateRealExpr(n->left()));
  case FAC      : return fac(                    evaluateRealExpr(n->left()));
  case FLOOR    : return floor(                  evaluateRealExpr(n->left()));
  case GAMMA    : return gamma(                  evaluateRealExpr(n->left()));
  case GAUSS    : return gauss(                  evaluateRealExpr(n->left()));
  case INVERF   : return inverseErrorFunction(   evaluateRealExpr(n->left()));
  case LN       : return log(                    evaluateRealExpr(n->left()));
  case LOG10    : return log10(                  evaluateRealExpr(n->left()));
  case NORM     : return norm(                   evaluateRealExpr(n->left()));
  case POLY     : return evaluatePolynomial(n);
  case PRODUCT  : return evaluateProduct(n);
  case SUM      : return evaluateSum(n);
  case PROBIT   : return probitFunction(         evaluateRealExpr(n->left()));
  case SEC      : return sec(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case SIGN     : return sign(                   evaluateRealExpr(n->left()));
  case SIN      : return sin(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case SINH     : return sinh(                   evaluateRealExpr(n->left()));
  case SQR      : return sqr(                    evaluateRealExpr(n->left()));
  case SQRT     : return sqrt(                   evaluateRealExpr(n->left()));
  case TAN      : return tan(                    evaluateRealExpr(n->left()), getTrigonometricMode());
  case TANH     : return tanh(                   evaluateRealExpr(n->left()));
  case IIF      : return evaluateBoolExpr(n->child(0)) ? evaluateRealExpr(n->child(1)) : evaluateRealExpr(n->child(2));

  case INDEXEDSUM     :
    {            const ExpressionNode *startAssignment = n->child(0);
                 const ExpressionNode *beginExpr       = startAssignment->right();
                 const ExpressionNode *expr            = n->child(2);
                 Real                 &i               = getValueRef(startAssignment->left()->getVariable());
                 const Real            endIndex        = evaluateRealExpr(n->child(1));
                 Real                  sum             = 0;
                 for(i = evaluateRealExpr(beginExpr); i <= endIndex; i++) {
                   sum += evaluateRealExpr(expr);
                 }
                 return sum;
    }
  case INDEXEDPRODUCT :
    {            const ExpressionNode *startAssignment = n->child(0);
                 const ExpressionNode *beginExpr       = startAssignment->right();
                 const ExpressionNode *expr            = n->child(2);
                 const Real            endIndex        = evaluateRealExpr(n->child(1));
                 Real                 &i               = getValueRef(startAssignment->left()->getVariable());
                 Real                  product         = 1;

                 for(i = evaluateRealExpr(beginExpr); i <= endIndex; i++) {
                   product *= evaluateRealExpr(expr);
                 }
                 return product;
    }
  default      : throwUnknownSymbolException(method, n);
                 return 0;
  }
}

Real Expression::evaluateProduct(const ExpressionNode *n) const {
  Real prod = 1;
  const FactorArray &factors = n->getFactorArray();
  for(size_t i = 0; i < factors.size(); i++) {
    prod *= evaluateRealExpr(factors[i]);
  }
  return prod;
}

Real Expression::evaluateSum(const ExpressionNode *n) const {
  Real sum = 0;
  const AddentArray &a = n->getAddentArray();
  for(size_t i = 0; i < a.size(); i++) {
    const SumElement *e = a[i];
    const Real v = evaluateRealExpr(e->getNode());
    if(e->isPositive()) {
      sum += v;
    } else {
      sum -= v;
    }
  }
  return sum;
}

Real Expression::evaluatePow(const ExpressionNode *n) const {
  Real base = evaluateRealExpr(n->left());
  const Real expo = evaluateRealExpr(n->right());
  if(base >= 0) {
    return mypow(base, expo);
  } else {
    Rational expoR;
    if(Rational::isRealRational(expo, &expoR)) {
      if(expoR.getNumerator() != 1) {
        base = mypow(base, (Real)expoR.getNumerator());
      }
      if(expoR.getDenominator() != 1) {
        base = root(base, (Real)expoR.getDenominator());
      }
      return base;
    } else {
      return pow(base, expo); // -1.#IND
    }
  }
}

Real Expression::evaluateRoot(const ExpressionNode *n) const {
  Real       rad  = evaluateRealExpr(n->left());
  const Real rt   = evaluateRealExpr(n->right());
  if(rad >= 0) {
    return root(rad, rt);
  } else {
    Rational rootR;
    if(Rational::isRealRational(rt, &rootR)) {
      if(rootR.getNumerator() != 1) {
        rad = root(rad, (Real)rootR.getNumerator());
      }
      if(rootR.getDenominator() != 1) {
        rad = mypow(rad, (Real)rootR.getDenominator());
      }
      return rad;
    } else {
      return root(rad, rt); // -1.#IND
    }
  }
}

/**
 * n.symbol = POLY, n.getCoefficientArray = coefficients. coef[0..n] poly(x) = ((coef[0] * x) + coef[1]) * x)... ) + coef[n]
 * return value of polynomial p(x) with coefficients contained in n.getCoefficientArray, and x in n.getArgument()
 */
Real Expression::evaluatePolynomial(const ExpressionNode *n) const {
  const Real         x = evaluateRealExpr(n->getArgument());
  CompactArray<Real> coef;
  getCoefficients(coef, n->getCoefficientArray());
  Real result = 0;
  for(size_t i = 0; i < coef.size(); i++) {
    result = result * x + coef[i];
  }
  return result;
}

void Expression::getCoefficients(CompactArray<Real> &dst, const ExpressionNodeArray &coefficientArray) const {
  dst.setCapacity(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    dst.add(evaluateRealExpr(coefficientArray[i]));
  }
}

void Expression::doAssignment(const ExpressionNode *n) const {
  DEFINEMETHODNAME;
  switch(n->getSymbol()) {
  case ASSIGN:
//    printf(_T("doasign:<%s> = %le\n"),n->left()->getName().cstr(),evaluateRealExpr(n->right()));
    { ExpressionVariable &var = n->left()->getVariable();
      if(!var.isConstant()) {
        getValueRef(var) = evaluateRealExpr(n->right());
      }
    }
    break;
  default:
    throwUnknownSymbolException(method, n);
  }
}

bool Expression::evaluateBoolExpr(const ExpressionNode *n) const {
  DEFINEMETHODNAME;
  switch(n->getSymbol()) {
  case TYPEBOOL: return n->getBool();
  case NOT     : return !evaluateBoolExpr(n->child(0));
  case AND     : return evaluateBoolExpr(n->left()) && evaluateBoolExpr(n->right());
  case OR      : return evaluateBoolExpr(n->left()) || evaluateBoolExpr(n->right());
  case EQ      : return evaluateRealExpr(n->left()) == evaluateRealExpr(n->right());
  case NE      : return evaluateRealExpr(n->left()) != evaluateRealExpr(n->right());
  case LE      : return evaluateRealExpr(n->left()) <= evaluateRealExpr(n->right());
  case LT      : return evaluateRealExpr(n->left()) <  evaluateRealExpr(n->right());
  case GE      : return evaluateRealExpr(n->left()) >= evaluateRealExpr(n->right());
  case GT      : return evaluateRealExpr(n->left()) >  evaluateRealExpr(n->right());
  default      : throwUnknownSymbolException(method, n);

  }
  return true;
}
