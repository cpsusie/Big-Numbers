#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

#define N( n) SNode(n)
#define NV(v) SNode(getTree(),v)

ExpressionFactor *SNode::factor(SNode b, SNode e) { // static
  return b.getTree().fetchFactorNode(b, e);
}

SNode SNode::reduce() {
  ENTERMETHOD();
  const SNodeArray &childArray = getChildArray();
  const int        childCount = (int)childArray.size() - 1;
  SStmtList        newStmtList;

  for (int i = 0; i < childCount; i++) {
    const SNode &stmt = childArray[i];
    newStmtList.add(assignStmt(stmt.left(), stmt.right().reduceRealExp()));
  }
  SNode last = childArray.last();
  switch (last.getReturnType()) {
  case EXPR_RETURN_REAL:
    newStmtList.add(last.reduceRealExp());
    break;
  case EXPR_RETURN_BOOL:
    newStmtList.add(last.reduceBoolExp());
    break;
  default:
    last.throwUnknownSymbolException(__TFUNCTION__);
  }
  RETURNNODE(stmtList(newStmtList.removeUnusedAssignments()));
}

SNode SNode::reduceBoolExp() {
  ENTERMETHOD();
  if (isConstant()) {
    RETURNNODE(NV(evaluateBool()));
  }
  else {
    switch (getSymbol()) {
    case AND:
    case OR : RETURNNODE(reduceAndOr());
    case NOT: RETURNNODE(reduceNot());

    case EQ:
    case NE:
    case LT:
    case LE:
    case GT:
    case GE: RETURNNODE(boolExp(getSymbol(), left().reduceRealExp(), right().reduceRealExp()));
    default:
      throwUnknownSymbolException(__TFUNCTION__);
      RETURNNULL;
    }
  }
}

SNode SNode::reduceRealExp() {
  ENTERMETHOD();
  switch(getSymbol()) {
  case NUMBER         :
  case TYPEBOOL       :
  case NAME           : RETURNNODE( *this );
  case MINUS          : RETURNNODE( -left().reduceRealExp() );
  case SUM            : RETURNNODE( reduceSum()     );
  case PRODUCT        : RETURNNODE( reduceProduct() );
  case POW            : RETURNNODE( reducePower()   );
  case MOD            : RETURNNODE( reduceModulus() );

  case LN             : RETURNNODE( reduceLn()      );
  case LOG10          : RETURNNODE( reduceLog10()   );
  case LOG2           : RETURNNODE( reduceLog2()    );
  case ASIN           :
  case ASINH          :
  case ATAN           :
  case ATANH          :
  case COT            :
  case ERF            :
  case INVERF         :
  case SIGN           :
  case SIN            :
  case SINH           :
  case TAN            :
  case TANH           : RETURNNODE( reduceAsymmetricFunction() );

  case ABS            :
  case COS            :
  case COSH           :
  case GAUSS          : RETURNNODE( reduceSymmetricFunction() );

  case POLY           : RETURNNODE( reducePoly() );

  case ACOS           :
  case ACOT           :
  case ACOSH          :
  case BINOMIAL       :
  case CHI2DENS       :
  case CHI2DIST       :
  case LINCGAMMA      :
  case CEIL           :
  case FAC            :
  case FLOOR          :
  case GAMMA          :
  case MAX            :
  case MIN            :
  case NORM           :
  case PROBIT         : RETURNNODE( reduceTreeNode() );

  case INDEXEDPRODUCT :
    RETURNNODE( indexProd(assignStmt(child(0).left(), child(0).right().reduceRealExp()) // startAssignment
                         ,child(1).reduceRealExp()                                      // endExpr
                         ,child(2).reduceRealExp()                                      // expr
                         )
                );
  case INDEXEDSUM     :
    RETURNNODE( indexSum(assignStmt(child(0).left(), child(0).right().reduceRealExp())  // startAssignment
                        ,child(1).reduceRealExp()                                       // endExpr
                        ,child(2).reduceRealExp()                                       // expr
                        )
              );

  case IIF            : RETURNNODE( reduceCondExp() );

  default             :
    throwUnknownSymbolException(__TFUNCTION__);
    RETURNNULL;
  }
}

//------------------------------------ reduceSum ----------------------------------------

static ExpressionNodeSelector *getTrigonometricFunctionSelector() {
  static const ExpressionSymbolSet    functionSet(SIN,COS,EOI);
  static ExpressionNodeSymbolSelector selector(&functionSet);
  return &selector;
}

static ExpressionNodeSelector *getLogarithmicFunctionSelector() {
  static const ExpressionSymbolSet    functionSet(LN,LOG10,LOG2,EOI);
  static ExpressionNodeSymbolSelector selector(&functionSet);
  return &selector;
}

// symbol == SUM
SNode SNode::reduceSum() const {
  ENTERMETHOD();

  bool anyChanges = false;
  bool hasTrigonometricFunctions = false, hasLogarithmicFunctions = false;
  const AddentArray &a = getAddentArray();

  AddentArray reduced;
  for(size_t i = 0; i < a.size(); i++) {
    SumElement *e = a[i];
    SNode       reducedNode = N(e->getNode()).reduceRealExp();
    if(reducedNode.node() != e->getNode()) anyChanges = true;
    if(!hasTrigonometricFunctions) hasTrigonometricFunctions = reducedNode.node()->getNodeCount(getTrigonometricFunctionSelector()) > 0;
    if(!hasLogarithmicFunctions  ) hasLogarithmicFunctions   = reducedNode.node()->getNodeCount(getLogarithmicFunctionSelector())   > 0;
    reduced.add(reducedNode, e->isPositive());
  }

  if(reduced.size() <= 1) {
    RETURNNODE( NV(reduced) );
  }

  BitSet done(reduced.size());
  do {
    AddentArray tmp = reduced;
    reduced.clear();
    done.setCapacity(tmp.size());
    done.clear();
    SumElement *sqrSinOrCos = NULL;
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      SumElement *e1 = tmp[i1];
      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        SumElement *e2 = tmp[i2];
        if(Expr::equal(e1->getNode(), e2->getNode())) {
          done.add(i1);
          done.add(i2);
          int factor = (e1->isPositive() == e2->isPositive()) ? 2 : 0;
          for(size_t i3 = i2+1; i3 < tmp.size(); i3++) { // check if there are more copies
            if(done.contains(i3)) continue;
            SumElement *e3 = tmp[i3];
            if(Expr::equal(e1->getNode(), e3->getNode())) {
              done.add(i3);
              factor += (e3->isPositive() == e1->isPositive()) ? 1 : -1;
            }
          }
          switch(factor) {
          case  0:
            break; // all copies cancels
          case -1:
            reduced.add(e1->getNode(), !e1->isPositive());
            break;
          case  1:
            reduced.add(e1);
            break;
          case  2:
          case -2:
            reduced.add(_2() * N(e1->getNode()),  (factor>0) == e1->isPositive());
            break;
          default:
            if(factor < 0) {
              reduced.add(NV(-factor) * N(e1->getNode()), !e1->isPositive());
            } else {
              reduced.add(NV( factor) * N(e1->getNode()),  e1->isPositive());
            }
          }
        } else if(hasTrigonometricFunctions && canUseIdiotRule(e1->getNode(), e2->getNode()) && e1->isPositive() == e2->isPositive()) {
          reduced.add(_1(), e1->isPositive());
          done.add(i1);
          done.add(i2);
        } else if(hasTrigonometricFunctions && canUseReverseIdiotRule(e1, e2, sqrSinOrCos)) {
          reduced.add(sqrSinOrCos);
          done.add(i1);
          done.add(i2);
        } else if(hasLogarithmicFunctions && N(e1->getNode()).sameLogarithm(e2->getNode())) {
          reduced.add(mergeLogarithms(*e1, *e2));
          done.add(i1);
          done.add(i2);
        } else {
          SumElement *cf = (getTree().getState() == PS_MAINREDUCTION1) ? getCommonFactor(*e1, *e2) : NULL;
          if(cf != NULL) {
            if(cf->getNode()->getSymbol() != SUM) {
              reduced.add(cf);
            } else {
              ExpressionNode *cn = cf->getNode();
              const AddentArray &a = cn->getAddentArray();
              if(cf->isPositive()) {
                reduced.addAll(a);
              } else {
                for(size_t j = 0; j < a.size(); j++) {
                  SumElement *e = a[j];
                  reduced.add(e->getNode(), !e->isPositive());
                }
              }
            }
            done.add(i1);
            done.add(i2);
          }
        }
      }
    }
    for(size_t i = 0; i < tmp.size(); i++) { // now add the untouched
      if(!done.contains(i)) {
        reduced.add(tmp[i]);
      }
    }
    if(!done.isEmpty()) anyChanges = true;
  } while(!done.isEmpty() && reduced.size() > 1);

  AddentArray tmp = reduced;
  reduced.clear();
  for(size_t i = 0; i < tmp.size(); i++) {
    SumElement     *e  = tmp[i];
    ExpressionNode *n  = e->getNode();
    ExpressionNode *n1 = changeFirstNegativeFactor().node();
    if(n1) {
      reduced.add(n1, !e->isPositive());
      anyChanges = true;
    } else {
      reduced.add(e);
    }
  }

  tmp = reduced;
  reduced.clear();
  Rational constantElements = 0;
  int constAdditionCount = 0;
  for(size_t i = 0; i < tmp.size(); i++) { // then remove all rational constants, added together in constantElements
    SumElement     *e = tmp[i];
    ExpressionNode *a = e->getNode();
    Rational r;
    if(!a->reducesToRationalConstant(&r)) {
      reduced.add(e);
    } else {
      constAdditionCount++;
      if(!a->isNumber() || (constAdditionCount > 1)) anyChanges = true;
      if(e->isPositive()) {
        constantElements += r;
      } else {
        constantElements -= r;
      }
    }
  }

  if(!anyChanges) RETURNNODE( *this );

  reduced.add(NV(constantElements), true);

  RETURNNODE( NV(reduced) );
}

/*
 * n1,n2 expression in a sum
 * return true if n1 = cos^2(expression) and n2 = sin^2(expression) or vice versa
 */
bool SNode::canUseIdiotRule(SNode n1, SNode n2) const {
  ENTERMETHOD2(n1, n2);

  if(n1.getSymbol() == POW && n2.getSymbol() == POW) {
    if(n1.right().isConstant() && n2.right().isConstant()) {
      const Real e1 = n1.right().evaluateReal();
      const Real e2 = n2.right().evaluateReal();
      if((e1 == 2) && (e2 == 2)) {
        const ExpressionInputSymbol f1 = n1.left().getSymbol();
        const ExpressionInputSymbol f2 = n2.left().getSymbol();
        RETURNBOOL ( ((f1 == SIN && f2 == COS) || (f1 == COS && f2 == SIN)) && n1.left().left().equal(n2.left().left()));
      }
    }
  }
  RETURNBOOL( false );
}

static ExpressionInputSymbol getDualTrigonometricFunction(ExpressionInputSymbol symbol) {
  switch(symbol) {
  case SIN: return COS;
  case COS: return SIN;
  default : throwInvalidArgumentException(__TFUNCTION__, _T("symbol=%s"), SNode::getSymbolName(symbol).cstr());
            return EOI;
  }
}

static bool isSinOrCos(SNode n) {
  switch(n.getSymbol()) {
  case SIN:
  case COS:
    return true;
  default:
    return false;
  }
}

bool SNode::isSquareOfSinOrCos() const {
  ENTERMETHOD();

  switch(getSymbol()) {
  case POW:
    RETURNBOOL( isSinOrCos(left()) && right().isConstant() && (right().evaluateReal() == 2) );
  default :
    RETURNBOOL( false );
  }
}

bool SNode::canUseReverseIdiotRule(SumElement *e1, SumElement *e2, SumElement* &result) const {
  ENTERMETHOD2(*e1, *e2);

  for(int i = 0; i < 2; i++) {
    Rational r1;
    if(e1->getNode()->reducesToRationalConstant(&r1)) {
      if(!e1->isPositive()) r1 = -r1;
      if((fabs(r1) == 1) && N(e2->getNode()).isSquareOfSinOrCos() && (r1 == 1) != (e2->isPositive())) {
        ExpressionNode *sinOrCos = e2->getNode()->left();
        SNode           n = pow(unaryExp(getDualTrigonometricFunction(sinOrCos->getSymbol()), sinOrCos->left()), _2());
        result = new SumElement(n, r1 == 1); TRACE_NEW(result);
        RETURNBOOL( true );
      }
    }
    std::swap(e1, e2);
  }
  RETURNBOOL( false );
}

bool SNode::sameLogarithm(SNode n) const {
  ENTERMETHOD2(*this, n);

  RETURNBOOL( ((getSymbol() == LN   ) && (n.getSymbol() == LN   ))
           || ((getSymbol() == LOG10) && (n.getSymbol() == LOG10))
           || ((getSymbol() == LOG2 ) && (n.getSymbol() == LOG2 ))
            );
}

/*
 * e1 assumes e1->getNode.getSymbol = LN,LOG10 or LOG2
 * e2 assumes e2->getNode.getSymbol = e1.getNode.getSymbol
 * return  log(a*b) =  log(a) + log(b),
 *        -log(a*b) = -log(a) - log(b),
 *         log(a/b) =  log(a) - log(b)
 *         log(b/a) = -log(a) + log(b) where log = LN or LOG10
 */
SumElement *SNode::mergeLogarithms(SumElement &e1, SumElement &e2) const {
  ENTERMETHOD2(e1, e2);

  const SNode arg1        = e1.getNode()->left();
  const SNode arg2        = e2.getNode()->left();
  const ExpressionInputSymbol logFunction = e1.getNode()->getSymbol();
  SumElement *result;
  if(e1.isPositive() == e2.isPositive()) { // log(arg1) + log(arg2) = log(arg1*arg2)
    result = new SumElement(unaryExp(logFunction, (arg1 * arg2).reduceRealExp()), e1.isPositive());
  } else if(e1.isPositive()) {             // log(arg1) - log(arg2) = log(arg1/arg2)
    result = new SumElement(unaryExp(logFunction, (arg1 / arg2).reduceRealExp()),true);
  } else {                                 // log(arg2) - log(arg1) = log(arg2/arg1)
    result = new SumElement(unaryExp(logFunction, (arg2 / arg1).reduceRealExp()),true);
  }
  TRACE_NEW(result);
  RETURN(result);
}

/*
 * e1, e2  elements in a sum
 * return If(e1 == a*b and e2 == a*c)
 *          then return a*(b+c). Distributive low for * and +/-
 *        If(e1 == p1 * a^c1 and e2 == p2 * a^c2)
 *          then, assuming c1 < c2, return a^c1 * (p1 + p2 * a^(c2-c1))
 *          and symmetric if c1 > c2 (taking care of signs of e1 and e2 too)
 * return NULL If no common factors found
 */
SumElement *SNode::getCommonFactor(SumElement &e1, SumElement &e2) const {
  ENTERMETHOD2(e1, e2);

  FactorArray fl1, fl2;
  N(e1.getNode()).getFactors(fl1);
  N(e2.getNode()).getFactors(fl2);

  FactorArray commonFactors;
  int signShiftCount = 0;

StartSearch:
  for(size_t i1 = 0; i1 < fl1.size(); i1++) {
    ExpressionFactor *factor1 = fl1[i1];
    for(size_t i2 = 0; i2 < fl2.size(); i2++) {
      ExpressionFactor *factor2 = fl2[i2];

      if(Expr::equal(factor1, factor2)) {
        fl1.remove(i1);
        fl2.remove(i2);
        commonFactors.add(factor1);
        goto StartSearch;
      } else if((factor1->base().equal(factor2->base()))
            &&  (factor1->exponent().isNumber() && factor2->exponent().isNumber())) { // exponents are different
        fl1.remove(i1);
        fl2.remove(i2);
        SNode         base        = factor1->base(); // == factor2->base()
        const Number &c1          = factor1->exponent().getNumber();
        const Number &c2          = factor2->exponent().getNumber();
        const Number *minExponent , *maxExponent;
        FactorArray  *flEMin      , *flEMax;

        if(c1 < c2) {
          minExponent  = &c1 ; maxExponent  = &c2;
          flEMin       = &fl1; flEMax       = &fl2;
        } else {
          minExponent  = &c2 ; maxExponent  = &c1;
          flEMin       = &fl2; flEMax       = &fl1;
        }
        commonFactors.add(base, NV(*minExponent));
        flEMax->add(base, NV(*maxExponent - *minExponent));
        goto StartSearch;
      } else {
        Rational eR1,eR2;
        if((factor1->exponent().reducesToRationalConstant(&eR1) && factor2->exponent().reducesToRationalConstant(&eR2))
         && factor1->base().equalMinus(factor2->base())) {
          if(isAsymmetricExponent(eR1) && isAsymmetricExponent(eR2)) {
            signShiftCount++;
          } else if(isSymmetricExponent(eR1) && isSymmetricExponent(eR2)) {
            // do nothing
          } else {
            continue;
          }
          fl1.remove(i1);
          fl2.remove(i2);

          SNode           base = factor1->base(); // == -factor2->base()
          const Rational *minExponent , *maxExponent;
          FactorArray    *flEMin      , *flEMax;

          if(eR1 == eR2) {
            commonFactors.add(factor1);
          } else {
            if(eR1 < eR2) {
              minExponent  = &eR1; maxExponent = &eR2;
              flEMin       = &fl1; flEMax      = &fl2;
            } else { // e1 > e2
              minExponent  = &eR2; maxExponent = &eR1;
              flEMin       = &fl2; flEMax      = &fl1;
            }
            commonFactors.add(base, NV(*minExponent));
            flEMax->add(base, NV(*maxExponent - *minExponent));
          }
          goto StartSearch;
        }
      }
    }
  }
  if(commonFactors.size() == 0) {
    RETURNNULL;
  }
  SNode bc;
  const SNode a = NV(commonFactors);

  bool positive = true;
  const bool e2positive = (e2.isPositive() == ::isEven(signShiftCount));

  if(e1.isPositive() == e2positive) {
    bc = NV(fl1) + NV(fl2);
    positive = e1.isPositive();
  } else if(e1.isPositive()) { // a2 negative
    bc = NV(fl1) - NV(fl2);
  } else { // e1 negative and e2 positive
    bc = NV(fl2) - NV(fl1);
  }
  SumElement *result = new SumElement(a * bc, positive); TRACE_NEW(result);
  RETURNPSHOWSTR( result );
}

SNode SNode::changeFirstNegativeFactor() const {
  ENTERMETHOD();
  switch(getSymbol()) {
  case NUMBER :
    if(isNegative()) {
      RETURNNODE( NV(-getNumber()) );
    }
    break;
  case PRODUCT:
    { const FactorArray &factorArray = getFactorArray();
      if(factorArray.size() == 0) RETURNNULL;
      FactorArray newFactorArray;
      ExpressionFactor *f0 = factorArray[0];
      if(f0->base().isNegative() && f0->exponent().isOdd()) {
        newFactorArray.add(-f0->base(), f0->exponent());
        for(size_t i = 1; i < factorArray.size(); i++) newFactorArray.add(factorArray[i]);
        RETURNNODE( NV(newFactorArray) );
      }
    }
    break;
  case POW    :
    if(left().isNegative() && right().isOdd()) {
      RETURNNODE( pow(NV(-left().getNumber()), right()) );
    }
    break;
  }
  RETURNNULL;
}

//------------------------------------ reduceProduct ----------------------------------------

// Symbol = PRODUCT
SNode SNode::reduceProduct() {
  ENTERMETHOD();

  FactorArray unreducedFactors, nonConstantFactors, constantFactors;
  getFactors(unreducedFactors);

  for(size_t i = 0; i < unreducedFactors.size(); i++) { // first remove all number factors, multiplied together in constantFactor
    ExpressionFactor *f = unreducedFactors[i];
    if(!f->isConstant()) {
      nonConstantFactors.add(f); // contains non-constant factors
    } else {
      Rational r;
      if(!f->reducesToRational(&r)) {
        constantFactors.add(f);
      } else if(r == 0) {
        RETURNNODE( _0() );
      } else if(r != 1) { // No need to add 1 as a factor
        constantFactors.add(NV(r));
      }
    }
  }

  // nonConstantFactors contains no constant factors
  FactorArray reduced = nonConstantFactors;

  if(reduced.size() > 1) {
    BitSet done(reduced.size());
    do {
      FactorArray tmp = reduced;;
      reduced.clear();
      done.setCapacity(tmp.size());
      done.clear();
      for(size_t i1 = 1; i1 < tmp.size(); i1++) {
        ExpressionFactor *f1 = tmp[i1];
        for(size_t i2 = 0; i2 < i1; i2++) {
          if(done.contains(i1)) break;
          if(done.contains(i2)) continue;
          ExpressionFactor *f2 = tmp[i2];
          if(f1->base().equal(f2->base())) { // Common base
            SNode newExponent = (f1->exponent() + f2->exponent()).reduceRealExp();
            if(!newExponent.isZero()) {
              reduced.add(f1->base(), newExponent);
            }
            done.add(i1);
            done.add(i2);
          } else {
            ExpressionFactor *f = reduceTrigonometricFactors(*f1, *f2);
            if(f != NULL) {
              done.add(i1);
              done.add(i2);
              if(!f->isConstant()) {
                reduced.add(f);
              } else {
                Rational r;
                if(!f->reducesToRational(&r)) {
                  constantFactors.add(f);
                } else if(r == 0) {        // No need to go further. The product is 0
                  RETURNNODE( _0() );
                } else if(r != 1) {        // No need to add 1 as a factor
                  constantFactors.add(NV(r));
                }
              }
            }
          }
        }
      }
      for(size_t i = 0; i < tmp.size(); i++) {
        if(!done.contains(i)) {
          reduced.add(tmp[i]);
        }
      }
    } while(!done.isEmpty() && reduced.size() > 1);
  }

  SNode constantFactor = reduceConstantFactors(constantFactors);
  if(!constantFactor.isOne()) {
    reduced.add(constantFactor);
  }
  const SNode result = NV(reduced);
  RETURNNODE( result );
}

FactorArray &SNode::getFactors(FactorArray &result) {
  return getFactors(result, _1());
}

FactorArray &SNode::getFactors(FactorArray &result, SNode exponent) {
  ENTERMETHOD2(*this, exponent);

  switch(getSymbol()) {
  case NUMBER:
    if(!isOne()) { // 1 should not be added
      const Number &v = getNumber();
      if(v.isRational()) {
        const Rational r = v.getRationalValue();
        if(abs(r.getNumerator()) == 1) {
          result.add(NV(sign(r.getNumerator()) * r.getDenominator()), (-exponent).reduceRealExp());
          break;
        }
      }
      result.add(*this, exponent);
    }
    break;
  case PRODUCT:
    { const FactorArray &a = getFactorArray();
      for(size_t i = 0; i < a.size(); i++) N(a[i]).getFactors(result, exponent);
    }
    break;
  case POW :
    getFactorsInPower(result, exponent);
    break;
  default:
    result.add(reduceRealExp(), exponent);
    break;
  }
  RETURNSHOWSTR( result );
}

/*
 * symbol = POW
 * exponent is the outer exponent of this. value = pow(pow(left,right),exponent)
 * return this split into as many separate factors as possible
 */
FactorArray &SNode::getFactorsInPower(FactorArray &result, SNode exponent) {
  ENTERMETHOD2(*this, exponent);
  FactorArray tmp1, tmp2;
  SNode base = left();
  switch(base.getSymbol()) {
  case POW :
    multiplyExponents(tmp2, base.left().reduceRealExp().getFactors(tmp1, base.right().reduceRealExp()), right().reduceRealExp());
    break;
  default  :
    base.reduceRealExp().getFactors(tmp2, right().reduceRealExp());
    break;
  }
  multiplyExponents(result, tmp2, exponent);
  RETURNSHOWSTR( result );
}

// symbol = POW
SNode SNode::reducePower() {
  ENTERMETHOD();

  SNode base  = left();
  SNode expo  = right();

  const SNode rBase = base.reduceRealExp();
  const SNode rExpo = expo.reduceRealExp();
  SNode result;

  if(rBase.getSymbol() == POW) {
    result = pow(rBase.left(), rBase.right() * rExpo);
  } else if(rBase.isSameNode(base) && rExpo.isSameNode(expo)) { // nothing changed
    result = *this;
  } else {
    result = pow(rBase, rExpo);
  }
  Rational tmp;
  if(result.reducesToRationalConstant(&tmp)) {
    result = NV(tmp);
  }
  RETURNNODE( result );
}

/* factors list of ExpressionFactor
 * factor factor to multiply with
 * return list of expressionFactors, result[i] = fetchFactorNode(factors[i].base,factors[i].exponent * factor)
 */
FactorArray &SNode::multiplyExponents(FactorArray &result, FactorArray &factors, SNode exponent) {
  ENTERMETHOD2(factors, exponent);

  if(exponent.isOne()) {
    result.addAll(factors);
    RETURNSHOWSTR( result );
  } else if(exponent.isZero()) {
    RETURNSHOWSTR( result ); // the empty list has value 1
  }

  Rational   exponentR                  = 1;
  const bool exponentIsRationalConstant = exponent.reducesToRationalConstant(&exponentR);

  for(size_t i = 0; i < factors.size(); i++) {
    ExpressionFactor     *f  = factors[i];
    const SNode           e  = f->exponent();
    Rational              eR = 1;
    const bool            eIsRationalConstant = e.reducesToRationalConstant(&eR);
    if(exponentIsRationalConstant && eIsRationalConstant) {
      if(rationalExponentsMultiply(eR, exponentR)) {
        result.add(f->base(), NV(eR * exponentR));
      } else {
        const Rational newE = eR * exponentR;
        result.add(f->base(), NV(newE.getNumerator()*2) / NV(newE.getDenominator()*2));
      }
    } else {
      result.add(f, exponent);
    }
  }
  RETURNSHOWSTR( result );
}

bool SNode::rationalExponentsMultiply(const Rational &r1, const Rational &r2) { // static
  return isAsymmetricExponent(r1) || isAsymmetricExponent(r2);
}

SNode SNode::multiplySumSum(SNode n1, SNode n2) const {
  const AddentArray &aa1 = n1.getAddentArray();
  const AddentArray &aa2 = n2.getAddentArray();

  ParserTree &tree = n1.getTree();
  AddentArray newAddentArray(aa1.size() * aa2.size());
  for(size_t i = 0; i < aa1.size(); i++) {
    SumElement *e1 = aa1[i];
    const SNode s1 = e1->getNode();
    for(size_t j = 0; j < aa2.size(); j++) {
      SumElement *e2 = aa2[j];
      const SNode s2 = e2->getNode();
      newAddentArray.add(s1 * s2, e1->isPositive() == e2->isPositive());
    }
  }
  ExpressionNodeSum *n = new ExpressionNodeSum(&tree, newAddentArray); TRACE_NEW(n); // dont use getSum here. It has to be an ExpressionNodeSum
  return n;
}

ExpressionFactor *SNode::reduceTrigonometricFactors(ExpressionFactor &f1, ExpressionFactor &f2) {
  ENTERMETHOD2(f1,f2);

  if(!f1.base().isTrigonomtricFunction() || !f2.base().isTrigonomtricFunction()) {
    RETURNNULL;
  }
  SNode arg = f1.base().left();
  if(!arg.equal(f2.base().left())) {
    RETURNNULL;
  }

  if(!f1.exponent().isNumber() || !f2.exponent().isNumber()) {
    RETURNNULL;
  }
  const Real e1 = f1.exponent().getReal();
  const Real e2 = f2.exponent().getReal();

  switch(f1.base().getSymbol()) {
  case SIN:
    switch(f2.base().getSymbol()) {
    case COS: RETURN( e1 == -e2 ? factor(unaryExp(TAN,arg),f1.exponent()) : NULL );
    case TAN: RETURN( e1 == -e2 ? factor(unaryExp(COS,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case COS:
    switch(f2.base().getSymbol()) {
    case SIN: RETURN( e1 == -e2 ? factor(unaryExp(TAN,arg),f2.exponent()) : NULL );
    case TAN: RETURN( e1 ==  e2 ? factor(unaryExp(SIN,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case TAN:
    switch(f2.base().getSymbol()) {
    case SIN: RETURN( e1 == -e2 ? factor(unaryExp(COS,arg),f2.exponent()) : NULL );
    case COS: RETURN( e1 ==  e2 ? factor(unaryExp(SIN,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  }
  RETURNNULL;
}

SNode SNode::reduceConstantFactors(FactorArray &factorArray) {
  ENTERMETHOD1(factorArray);

  if(factorArray.size() == 0) {
    RETURNNODE( _1() );
  }

  FactorArray reduced = factorArray;
  for(int startSize = (int)reduced.size(); startSize > 0; startSize = (int)reduced.size()) {
    const FactorArray tmp = reduced;
    reduced.clear();
    BitSet done(tmp.size());
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      if(done.contains(i1)) continue;
      ExpressionFactor       *f1                   = tmp[i1];
      SNode                   base1                = f1->base();
      SNode                   expo1                = f1->exponent();
      Rational                B1R, E1R;
      const bool              b1IsRationalConstant = base1.reducesToRationalConstant(&B1R);
      const bool              e1IsRationalConstant = expo1.reducesToRationalConstant(&E1R);
      SNode                   reducedBase1         = b1IsRationalConstant ? NV(B1R) : base1;

      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        ExpressionFactor *f2    = tmp[i2];
        SNode             base2 = f2->base();
        SNode             expo2 = f2->exponent();
        Rational          B2R, E2R;
        const bool        b2IsRationalConstant = base2.reducesToRationalConstant(&B2R);
        const bool        e2IsRationalConstant = expo2.reducesToRationalConstant(&E2R);
        SNode             reducedBase2         = b2IsRationalConstant ? NV(B2R) : base2;

        if(e1IsRationalConstant && e2IsRationalConstant) {
          if(E1R == E2R) {
            reduced.add(reducedBase1 * reducedBase2, E1R);
            done.add(i1);
            done.add(i2);
          } else if(E1R == -E2R) {
            if(E1R > 0) {
              reduced.add(reducedBase1 / reducedBase2, E1R);
            } else {
              reduced.add(reducedBase2 / reducedBase1, E2R);
            }
            done.add(i1);
            done.add(i2);
          } else if(base1.equal(base2)) {
            reduced.add(reducedBase1, E2R + E1R);
            done.add(i1);
            done.add(i2);
          }
        } else if(expo1.equal(expo2)) {
          reduced.add(reducedBase1 * reducedBase2, expo1);
          done.add(i1);
          done.add(i2);
        }
      }
    }
    for(size_t i = 0; i < tmp.size(); i++) {
      if(!done.contains(i)) {
        reduced.add(tmp[i]);
      }
    }
    if(reduced.size() == startSize) {
      break;
    }
  }

  Rational    rationalPart = 1;
  FactorArray fa;

  for(size_t i = 0; i < reduced.size(); i++) {
    ExpressionFactor *f = reduced[i];
    Rational r;
    if(f->reducesToRationalConstant(&r)) {
      if(r == 0) {
        RETURNNODE( _0() );
      } else if(r == 1) {
        continue;
      } else {
        rationalPart *= r;
      }
    } else {
      SNode base = f->base();
      SNode expo = f->exponent();
      Rational baseR, expoR;
      if(base.reducesToRationalConstant(&baseR)) {
        if(expo.reducesToRationalConstant(&expoR)) {
          fa.add(reduceRationalPower(baseR, expoR));
        } else { // base is rational. expo is irrational
          fa.add(NV(baseR), expo);
        }
      } else {
        fa.add(f);
      }
    }
  }

  if(rationalPart != 1) {
    const INT64 &num = rationalPart.getNumerator();
    const INT64 &den = rationalPart.getDenominator();
    if(num != 1) {
      fa.add(NV(num));
    }
    if(den != 1) {
      fa.add(NV(den), _m1());
    }
  }
  RETURNNODE( NV(fa) );
}

SNode SNode::reduceRationalPower(const Rational &base, const Rational &exponent) {
  ENTERMETHOD2NUM(base, exponent);

  if(exponent.isInteger()) {
    RETURNNODE( NV(pow(base, getInt(exponent))) );
  } else {
    const INT64 &ed = exponent.getDenominator();
    const INT64 &bn = base.getNumerator();
    const INT64 &bd = base.getDenominator();
    PrimeFactorArray bnPrimeFactors(bn);
    PrimeFactorArray bdPrimeFactors(bd);

    PrimeFactorSet bnRootFactors = bnPrimeFactors.findFactorsWithMultiplicityAtLeast((UINT)ed);
    PrimeFactorSet bdRootFactors = bdPrimeFactors.findFactorsWithMultiplicityAtLeast((UINT)ed);
    Rational niceRootFactor = 1;
    __int64 bnR = 1, bdR = 1;
    if((abs(bn) == 1 || !bnRootFactors.isEmpty()) && (bd == 1 || !bdRootFactors.isEmpty())) {
      for(Iterator<size_t> it1 = bnRootFactors.getIterator(); it1.hasNext();) {
        PrimeFactor &pf = bnPrimeFactors[it1.next()];
        do {
          bnR *= pf.m_prime;
          pf.m_multiplicity -= (UINT)ed;
        } while(pf.m_multiplicity >= (UINT)ed);
      }
      for(Iterator<size_t> it2 = bdRootFactors.getIterator(); it2.hasNext();) {
        PrimeFactor &pf = bdPrimeFactors[it2.next()];
        do {
          bdR *= pf.m_prime;
          pf.m_multiplicity -= (UINT)ed;
        } while(pf.m_multiplicity >= (UINT)ed);
      }
    }

    if(bnPrimeFactors.isPositive()) {
      niceRootFactor = Rational(bnR, bdR);
    } else if(isAsymmetricExponent(exponent)) {
      niceRootFactor = Rational(-bnR, bdR);
      bnPrimeFactors.setPositive();
    } else {
      throwInvalidArgumentException(__TFUNCTION__, _T("Base:%s, exponent:%s"), ::toString(base).cstr(), ::toString(exponent).cstr());
    }

    FactorArray fa;
    if(niceRootFactor != 1) {
      fa.add(NV(niceRootFactor), NV(exponent.getNumerator()));
    }

    const Rational nonRootFactor = pow(Rational(bnPrimeFactors.getProduct(), bdPrimeFactors.getProduct()), (int)exponent.getNumerator());
    if(nonRootFactor != 1) {
      if(nonRootFactor.getNumerator() == 1) {
        fa.add(NV(nonRootFactor.getDenominator()), NV(Rational(-1,ed)));
      } else {
        fa.add(NV(nonRootFactor), NV(Rational(1,ed)));
      }
    }
    RETURNNODE( NV(fa) );
  }
}

SNode SNode::reduceModulus() const {
  ENTERMETHOD();

  SNode l  = left();
  SNode r  = right();

  const SNode Rl = l.reduceRealExp();
  SNode Rr = r.reduceRealExp();
  if(Rr.isNegative() || Rr.isUnaryMinus()) Rr = -Rr;

  if(Rl.isSameNode(l) && Rr.isSameNode(r)) {
    RETURNNODE( *this );
  } else {
    RETURNNODE( Rl % Rr );
  }
}

/*
 * symbol = LN
 * return if argument is an integer power p of e then p else LN(reduce(leftChild))
 */
SNode SNode::reduceLn() {
  ENTERMETHOD();

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOfE();
  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) { // ln(a^b) = b * ln(a)
    RETURNNODE( Rarg.right() * unaryExp(getSymbol(), Rarg.left()) );
  }
  RETURNNODE( Rarg.isSameNode(arg) ? *this : unaryExp(getSymbol(), Rarg) );
}

SNode SNode::getPowerOfE() {
  ENTERMETHOD();

  if(isEulersConstant()) {
    RETURNNODE( _1() );
  } else if(isOne()) {
    RETURNNODE( _0() );
  } else if((getSymbol() == POW) && left().isEulersConstant()) {
    RETURNNODE( right() );
  } else {
    RETURNNULL;
  }
}

/*
 * symbol = LOG10
 * return if argument is an integer power p of 10 then p else LOG10(reduce(leftChild))
 */
SNode SNode::reduceLog10() {
  ENTERMETHOD();

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOf10();

  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) {
    RETURNNODE( Rarg.right() * unaryExp(getSymbol(), Rarg.left()) );
  }
  RETURNNODE( Rarg.isSameNode(arg) ? *this : unaryExp(getSymbol(), Rarg) );
}

SNode SNode::getPowerOf10() {
  ENTERMETHOD();

  if(isTen()) {
    RETURNNODE( _1() );
  } else if(isOne()) {
    RETURNNODE( _0() );
  } else if((getSymbol() == POW) && left().isTen()) {
    RETURNNODE( right() );
  } else {
    RETURNNULL;
  }
}

/*
 * symbol = LOG2
 * return if argument is an integer power p of 2 then p else LOG2(reduce(leftChild))
 */
SNode SNode::reduceLog2() {
  ENTERMETHOD();

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOf2();

  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) {
    RETURNNODE( Rarg.right() * unaryExp(getSymbol(), Rarg.left()) );
  }
  RETURNNODE( Rarg.isSameNode(arg) ? *this : unaryExp(getSymbol(), Rarg) );
}

SNode SNode::getPowerOf2() {
  ENTERMETHOD();

  if(isTen()) {
    RETURNNODE( _1() );
  } else if(isOne()) {
    RETURNNODE( _0() );
  } else if((getSymbol() == POW) && left().isTwo()) {
    RETURNNODE( right() );
  } else {
    RETURNNULL;
  }
}

SNode SNode::reduceAsymmetricFunction() {
  ENTERMETHOD();

  SNode arg  = left();
  if(getInverseFunction() == arg.getSymbol()) RETURNNODE( arg.left() );
  const SNode Rarg = arg.reduceRealExp();
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = -f(exp)
    RETURNNODE( -unaryExp(getSymbol(), Rarg.left()) );
  } else {
    RETURNNODE( Rarg.isSameNode(arg) ? *this : unaryExp(getSymbol(), Rarg) );
  }
}

SNode SNode::reduceSymmetricFunction() {
  ENTERMETHOD();

  SNode arg  = left();
  if(getInverseFunction() == arg.getSymbol()) RETURNNODE( arg.left() );
  const SNode Rarg = arg.reduceRealExp();
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = f(exp)
    RETURNNODE( unaryExp(getSymbol(), Rarg.left()) );
  } else {
    RETURNNODE( Rarg.isSameNode(arg) ? *this : unaryExp(getSymbol(), Rarg) );
  }
}

/*
 * symbol = POLY
 * return no leading zeroes, all constant coefficients evaluated
 */
SNode SNode::reducePoly() {
  ENTERMETHOD();

  SNodeArray &coefArray = getCoefArray();
  SNode       arg       = getArgument();  // arg is the parameter to the polynomial ex. poly[a,b,c,d](arg)

  SNodeArray newCoefArray;
  bool leadingCoef = true;
  for(size_t i = 0; i < coefArray.size(); i++) {
    SNode &coef = coefArray[i];
    if(!coef.isConstant()) {     // coef not constant
      newCoefArray.add(coef.reduceRealExp());
    } else if(coef.isNumber()) { // coef is constant
      if(coef.isZero() && leadingCoef) continue;
      newCoefArray.add(coef);
    } else {
      Rational r;
      if(coef.reducesToRationalConstant(&r)) {
        if(r.isZero() && leadingCoef) continue;
        newCoefArray.add(NV(r));
      } else {
        const Real c = coef.evaluateReal();
        if(c == 0 && leadingCoef) continue;
        newCoefArray.add(NV(c));
      }
    }
    leadingCoef = false;
  }
  switch(newCoefArray.size()) {
  case 0 : RETURNNODE( _0() );            // 0 polynomial == 0
  case 1 : RETURNNODE( newCoefArray[0] ); // Independent of arg
  default:
    { const SNode newArg = arg.reduceRealExp();
      if((newCoefArray.isSameNodes(coefArray)) && (newArg.isSameNode(arg))) {
        RETURNNODE(*this);
      } else {
        RETURNNODE( polyExp(newCoefArray, newArg) );
      }
    }
  }
}

SNode SNode::reduceCondExp() {
  ENTERMETHOD();

  SNode trueExp = child(1).reduceRealExp(), falseExp = child(2).reduceRealExp();
  if(trueExp.equal(falseExp)) {
    RETURNNODE( trueExp );
  }
  RETURNNODE( condExp(child(0).reduceBoolExp(), trueExp, falseExp ) );
}

SNode SNode::reduceTreeNode() {
  ENTERMETHOD();

  if(getInverseFunction() == left().getSymbol()) {
    RETURNNODE( left().left() );
  }

  switch(getSymbol()) {
  case IIF:
    { const SNode cond   = child(0).reduceBoolExp();
      const SNode eTrue  = child(1).reduceRealExp();
      const SNode eFalse = child(2).reduceRealExp();
      if(cond.isTrue() ) RETURNNODE( eTrue  );
      if(cond.isFalse()) RETURNNODE( eFalse );
      RETURNNODE( condExp(cond, eTrue, eFalse) );
    }
  default:
    { const SNodeArray &a = getChildArray();
      SNodeArray newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(N(a[i]).reduceRealExp());
      }
      SNode result = treeExp(getSymbol(), newChildArray);
      RETURNNODE( result );
    }
  }
}

}; // namespace Expr
