#include "pch.h"
#include <Math/Expression/Expression.h>

#define ddx(n) D(n,name)

Expression Expression::getDerived(const String &name, bool reduceResult /*=false*/) const {
  if(getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Cannot get derived of an expression returning boolean"));
  }
  if(getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Cannot get derived of expression when treeform is %s. call toStandardForm()"), getTreeFormName().cstr());
  }

  Expression result = *this;
  result.setRoot(result.ddx(result.getRoot()).node());
  result.pruneUnusedNodes();
  result.buildSymbolTable();
  result.setState(EXPR_DERIVED);

  if(reduceResult) {
    result.reduce();
  }
  return result;
}

SNode Expression::D(SNode n, const String &name) {
  switch(n.getSymbol()) {
  case NUMBER    :
    return _0();

  case NAME      :
    { const ExpressionVariable &v = n.variable();
      if(n.name() == name) {
        return _1();
      } else if(v.isConstant() || v.isLoopVar() || !v.isDefined()) {
        return _0();
      } else {
        return fetchVariableNode(n.name()+_T("'"));
      }
    }

  case PLUS      :
    return ddx(n.left()) + ddx(n.right());

  case MINUS     :
    if(n.isUnaryMinus()) {
      return -ddx(n.left());
    } else {
      return ddx(n.left()) - ddx(n.right());
    }

  case PROD      :
    return ddx(n.left()) * n.right() + n.left() * ddx(n.right());

  case QUOT      :
    return (ddx(n.left()) * n.right() - n.left() * ddx(n.right())) / sqr(n.right());

  case POW       :
    { const SNode l = n.left();
      const SNode r = n.right();

      if(r.isNumber()) { // d/dx(l^c) = c * l' * l^(c-1)
        if(r.isOne()) {
          return ddx(l);
        } else {
          return r * ddx(l) * pow(l, r - _1());
        }
      } else {            // d/dx (l^r) -> (r/l*l' + r'*ln(l)) *(l^r)
        return (r/l * ddx(l) + ddx(r) * ln(l)) * n;
      }
    }
  case SQR       :        // sqr(u(x)) = powerS(u(x),2)
    return ddx(pow(n.left(), _2()));

  case SQRT      :        // sqrt(u(x)) = rootS(u(x),2)
    return ddx(::root(n.left(), _2()));

  case ROOT      :
    { const SNode l = n.left(), r = n.right();
      if(r.isNumber()) {
        if(r.isOne()) {  // take care of root(u(x),1) or we'll get division by zero
          return ddx(l);
        } else {         // d/dx(rootS(l,c)) = l'*rootS(l,1/(1/c-1))/c
          return (ddx(l) * root(l, reciprocal(reciprocal(r) - _1()))) / r;
        }
      } else {           // d(dx(rootS(l,r))) = rootS(l,r)*(l'*r/l - ln(l)*r')/(r^2)
        return n * (ddx(l) * r / l - ln(l) * ddx(r)) / sqr(r);
      }
    }
  case EXP       :
    return ddx(n.left()) * n;
  case EXP10     :
    return ddx(n.left()) * n * ln(_10());
  case EXP2      :
    return ddx(n.left()) * n * ln(_2());

  case LN        :
    return ddx(n.left()) / n.left();
  case LOG10     :
    return ddx(n.left()) / (ln(_10()) * n.left());
  case LOG2      :
    return ddx(n.left()) / (ln(_2()) * n.left());

  case SIN       :
    return  ddx(n.left()) * cos(n.left());

  case COS       :
    return -ddx(n.left()) * sin(n.left());

  case TAN       :  // d/dx(tan(u(x))) = u'(1+tan^2(u(x)))
    return  ddx(n.left()) * (_1() + sqr(n));

  case COT       :  // d/dx(cot(u(x))) = -u'(1+cot^2(u(x)))
    return -ddx(n.left()) * (_1() + sqr(n));

  case CSC       : // d/dx(csc(u(x))) = -u' * csc(u(x)) * cot(u(x))
    return -ddx(n.left()) * n * cot(n.left());

  case SEC       : // d/dxsec(u(x)) = u' * sec(u(x)) * tan(u(x))
    return  ddx(n.left()) * n * tan(n.left());

  case ACOS      :  // d/dx(acos(u(x))) = -u'/sqrt(1-u(x)^2)
    return -ddx(n.left()) / sqrt(_1() - sqr(n.left()));

  case ACSC      :  // d/dx(acsc(u(x))) = u'/(u(x)^2*sqrt(1-u(x)^2))
    return  ddx(n.left()) / ( sqr(n.left()) * sqrt(_1() - sqr(n.left())) );

  case ACOT      :  // d/dx(acot(u(x))) = -u'/(1+u(x)^2)
    return -ddx(n.left()) / (_1() + sqr(n.left()));

  case ASIN      :  // d/dx(asin(u(x))) = u'/sqrt(1-u(x)^2)
    return  ddx(n.left()) / sqrt(_1() - sqr(n.left()));

  case ATAN      :  // d/dx(atan(u(x))) = u'/(1+u(x)^2)
    return  ddx(n.left()) / (_1() + sqr(n.left()));

  case ATAN2     :  // d/dx(atan2(u1(x),u2(x))) = d/dx(atan(u1(x)/u2(x))) = (u1'*u2-u1*u2')/(u1^2+u2^2)
    { const SNode u1 = n.left(), u2 = n.right();
      return (ddx(u1)*u2 - u1*ddx(u2)) / (sqr(u1) + sqr(u2));
    }

  case ASEC      :  // d/dx(asec(u(x))) = -u'/(u(x)^2*sqrt(1-u(x)^2))
    return -ddx(n.left()) / (sqr(n.left()) * sqrt(_1() - sqr(n.left())));

  case SINH      : // d/dx(sinh(u(x))) = u'*cosh(u(x))
    return  ddx(n.left()) * cosh(n.left());

  case COSH      : // d/dx(cosh(u(x))) = u'*sinh(u(x))
    return  ddx(n.left()) * sinh(n.left());

  case TANH      : // d/dx(tanh(u(x))) = u'*(1-tanh^2(u(x)))
    return  ddx(n.left()) * (_1() - sqr(n));

  case ASINH     : // d/dx(asinh(u(x))) = u'/sqrt(u(x)^2+1)
    return  ddx(n.left()) / sqrt(sqr(n.left()) + _1());

  case ACOSH     : // d/dx(acosh(u(x))) = u'/sqrt(u(x)^2-1)
    return  ddx(n.left()) / sqrt(sqr(n.left()) - _1());

  case ATANH     : // d/dx(atanh(u(x))) = u'/(1-u(x)^2)
    return  ddx(n.left()) / (_1() - sqr(n.left()));

  case NORM      : // d/dx(norm(u(x))) = u'*gauss(u(x))
    return  ddx(n.left()) * gauss(n.left());

  case PROBIT    : // d/dx(probit(u(x))) = u'/gauss(probit(u(x)))
    return  ddx(n.left()) / gauss(n);

  case GAUSS     : // d/dx(gauss(u(x))) = -u'*u(x)*gauss(u(x))
    return -ddx(n.left()) * n.left() * n;

  case ERF       : // d/dx(erf(u(x))) = 2^(3/2)*u'*gauss(sqrt(2)*u(x))
    { const SNode sqrt2(sqrt(_2()));
      return _2() * sqrt2 * ddx(n.left()) * gauss(sqrt2 * n.left());
    }

  case INVERF    : // d/dx(inverf(u(x))) = u'/(erf'(inverf(u(x)))) = u'/ ((2^(3/2)*gauss(sqrt(2)*inverf(u(x)))))
    { const SNode sqrt2(sqrt(_2()));
      return ddx(n.left()) / (_2() * sqrt2 * gauss(sqrt2 * n));
    }
  case POLY      :
   return DPolynomial(n,name);

  case MAX       :
    return condExp(binExp(GE, n.left(), n.right()),ddx(n.left()), ddx(n.right()));

  case MIN       :
    return condExp(binExp(LE, n.left(), n.right()),ddx(n.left()), ddx(n.right()));

  case IIF        :
    return condExp(n.child(0), ddx(n.child(1)), ddx(n.child(2)));

  case SEMI      :
    return binExp(SEMI, DStatementList(n.child(0),name), ddx(n.child(1)));

  case RETURNREAL:
    return unaryExpression(RETURNREAL, ddx(n.left()));

  case RETURNBOOL:
    throwException(_T("Cannot get derived of boolean expression"));

  case INDEXEDSUM:
    return indexSum(n.child(0), n.child(1), ddx(n.child(2)));

  case INDEXEDPRODUCT:
    { const SNode startAssignment = n.child(0);
      const SNode endExpr         = n.child(1);
      const SNode expr            = n.child(2); // the expression to multiply
      const SNode productCounter  = startAssignment.left();
      const SNode sumCounter(allocateLoopVarNode(productCounter.name()));

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
    { const ExpressionNode *node = n.node();
      if(!node->dependsOn(name)) {
        return _0();
      }

      throwException(_T("Cannot find derived of \"%s\""), n.toString().cstr());
      return SNode(NULL);
    }
  }
}


/**
 * n.getSymbol = POLY
 * name find derived with respect to name
 * return expression containing derived of poly n
 * throws ExpressionUnderivableException if any of the expressions involved in evaluating the polynomial cannot be derived
 */
SNode Expression::DPolynomial(SNode n, const String &name) {
  const SExprList coefficients(n.getCoefficientArray());
  SNode           u            = n.getArgument();   // u(x) is the parameter to the polynomial
  const SNode     dudx         = ddx(u);            // dudx is u derived w.r.t. name

  SExprList newCoefficients;
  const int degree = n.getDegree();
  newCoefficients.add(ddx(coefficients[0]));
  for(int i = 1; i < (int)coefficients.size(); i++) {
    newCoefficients.add(SNode(this, degree-i+1) * coefficients[i-1] * dudx + ddx(coefficients[i]));
  }
  if(newCoefficients.size() == 0) {
    return _0();
  }
  return fetchPolyNode(newCoefficients, u);
}

SNode Expression::DStatementList(SNode n, const String &name) {
  DEFINEMETHODNAME;

  SStmtList stmtList(n);
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
        d.add(assignStmt(fetchVariableNode(var.name()+_T("'")), ddx(expr)));
      }
      break;
    default:
      throwUnknownSymbolException(method, stmt);
    }
  }
  return d;
}
