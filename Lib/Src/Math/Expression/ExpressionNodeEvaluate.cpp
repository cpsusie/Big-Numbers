#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

Real ExpressionNode::evaluateStatementListReal() const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList((ExpressionNode*)this);

  const size_t stmtCount = stmtList.size() - 1;
  for(size_t i = 0; i < stmtCount; i++) {
    stmtList[i]->doAssignment();
  }
  const ExpressionNode *last = stmtList.last();
  switch(last->getSymbol()) {
  case RETURNREAL: return last->left()->evaluateReal();
  default        : last->throwUnknownSymbolException(method);
  }
  return 0;
}

bool ExpressionNode::evaluateStatementListBool() const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList((ExpressionNode *)this);

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    stmtList[i]->doAssignment();
  }
  const ExpressionNode *last = stmtList.last();
  switch(last->getSymbol()) {
  case RETURNBOOL: return last->left()->evaluateBool();
  default        : last->throwUnknownSymbolException(method);
  }
  return false;
}

Real ExpressionNode::evaluateReal() const {
  DEFINEMETHODNAME;

  switch(getSymbol()) {
  case NAME    : return getValueRef();
  case NUMBER  : return getReal(); // dont use getValue() as it takes the value in m_valueTable
                                   // We might be called from buildSymbolTable
                                   // before this table is filled with constants
  case PLUS     : return left()->evaluateReal() + right()->evaluateReal();
  case MINUS    : return isUnaryMinus() ? -left()->evaluateReal() : (left()->evaluateReal() - right()->evaluateReal());
  case PROD     : return left()->evaluateReal() * right()->evaluateReal();
  case QUOT     : return left()->evaluateReal() / right()->evaluateReal();
  case MOD      : return fmod(left()->evaluateReal(), right()->evaluateReal());
  case POW      : return evaluatePow();
  case ROOT     : return evaluateRoot();
  case BINOMIAL : return binomial(               left()->evaluateReal(),  right()->evaluateReal());
  case CHI2DIST : return chiSquaredDistribution( left()->evaluateReal(),  right()->evaluateReal());
  case CHI2DENS : return chiSquaredDensity(      left()->evaluateReal(),  right()->evaluateReal());
  case LINCGAMMA: return lowerIncGamma(          left()->evaluateReal(),  right()->evaluateReal());
  case ATAN2    : return atan2(                  left()->evaluateReal(),  right()->evaluateReal(), getTrigonometricMode());
  case HYPOT    : return hypot(                  left()->evaluateReal(),  right()->evaluateReal());
  case MAX      : return dmax(                   left()->evaluateReal(),  right()->evaluateReal());
  case MIN      : return dmin(                   left()->evaluateReal(),  right()->evaluateReal());
  case RAND     : return randReal(               left()->evaluateReal(),  right()->evaluateReal());
  case NORMRAND : return randomGaussian(         left()->evaluateReal(),  right()->evaluateReal());
  case ABS      : return fabs(                   left()->evaluateReal());
  case ACOS     : return acos(                   left()->evaluateReal(),  getTrigonometricMode());
  case ACOSH    : return acosh(                  left()->evaluateReal());
  case ACOT     : return acot(                   left()->evaluateReal(),  getTrigonometricMode());
  case ACSC     : return acsc(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASEC     : return asec(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASIN     : return asin(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASINH    : return asinh(                  left()->evaluateReal());
  case ATAN     : return atan(                   left()->evaluateReal(),  getTrigonometricMode());
  case ATANH    : return atanh(                  left()->evaluateReal());
  case CEIL     : return ceil(                   left()->evaluateReal());
  case COS      : return cos(                    left()->evaluateReal(),  getTrigonometricMode());
  case COSH     : return cosh(                   left()->evaluateReal());
  case COT      : return cot(                    left()->evaluateReal(),  getTrigonometricMode());
  case CSC      : return csc(                    left()->evaluateReal(),  getTrigonometricMode());
  case ERF      : return errorFunction(          left()->evaluateReal());
  case EXP      : return exp(                    left()->evaluateReal());
  case EXP10    : return exp10(                  left()->evaluateReal());
  case EXP2     : return exp2(                   left()->evaluateReal());
  case FAC      : return fac(                    left()->evaluateReal());
  case FLOOR    : return floor(                  left()->evaluateReal());
  case GAMMA    : return gamma(                  left()->evaluateReal());
  case GAUSS    : return gauss(                  left()->evaluateReal());
  case INVERF   : return inverseErrorFunction(   left()->evaluateReal());
  case LN       : return log(                    left()->evaluateReal());
  case LOG10    : return log10(                  left()->evaluateReal());
  case LOG2     : return log2(                   left()->evaluateReal());
  case NORM     : return norm(                   left()->evaluateReal());
  case PROBIT   : return probitFunction(         left()->evaluateReal());
  case SEC      : return sec(                    left()->evaluateReal(),  getTrigonometricMode());
  case SIGN     : return sign(                   left()->evaluateReal());
  case SIN      : return sin(                    left()->evaluateReal(),  getTrigonometricMode());
  case SINH     : return sinh(                   left()->evaluateReal());
  case SQR      : return sqr(                    left()->evaluateReal());
  case SQRT     : return sqrt(                   left()->evaluateReal());
  case TAN      : return tan(                    left()->evaluateReal(),  getTrigonometricMode());
  case TANH     : return tanh(                   left()->evaluateReal());
  case IIF      : return child(0)->evaluateBool() ? child(1)->evaluateReal() : child(2)->evaluateReal();

  case INDEXEDSUM     :
    {            const ExpressionNode *startAssignment = child(0);
                 const ExpressionNode *beginExpr       = startAssignment->right();
                 const ExpressionNode *expr            = child(2);
                 Real                 &i               = startAssignment->left()->getValueRef();
                 const Real            endIndex        = child(1)->evaluateReal();
                 Real                  sum             = 0;
                 for(i = beginExpr->evaluateReal(); i <= endIndex; i++) {
                   sum += expr->evaluateReal();
                 }
                 return sum;
    }
  case INDEXEDPRODUCT :
    {            const ExpressionNode *startAssignment = child(0);
                 const ExpressionNode *beginExpr       = startAssignment->right();
                 const ExpressionNode *expr            = child(2);
                 Real                 &i               = startAssignment->left()->getValueRef();
                 const Real            endIndex        = child(1)->evaluateReal();
                 Real                  product         = 1;

                 for(i = beginExpr->evaluateReal(); i <= endIndex; i++) {
                   product *= expr->evaluateReal();
                 }
                 return product;
    }
  default      : throwUnknownSymbolException(method);
                 return 0;
  }
}

Real ExpressionNode::evaluatePow() const {
  Real base = left()->evaluateReal();
  const Real expo = right()->evaluateReal();
  if(base >= 0) {
    return mypow(base, expo);
  } else {
    Rational expoR;
    if(Rational::isRational(expo, &expoR)) {
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

Real ExpressionNode::evaluateRoot() const {
  Real       rad  = left()->evaluateReal();
  const Real rt   = right()->evaluateReal();
  if(rad >= 0) {
    return root(rad, rt);
  } else {
    Rational rootR;
    if(Rational::isRational(rt, &rootR)) {
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

void ExpressionNode::doAssignment() const {
  switch(getSymbol()) {
  case ASSIGN:
//    printf(_T("doasign:<%s> = %le\n"),n->left()->getName().cstr(),evaluateReal(n->right()));
    { ExpressionVariable &var = left()->getVariable();
      if(!var.isConstant()) {
        m_tree.getValueRef(var) = right()->evaluateReal();
      }
    }
    break;
  default:
    throwUnknownSymbolException(__TFUNCTION__);
  }
}

bool ExpressionNode::evaluateBool() const {
  DEFINEMETHODNAME;
  switch(getSymbol()) {
  case TYPEBOOL: return getBool();
  case SYMNOT  : return !child(0)->evaluateBool();
  case SYMAND  : return left()->evaluateBool() && right()->evaluateBool();
  case SYMOR   : return left()->evaluateBool() || right()->evaluateBool();
  case EQ      : return left()->evaluateReal() == right()->evaluateReal();
  case NE      : return left()->evaluateReal() != right()->evaluateReal();
  case LE      : return left()->evaluateReal() <= right()->evaluateReal();
  case LT      : return left()->evaluateReal() <  right()->evaluateReal();
  case GE      : return left()->evaluateReal() >= right()->evaluateReal();
  case GT      : return left()->evaluateReal() >  right()->evaluateReal();
  default      : throwUnknownSymbolException(method);
  }
  return true;
}
