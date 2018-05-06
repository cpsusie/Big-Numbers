#include "pch.h"
#include <Math/Expression/ParserTree.h>

#define ddx(n) n.D(name)

SNode SNode::D(const String &name) const {
  switch(getSymbol()) {
  case NUMBER    :
    return _0();

  case NAME      :
    { const ExpressionVariable &v = variable();
      if(v.getName() == name) {
        return _1();
      } else if(v.isConstant() || v.isLoopVar() || !v.isDefined()) {
        return _0();
      } else {
        return getTree()->fetchVariableNode(v.getName()+_T("'"));
      }
    }

  case PLUS      :
    return ddx(left()) + ddx(right());

  case MINUS     :
    if(isUnaryMinus()) {
      return -ddx(left());
    } else {
      return ddx(left()) - ddx(right());
    }

  case PROD      :
    return ddx(left()) * right() + left() * ddx(right());

  case QUOT      :
    return (ddx(left()) * right() - left() * ddx(right())) / sqr(right());

  case POW       :
    { const SNode l = left();
      const SNode r = right();

      if(r.isNumber()) { // d/dx(l^c) = c * l' * l^(c-1)
        if(r.isOne()) {
          return ddx(l);
        } else {
          return r * ddx(l) * pow(l, r - _1());
        }
      } else {            // d/dx (l^r) -> (r/l*l' + r'*ln(l)) *(l^r)
        return (r/l * ddx(l) + ddx(r) * ln(l)) * *this;
      }
    }
  case SQR       :        // sqr(u(x)) = powerS(u(x),2)
    return ddx(pow(left(), _2()));

  case SQRT      :        // sqrt(u(x)) = rootS(u(x),2)
    return ddx(::root(left(), _2()));

  case ROOT      :
    { const SNode l = left(), r = right();
      if(r.isNumber()) {
        if(r.isOne()) {  // take care of root(u(x),1) or we'll get division by zero
          return ddx(l);
        } else {         // d/dx(rootS(l,c)) = l'*rootS(l,1/(1/c-1))/c
          return (ddx(l) * root(l, reciprocal(reciprocal(r) - _1()))) / r;
        }
      } else {           // d(dx(rootS(l,r))) = rootS(l,r)*(l'*r/l - ln(l)*r')/(r^2)
        return *this * (ddx(l) * r / l - ln(l) * ddx(r)) / sqr(r);
      }
    }
  case EXP       :
    return ddx(left()) * *this;
  case EXP10     :
    return ddx(left()) * *this * ln(_10());
  case EXP2      :
    return ddx(left()) * *this * ln(_2());

  case LN        :
    return ddx(left()) / left();
  case LOG10     :
    return ddx(left()) / (ln(_10()) * left());
  case LOG2      :
    return ddx(left()) / (ln(_2()) * left());

  case SIN       :
    return  ddx(left()) * cos(left());

  case COS       :
    return -ddx(left()) * sin(left());

  case TAN       :  // d/dx(tan(u(x))) = u'(1+tan^2(u(x)))
    return  ddx(left()) * (_1() + sqr(*this));

  case COT       :  // d/dx(cot(u(x))) = -u'(1+cot^2(u(x)))
    return -ddx(left()) * (_1() + sqr(*this));

  case CSC       : // d/dx(csc(u(x))) = -u' * csc(u(x)) * cot(u(x))
    return -ddx(left()) * *this * cot(left());

  case SEC       : // d/dxsec(u(x)) = u' * sec(u(x)) * tan(u(x))
    return  ddx(left()) * *this * tan(left());

  case ACOS      :  // d/dx(acos(u(x))) = -u'/sqrt(1-u(x)^2)
    return -ddx(left()) / sqrt(_1() - sqr(left()));

  case ACSC      :  // d/dx(acsc(u(x))) = u'/(u(x)^2*sqrt(1-u(x)^2))
    return  ddx(left()) / ( sqr(left()) * sqrt(_1() - sqr(left())) );

  case ACOT      :  // d/dx(acot(u(x))) = -u'/(1+u(x)^2)
    return -ddx(left()) / (_1() + sqr(left()));

  case ASIN      :  // d/dx(asin(u(x))) = u'/sqrt(1-u(x)^2)
    return  ddx(left()) / sqrt(_1() - sqr(left()));

  case ATAN      :  // d/dx(atan(u(x))) = u'/(1+u(x)^2)
    return  ddx(left()) / (_1() + sqr(left()));

  case ATAN2     :  // d/dx(atan2(u1(x),u2(x))) = d/dx(atan(u1(x)/u2(x))) = (u1'*u2-u1*u2')/(u1^2+u2^2)
    { const SNode u1 = left(), u2 = right();
      return (ddx(u1)*u2 - u1*ddx(u2)) / (sqr(u1) + sqr(u2));
    }

  case ASEC      :  // d/dx(asec(u(x))) = -u'/(u(x)^2*sqrt(1-u(x)^2))
    return -ddx(left()) / (sqr(left()) * sqrt(_1() - sqr(left())));

  case SINH      : // d/dx(sinh(u(x))) = u'*cosh(u(x))
    return  ddx(left()) * cosh(left());

  case COSH      : // d/dx(cosh(u(x))) = u'*sinh(u(x))
    return  ddx(left()) * sinh(left());

  case TANH      : // d/dx(tanh(u(x))) = u'*(1-tanh^2(u(x)))
    return  ddx(left()) * (_1() - sqr(*this));

  case ASINH     : // d/dx(asinh(u(x))) = u'/sqrt(u(x)^2+1)
    return  ddx(left()) / sqrt(sqr(left()) + _1());

  case ACOSH     : // d/dx(acosh(u(x))) = u'/sqrt(u(x)^2-1)
    return  ddx(left()) / sqrt(sqr(left()) - _1());

  case ATANH     : // d/dx(atanh(u(x))) = u'/(1-u(x)^2)
    return  ddx(left()) / (_1() - sqr(left()));

  case NORM      : // d/dx(norm(u(x))) = u'*gauss(u(x))
    return  ddx(left()) * gauss(left());

  case PROBIT    : // d/dx(probit(u(x))) = u'/gauss(probit(u(x)))
    return  ddx(left()) / gauss(*this);

  case GAUSS     : // d/dx(gauss(u(x))) = -u'*u(x)*gauss(u(x))
    return -ddx(left()) * left() * *this;

  case ERF       : // d/dx(erf(u(x))) = 2^(3/2)*u'*gauss(sqrt(2)*u(x))
    { const SNode sqrt2(sqrt(_2()));
      return _2() * sqrt2 * ddx(left()) * gauss(sqrt2 * left());
    }

  case INVERF    : // d/dx(inverf(u(x))) = u'/(erf'(inverf(u(x)))) = u'/ ((2^(3/2)*gauss(sqrt(2)*inverf(u(x)))))
    { const SNode sqrt2(sqrt(_2()));
      return ddx(left()) / (_2() * sqrt2 * gauss(sqrt2 * *this));
    }
  case POLY      :
   return DPoly(name);

  case MAX       :
    return condExp(binExp(GE, left(), right()),ddx(left()), ddx(right()));

  case MIN       :
    return condExp(binExp(LE, left(), right()),ddx(left()), ddx(right()));

  case IIF        :
    return condExp(child(0), ddx(child(1)), ddx(child(2)));

  case SEMI      :
    return binExp(SEMI, child(0).DStmtList(name), ddx(child(1)));

  case RETURNREAL:
    return getTree()->unaryExpression(RETURNREAL, ddx(left()));

  case RETURNBOOL:
    throwException(_T("Cannot get derived of boolean expression"));

  case INDEXEDSUM:
    return indexSum(child(0), child(1), ddx(child(2)));

  case INDEXEDPRODUCT:
    { const SNode startAssignment = child(0);
      const SNode endExpr         = child(1);
      const SNode expr            = child(2); // the expression to multiply
      const SNode productCounter  = startAssignment.left();
      const SNode sumCounter(getTree()->allocateLoopVarNode(productCounter.name()));

      return indexSum(assignStmt(sumCounter, startAssignment.right())
                     ,endExpr
                     ,indexProd(startAssignment
                               ,endExpr
                               ,condExp(binExp(EQ, productCounter, sumCounter), ddx(expr), expr)
                               )
                     );
    }

  default        :
  case SUM       :
  case PRODUCT   :
    { if(!m_node->dependsOn(name)) {
        return _0();
      }
      throwException(_T("Cannot find derived of \"%s\""), toString().cstr());
      return SNode(NULL);
    }
  }
}


/**
 * getSymbol() = POLY
 * name find derived with respect to name
 * return SNode containing derived of poly
 * throws ExpressionUnderivableException if any of the SNode involved in evaluating the polynomial cannot be derived
 */
SNode SNode::DPoly(const String &name) const {
  const SExprList coefficients(getCoefficientArray());
  SNode           u            = getArgument();   // u(x) is the parameter to the polynomial
  const SNode     dudx         = ddx(u);          // dudx is u derived wrt. name

  SExprList newCoefficients;
  const int degree = getDegree();
  newCoefficients.add(ddx(coefficients[0]));
  for(int i = 1; i < (int)coefficients.size(); i++) {
    newCoefficients.add(SNode(getTree(), degree-i+1) * coefficients[i-1] * dudx + ddx(coefficients[i]));
  }
  if(newCoefficients.size() == 0) {
    return _0();
  }
  return getTree()->fetchPolyNode(newCoefficients, u);
}

SNode SNode::DStmtList(const String &name) const {
  DEFINEMETHODNAME;

  SStmtList stmtList(*this);
  SStmtList d;
  for(size_t i = 0; i < stmtList.size(); i++) {
    const SNode &stmt = stmtList[i];
    switch(stmt.getSymbol()) {
    case ASSIGN:
      { const SNode var(stmt.left());   // variable assigned to
        const SNode expr(stmt.right()); // expression on the right side of =
        if(var.name() == name) {
          throwException(_T("Cannot find derived of statement \"%s\", because a value is assigned to %s"), stmt.toString().cstr(), name.cstr());
        }
        d.add(stmt);
        d.add(assignStmt(getTree()->fetchVariableNode(var.name()+_T("'")), ddx(expr)));
      }
      break;
    default:
      stmt.throwUnknownSymbolException(method);
    }
  }
  return d;
}
