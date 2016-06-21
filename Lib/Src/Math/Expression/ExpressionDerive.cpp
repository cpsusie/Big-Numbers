#include "pch.h"
#include <Math/Expression/Expression.h>

Expression Expression::getDerived(const String &name, bool reduceResult /*=false*/) const {
  if(getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Cannot get derived of an expression returning boolean"));
  }
  if(getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Cannot get derived of expression when treeform is %s. call toStandardForm()"), getTreeFormName().cstr());
  }

  Expression result = *this;
  result.setRoot(result.D(result.getRoot(), name).node());
  result.pruneUnusedNodes();
  result.buildSymbolTable();
  result.setState(EXPR_DERIVED);

  if(reduceResult) {
    result.reduce();
  }
  return result;
}

SNode Expression::D(const SNode &n, const String &name) const {
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
    return D(n.left(), name) + D(n.right(), name);

  case MINUS     :
    if(n.isUnaryMinus()) {
      return -D(n.left(), name);
    } else {
      return D(n.left(), name) - D(n.right(), name);
    }

  case PROD      :
    return D(n.left(), name) * n.right() + n.left() * D(n.right(), name);

  case QUOT      :
    return (D(n.left(), name) * n.right() - n.left() * D(n.right(), name)) / sqr(n.right());

  case POW       :
    { const SNode l = n.left();
      const SNode r = n.right();

      if(r.isNumber()) { // d/dx(l^c) = c * l' * l^(c-1)
        if(r.isOne()) {
          return D(l, name);
        } else {
          return r * D(l, name) * pow(l, r - _1());
        }
      } else {            // d/dx (l^r) -> (r/l*l' + r'*ln(l)) *(l^r)
        return (r/l * D(l,name) + D(r, name) * ln(l)) * n;
      }
    }
  case SQR       :        // sqr(u(x)) = powerS(u(x),2)
    return D(pow(n.left(), _2()), name);

  case SQRT      :        // sqrt(u(x)) = rootS(u(x),2)
    return D(::root(n.left(), _2()), name); 

  case ROOT      :
    { const SNode l = n.left(), r = n.right();
      if(r.isNumber()) { 
        if(r.isOne()) {  // take care of root(u(x),1) or we'ææ get division by zero
          return D(l, name);
        } else {         // d/dx(rootS(l,c)) = l'*rootS(l,1/(1/c-1))/c  
          return (D(l, name) * root(l, reciprocal(reciprocal(r) - _1()))) / r;
        }
      } else {           // d(dx(rootS(l,r))) = rootS(l,r)*(l'*r/l - ln(l)*r')/(r^2)
        return n * (D(l, name) * r / l - ln(l) * D(r, name)) / sqr(r);
      }
    }
  case LN        :
    return D(n.left(), name) / n.left();

  case LOG10     :
    { const SNode ln10(ln(SNode(this, 10)));
      return D(n.left(), name) / (ln10 * n.left());
    }

  case EXP       :
    return  D(n.left(), name) * n;

  case SIN       :
    return  D(n.left(), name) * cos(n.left());

  case COS       :
    return -D(n.left(), name) * sin(n.left());

  case TAN       :  // d/dx(tan(u(x))) = u'(1+tan^2(x))
    return  D(n.left(), name) * (_1() + sqr(n));

  case COT       :  // d/dx(cot(u(x))) = -u'(1+cot^2(x))
    return -D(n.left(), name) * (_1() + sqr(n));

  case CSC       : // d/dx(csc(u(x))) = -u' * csc(u(x)) * cot(u(x))
    return -D(n.left(), name) * n * cot(n.left());

  case SEC       : // d/dxsec(u(x)) = u' * sec(u(x)) * tan(u(x))
    return  D(n.left(), name) * n * tan(n.left());

  case ACOS      :  // d/dx(acos(u(x))) = -u'/sqrt(1-u(x)^2)
    return -D(n.left(), name) / sqrt(_1() - sqr(n.left()));

  case ACSC      :  // d/dx(acsc(u(x))) = u'/(u(x)^2*sqrt(1-u(x)^2))
    return  D(n.left(), name) / ( sqr(n.left()) * sqrt(_1() - sqr(n.left())) );

  case ACOT      :  // d/dx(acot(u(x))) = -u'/(1+u(x)^2)
    return -D(n.left(), name) / (_1() + sqr(n.left()));

  case ASIN      :  // d/dx(asin(u(x))) = u'/sqrt(1-u(x)^2)
    return  D(n.left(), name) / sqrt(_1() - sqr(n.left()));

  case ATAN      :  // d/dx(atan(u(x))) = u'/(1+u(x)^2)
    return  D(n.left(), name) / (_1() + sqr(n.left()));

  case ASEC      :  // d/dx(asec(u(x))) = -u'/(u(x)^2*sqrt(1-u(x)^2))
    return -D(n.left(), name) / (sqr(n.left()) * sqrt(_1() - sqr(n.left())));

  case SINH      : // d/dx(sinh(u(x))) = u'*cosh(u(x))
    return  D(n.left(), name) * cosh(n.left());

  case COSH      : // d/dx(cosh(u(x))) = u'*sinh(u(x))
    return  D(n.left(), name) * sinh(n.left());

  case TANH      : // d/dx(tanh(u(x))) = u'*(1-tanh^2(u(x)))
    return  D(n.left(), name) * (_1() - sqr(n));

  case ASINH     : // d/dx(asinh(u(x))) = u'/sqrt(u(x)^2+1)
    return  D(n.left(), name) / sqrt(sqr(n.left()) + _1());

  case ACOSH     : // d/dx(acosh(u(x))) = u'/sqrt(u(x)^2-1)
    return  D(n.left(), name) / sqrt(sqr(n.left()) - _1());

  case ATANH     : // d/dx(atanh(u(x))) = u'/(1-u(x)^2)
    return  D(n.left(), name) / (_1() - sqr(n.left()));

  case NORM      : // d/dx(norm(u(x))) = u'*gauss(u(x))
    return  D(n.left(), name) * gauss(n.left());

  case PROBIT    : // d/dx(probit(u(x))) = u'/gauss(probit(u(x)))
    return  D(n.left(), name) / gauss(n);

  case GAUSS     : // d/dx(gauss(u(x))) = -u'*u(x)*gauss(u(x))
    return -D(n.left(), name) * n.left() * n;

  case ERF       : // d/dx(erf(u(x))) = 2^(3/2)*u'*gauss(sqrt(2)*u(x))
    { const SNode sqrt2(sqrt(_2()));
      return _2() * sqrt2 * D(n.left(), name) * gauss(sqrt2 * n.left());
    }

  case INVERF    : // d/dx(inverf(u(x))) = u'/(erf'(inverf(u(x)))) = u'/ ((2^(3/2)*gauss(sqrt(2)*inverf(u(x)))))
    { const SNode sqrt2(sqrt(_2()));
      return D(n.left(), name) / (_2() * sqrt2 * gauss(sqrt2 * n));
    }
  case POLY      :
   return DPolynomial(n, name);

  case MAX       :
    return condExp(binExp(GE, n.left(), n.right()),D(n.left(), name), D(n.right(), name));

  case MIN       :
    return condExp(binExp(LE, n.left(), n.right()),D(n.left(), name), D(n.right(), name));

  case IIF        :
    return condExp(n.child(0), D(n.child(1), name), D(n.child(2), name));

  case SEMI      :
    return binExp(SEMI, DStatementList(n.child(0), name), D(n.child(1), name));

  case RETURNREAL:
    return unaryExpression(RETURNREAL, D(n.left(), name));

  case RETURNBOOL:
    throwException("Cannot get derived of boolean expression");

  case INDEXEDSUM:
    return indexSum(n.child(0), n.child(1), D(n.child(2), name));

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
                               ,condExp(binExp(EQ, productCounter, sumCounter), D(expr, name), expr)
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
SNode Expression::DPolynomial(const SNode &n, const String &name) const {
  const SExprList coefficients(n.getCoefficientArray());
  const SNode     u            = n.getArgument();   // u(x) is the parameter to the polynomial
  const SNode     dudx         = D(u, name);        // dudx is u derived w.r.t. name

  SExprList newCoefficients;
  const int degree = n.getDegree();
  newCoefficients.add(D(coefficients[0], name));
  for(int i = 1; i < (int)coefficients.size(); i++) {
    newCoefficients.add(SNode(this, degree-i+1) * coefficients[i-1] * dudx + D(coefficients[i], name));
  }
  if(newCoefficients.size() == 0) {
    return _0();
  }
  return fetchPolyNode(newCoefficients, u);
}

SNode Expression::DStatementList(const SNode &n, const String &name) const {
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
        d.add(assignStmt(fetchVariableNode(var.name()+_T("'")), D(expr, name)));
      }
      break;
    default:
      throwUnknownSymbolException(method, stmt);
    }
  }
  return d;
}
