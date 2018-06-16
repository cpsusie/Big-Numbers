#include "pch.h"
#include <Math/PrimeFactors.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

SNode SNode::reduce() {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_STMTLIST);
  if(isReduced()) RETURNTHIS;
  const SNodeArray &childArray = getChildArray();
  const size_t      childCount = childArray.size();
  const size_t      stmtCount  = childCount - 1;
  StmtList          newStmtList(getTree(), childCount);

  for(size_t i = 0; i < stmtCount; i++) {
    newStmtList.add(childArray[i].reduceAssign());
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
  newStmtList.removeUnusedAssignments();
  if(newStmtList.isSameNodes(childArray)) {
    setReduced();
    RETURNTHIS;
  }
  RETURNNODE(stmtList(newStmtList));
}

SNode SNode::reduceAssign() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_ASSIGN);
  if(isReduced()) RETURNTHIS;
  SNode Rr = right().reduceRealExp();
  if(Rr.isSameNode(right())) {
    right().setReduced();
    setReduced();
    RETURNTHIS;
  }
  return assignStmt(left(), Rr);
}

SNode SNode::reduceBoolExp() {
  ENTERMETHOD();
  CHECKNODERETURNTYPE(*this,EXPR_RETURN_BOOL);
  if(isReduced()) RETURNTHIS;
  if(isConstant()) {
    RETURNNODE(SNV(evaluateBool()));
  } else {
    switch(getSymbol()) {
    case AND:
    case OR : RETURNNODE(reduceAndOr());
    case NOT: RETURNNODE(reduceNot());

    case EQ:
    case NE:
    case LT:
    case LE:
    case GT:
    case GE:
      { SNode Rl = left().reduceRealExp(), Rr = right().reduceRealExp();
        if(Rl.isSameNode(left()) && Rr.isSameNode(right())) {
          setReduced();
          RETURNTHIS;
        } else {
          RETURNNODE(boolExp(getSymbol(), Rl, Rr));
        }
      }
    default:
      throwUnknownSymbolException(__TFUNCTION__);
      RETURNNULL;
    }
  }
}

SNode SNode::reduceRealExp() {
  ENTERMETHOD();
  CHECKNODERETURNTYPE(*this, EXPR_RETURN_REAL);
  if(isReduced()) RETURNTHIS;
  switch(getSymbol()) {
  case NUMBER         :
  case TYPEBOOL       :
  case NAME           :
    setReduced();
    RETURNTHIS;
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
  case ATAN2          :
  case ACOSH          :
  case ACSC           :
  case ASEC           :
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
  case HYPOT          :
  case NORM           :
  case PROBIT         : RETURNNODE( reduceTreeNode() );
  case INDEXEDSUM     :
  case INDEXEDPRODUCT : RETURNNODE( reduceIndexedExpr() );
  case IIF            : RETURNNODE( reduceCondExp() );
  default             :
    throwUnknownSymbolException(__TFUNCTION__);
    RETURNNULL;
  }
}

SNode SNode::reduceIndexedExpr() {
  ENTERMETHOD();
  if(isReduced()) RETURNTHIS;

  SNode assign  = child(0);
  SNode end     = child(1);
  SNode exp     = child(2);
  SNode Rassign = assign.reduceAssign();
  SNode Rend    = end.reduceRealExp();
  SNode Rexp    = exp.reduceRealExp();
  if(Rassign.isSameNode(assign) && Rend.isSameNode(end) && Rexp.isSameNode(exp)) {
    setReduced();
    RETURNTHIS;
  }

  RETURNNODE( indexedExp(getSymbol(), Rassign, Rend, Rexp) );
}

//------------------------------------ reduceSum ----------------------------------------

// symbol == SUM
SNode SNode::reduceSum() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_SUM);
  if(isReduced()) RETURNTHIS;

  static const ExpressionSymbolSet logFunctionSet(   LN,LOG10,LOG2,EOI);
  static const ExpressionSymbolSet sinCosFunctionSet(SIN,COS,      EOI);

  bool hasTrigonometricFunctions = false, hasLogarithmicFunctions = false;
  AddentArray a(getTree());
  getAddents(a);

  AddentArray reduced(a.getTree());
  for(size_t i = 0; i < a.size(); i++) {
    SNode e = a[i], n = e.left();
    SNode Rn = n.reduceRealExp();
    if(!hasTrigonometricFunctions) hasTrigonometricFunctions = Rn.getNodeCount(sinCosFunctionSet) > 0;
    if(!hasLogarithmicFunctions  ) hasLogarithmicFunctions   = Rn.getNodeCount(logFunctionSet   ) > 0;
    if(Rn.isSameNode(n)) {
      reduced.add(e);
    } else {
      reduced.add(addentExp(Rn, e.isPositive()));
    }
  }

  if(reduced.size() <= 1) {
    if(reduced.isSameNodes(a)) {
      setReduced();
      RETURNTHIS;
    }
    SNode result = sumExp(reduced);
    result.setReduced();
    RETURNNODE( result );
  }

  BitSet done(reduced.size());
  do {
    AddentArray tmp = reduced;
    reduced.clear();
    done.setCapacity(tmp.size());
    done.clear();
    SNode sqrSinOrCos;
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      SNode e1 = tmp[i1];
      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        SNode e2 = tmp[i2];
        if(e1.left().equal(e2.left())) {
          done.add(i1);
          done.add(i2);
          int factor = (e1.isPositive() == e2.isPositive()) ? 2 : 0;
          for(size_t i3 = i2+1; i3 < tmp.size(); i3++) { // check if there are more copies
            if(done.contains(i3)) continue;
            SNode e3 = tmp[i3];
            if(e1.left().equal(e3.left())) {
              done.add(i3);
              factor += (e3.isPositive() == e1.isPositive()) ? 1 : -1;
            }
          }
          switch(factor) {
          case  0:
            break; // all copies cancels
          case -1:
            reduced.add(addentExp(e1.left(), !e1.isPositive()));
            break;
          case  1:
            reduced.add(e1);
            break;
          case  2:
          case -2:
            reduced.add(addentExp(_2() * e1.left(),  (factor>0) == e1.isPositive()));
            break;
          default:
            if(factor < 0) {
              reduced.add(addentExp(SNV(-factor) * e1.left(), !e1.isPositive()));
            } else {
              reduced.add(addentExp(SNV( factor) * e1.left(),  e1.isPositive()));
            }
          }
        } else if(hasTrigonometricFunctions && canUseIdiotRule(e1.left(), e2.left()) && (e1.isPositive() == e2.isPositive())) {
          reduced.add(addentExp(_1(), e1.isPositive()));
          done.add(i1);
          done.add(i2);
        } else if(hasTrigonometricFunctions && canUseReverseIdiotRule(e1, e2, sqrSinOrCos)) {
          reduced.add(sqrSinOrCos);
          done.add(i1);
          done.add(i2);
        } else if(hasLogarithmicFunctions && e1.left().sameLogarithm(e2.left())) {
          reduced.add(mergeLogarithms(e1, e2));
          done.add(i1);
          done.add(i2);
        } else {
          SNode cf = (getTree().getState() == PS_MAINREDUCTION1) ? getCommonFactor(e1, e2) : NULL;
          if(!cf.isEmpty()) {
            if(cf.left().getSymbol() != SUM) {
              reduced.add(cf);
            } else {
              if(cf.isPositive()) {
                reduced += cf.left().getAddentArray();
              } else {
                reduced -= cf.left().getAddentArray();
              }
            }
            done.add(i1);
            done.add(i2);
          }
        }
      }
    }
    reduced += tmp.selectNodes(compl(done)); // now add the untouched
  } while(!done.isEmpty() && reduced.size() > 1);

  AddentArray tmp = reduced;
  reduced.clear();
  for(size_t i = 0; i < tmp.size(); i++) {
    SNode  e  = tmp[i], n  = e.left();
    SNode  n1 = changeFirstNegativeFactor();
    if(!n1.isEmpty()) {
      reduced.add(addentExp(n1, !e.isPositive()));
    } else {
      reduced.add(e);
    }
  }

  tmp = reduced;
  reduced.clear();
  Rational rationalAcc         = 0;
  BitSet   rationalAddentSet(tmp.size());
  // then replace all rational constants, added together in rationalAcc
  // if only 1 rational exist, then reuse it
  for(size_t i = 0; i < tmp.size(); i++) { 
    SNode e = tmp[i], c = e.left();
    Rational r;
    if(!c.reducesToRationalConstant(&r)) {
      reduced.add(e);
    } else {
      if(c.isRational()) {
        rationalAddentSet.add(i);
      }
      if(e.isPositive()) {
        rationalAcc += r;
      } else {
        rationalAcc -= r;
      }
    }
  }
  if(rationalAddentSet.size() == 1) {
    reduced.add(tmp[rationalAddentSet.select()]); // put the one found back again, to keep array the same (if not changed already)
  } else if(rationalAcc != 0) {
    reduced += rationalAcc;
  }
  reduced.sort();
  if(reduced.isSameNodes(a)) {
    setReduced();
    RETURNTHIS;
  }
  RETURNNODE( sumExp( reduced) );
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
        RETURNBOOL( ((f1 == SIN && f2 == COS) || (f1 == COS && f2 == SIN)) && n1.left().left().equal(n2.left().left()));
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

bool SNode::canUseReverseIdiotRule(SNode e1, SNode e2, SNode &result) const {
  ENTERMETHOD2(e1, e2);

  for(int i = 0; i < 2; i++) {
    Rational r1;
    if(e1.left().reducesToRationalConstant(&r1)) {
      if(!e1.isPositive()) r1 = -r1;
      if((fabs(r1) == 1) && e2.left().isSquareOfSinOrCos() && (r1 == 1) != (e2.isPositive())) {
        SNode sinOrCos = e2.left().left();
        SNode n = pow(unaryExp(getDualTrigonometricFunction(sinOrCos.getSymbol()), sinOrCos.left()), _2());
        result = addentExp(n, r1 == 1);
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
SNode SNode::mergeLogarithms(SNode e1, SNode e2) const {
  ENTERMETHOD2(e1, e2);
  CHECKSYMBOL(e1,e2.getSymbol());

  SNode arg1 = e1.left().left();
  SNode arg2 = e2.left().left();
  const ExpressionInputSymbol logFunction = e1.left().getSymbol();
  SNode result;
  if(e1.isPositive() == e2.isPositive()) { // log(arg1) + log(arg2) = log(arg1*arg2)
    result = addentExp(unaryExp(logFunction, (arg1 * arg2).reduceRealExp()), e1.isPositive());
  } else if(e1.isPositive()) {             // log(arg1) - log(arg2) = log(arg1/arg2)
    result = addentExp(unaryExp(logFunction, (arg1 / arg2).reduceRealExp()),true);
  } else {                                 // log(arg2) - log(arg1) = log(arg2/arg1)
    result = addentExp(unaryExp(logFunction, (arg2 / arg1).reduceRealExp()),true);
  }
  RETURNNODE(result);
}

/*
 * e1, e2  both type NT_ADDENT
 * return If(e1 == a*b and e2 == a*c)
 *          then return a*(b+c). Distributive low for * and +/-
 *        If(e1 == p1 * a^c1 and e2 == p2 * a^c2)
 *          then, assuming c1 < c2, return a^c1 * (p1 + p2 * a^(c2-c1))
 *          and symmetric if c1 > c2 (taking care of signs of e1 and e2 too)
 * return NULL If no common factors found
 */
SNode SNode::getCommonFactor(SNode e1, SNode e2) const {
  ENTERMETHOD2(e1, e2);
  CHECKNODETYPE(e1,NT_ADDENT);
  CHECKNODETYPE(e1,NT_ADDENT);
  FactorArray fl1(getTree()), fl2(getTree());
  e1.left().getFactors(fl1);
  e2.left().getFactors(fl2);

  FactorArray commonFactors(getTree());
  int signShiftCount = 0;

StartSearch:
  for(size_t i1 = 0; i1 < fl1.size(); i1++) {
    SNode factor1 = fl1[i1];
    for(size_t i2 = 0; i2 < fl2.size(); i2++) {
      SNode factor2 = fl2[i2];

      if(Expr::equal(factor1.node(), factor2.node())) {
        fl1.remove(i1);
        fl2.remove(i2);
        commonFactors *= factor1;
        goto StartSearch;
      } else if((factor1.base().equal(factor2.base()))
            &&  (factor1.exponent().isNumber() && factor2.exponent().isNumber())) { // exponents are different
        fl1.remove(i1);
        fl2.remove(i2);
        SNode         base        = factor1.base(); // == factor2->base()
        const Number &c1          = factor1.exponent().getNumber();
        const Number &c2          = factor2.exponent().getNumber();
        const Number *minExponent , *maxExponent;
        FactorArray  *flEMin      , *flEMax;

        if(c1 < c2) {
          minExponent  = &c1 ; maxExponent  = &c2;
          flEMin       = &fl1; flEMax       = &fl2;
        } else {
          minExponent  = &c2 ; maxExponent  = &c1;
          flEMin       = &fl2; flEMax       = &fl1;
        }
        commonFactors *= powerExp(base, SNV(*minExponent));
        *flEMax *= powerExp(base, SNV(*maxExponent - *minExponent));
        goto StartSearch;
      } else {
        Rational eR1,eR2;
        if((factor1.exponent().reducesToRationalConstant(&eR1) && factor2.exponent().reducesToRationalConstant(&eR2))
         && factor1.base().equalMinus(factor2.base())) {
          if(isAsymmetricExponent(eR1) && isAsymmetricExponent(eR2)) {
            signShiftCount++;
          } else if(isSymmetricExponent(eR1) && isSymmetricExponent(eR2)) {
            // do nothing
          } else {
            continue;
          }
          fl1.remove(i1);
          fl2.remove(i2);

          SNode           base = factor1.base(); // == -factor2->base()
          const Rational *minExponent , *maxExponent;
          FactorArray    *flEMin      , *flEMax;

          if(eR1 == eR2) {
            commonFactors *= factor1;
          } else {
            if(eR1 < eR2) {
              minExponent  = &eR1; maxExponent = &eR2;
              flEMin       = &fl1; flEMax      = &fl2;
            } else { // e1 > e2
              minExponent  = &eR2; maxExponent = &eR1;
              flEMin       = &fl2; flEMax      = &fl1;
            }
            commonFactors *= powerExp(base, *minExponent);
            *flEMax       *= powerExp(base, *maxExponent - *minExponent);
          }
          goto StartSearch;
        }
      }
    }
  }
  if(commonFactors.size() == 0) {
    RETURNNULL;
  }
  SNode a = productExp(commonFactors);

  bool positive = true;
  const bool e2positive = (e2.isPositive() == ::isEven(signShiftCount));

  SNode bc;
  if(e1.isPositive() == e2positive) {
    bc = productExp(fl1) + productExp(fl2);
    positive = e1.isPositive();
  } else if(e1.isPositive()) { // a2 negative
    bc = productExp(fl1) - productExp(fl2);
  } else { // e1 negative and e2 positive
    bc = productExp(fl2) - productExp(fl1);
  }
  SNode result = addentExp(a * bc, positive);
  RETURNNODE( result );
}

// Assume symbol = SUM. nested SUM-nodes will all be put on the same level, (in result), by recursive calls
bool SNode::getAddents(AddentArray &result, bool positive) const {
  ENTERMETHOD();
  CHECKNODETYPE(*this, NT_SUM);

  const AddentArray &a       = getAddentArray();
  const size_t       n       = a.size();
  bool               changed = false;
  for(size_t i = 0; i < n; i++) {
    SNode e = a[i], child = e.left();
    if(child.getSymbol() == SUM) {
      child.getAddents(result, e.isPositive() == positive);
      changed = true;
    } else if(positive) {
      result.add(e);
    } else {
      result.add(addentExp(child,!e.isPositive()));
      changed = true;
    }
  }
  RETURNBOOL( changed );
}

SNode SNode::changeFirstNegativeFactor() const {
  ENTERMETHOD();
  switch(getSymbol()) {
  case NUMBER :
    if(isNegativeNumber()) {
      RETURNNODE( SNV(-getNumber()) );
    }
    break;
  case PRODUCT:
    { const FactorArray &factorArray = getFactorArray();
      if(factorArray.size() == 0) RETURNNULL;
      FactorArray newFactorArray(getTree());
      SNode f0 = factorArray[0];
      if(f0.base().isNegativeNumber() && f0.exponent().isOdd()) {
        newFactorArray *= powerExp(-f0.base(), f0.exponent());
        for(size_t i = 1; i < factorArray.size(); i++) {
          newFactorArray *= factorArray[i];
        }
        RETURNNODE( productExp(newFactorArray) );
      }
    }
    break;
  case POW    :
    if(left().isNegativeNumber() && right().isOdd()) {
      RETURNNODE( pow(SNV(-left().getNumber()), right()) );
    }
    break;
  }
  RETURNNULL;
}

//------------------------------------ reduceProduct ----------------------------------------

// Symbol = PRODUCT
SNode SNode::reduceProduct() {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_PRODUCT);

  FactorArray unreducedFactors(getTree()), nonConstantFactors(getTree()), constantFactors(getTree());
  getFactors(unreducedFactors);

  for(size_t i = 0; i < unreducedFactors.size(); i++) { // first remove all number factors, multiplied together in constantFactor
    SNode f = unreducedFactors[i];
    if(!f.isConstant()) {
      nonConstantFactors *= f; // contains non-constant factors
    } else if(f.isReduced()) {
      constantFactors *= f;
    } else {
      Rational r;
      if(!f.reducesToRational(&r)) {
        constantFactors *= f;
      } else if(r == 0) {
        RETURNNODE( _0() );
      } else if(r != 1) { // No need to add 1 as a factor
        constantFactors *= r;
        constantFactors.last().setReduced();
      }
    }
  }

  // nonConstantFactors contains no constant factors
  FactorArray reduced = nonConstantFactors;

  if(reduced.size() > 1) {
    BitSet done(reduced.size());
    do {
      FactorArray tmp = reduced;
      reduced.clear();
      done.setCapacity(tmp.size());
      done.clear();
      for(size_t i1 = 1; i1 < tmp.size(); i1++) {
        SNode f1 = tmp[i1];
        for(size_t i2 = 0; i2 < i1; i2++) {
          if(done.contains(i1)) break;
          if(done.contains(i2)) continue;
          SNode f2 = tmp[i2];
          if(f1.base().equal(f2.base())) { // Common base
            SNode newExponent = (f1.exponent() + f2.exponent()).reduceRealExp();
            if(!newExponent.isZero()) {
              reduced *= powerExp(f1.base(), newExponent);
            }
            done.add(i1);
            done.add(i2);
          } else {
            SNode f = reduceTrigonometricFactors(f1, f2);
            if(!f.isEmpty()) {
              done.add(i1);
              done.add(i2);
              if(!f.isConstant()) {
                reduced *= f;
              } else {
                Rational r;
                if(!f.reducesToRational(&r)) {
                  constantFactors *= f;
                } else if(r == 0) {        // No need to go further. The product is 0
                  RETURNNODE( _0() );
                } else if(r != 1) {        // No need to add 1 as a factor
                  constantFactors *= r;
                }
              }
            }
          }
        }
      }
      for(size_t i = 0; i < tmp.size(); i++) {
        if(!done.contains(i)) {
          reduced *= tmp[i];
        }
      }
    } while(!done.isEmpty() && reduced.size() > 1);
  }

  SNode constantFactor = reduceConstantFactors(constantFactors);
  if(!constantFactor.isOne()) {
    reduced *= constantFactor;
  }
  reduced.sort();
  if(getFactorArray().isSameNodes(reduced)) {
    setReduced();
    RETURNTHIS;
  }
  RETURNNODE( productExp(reduced) );
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
        if(::abs(r.getNumerator()) == 1) {
          result *= powerExp(SNV(sign(r.getNumerator()) * r.getDenominator()), (-exponent).reduceRealExp());
          break;
        }
      }
      result *= powerExp(*this, exponent);
    }
    break;
  case PRODUCT:
    { FactorArray &a = getFactorArray();
      for(size_t i = 0; i < a.size(); i++) {
        a[i].getFactors(result, exponent);
      }
    }
    break;
  case POW :
    getFactorsInPower(result, exponent);
    break;
  default:
    result *= powerExp(reduceRealExp(), exponent);
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
  CHECKSYMBOL(*this,POW);

  FactorArray tmp1(getTree()), tmp2(getTree());
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
  CHECKSYMBOL(*this,POW);

  SNode base  = left();
  SNode expo  = right();

  SNode rBase = base.reduceRealExp();
  SNode rExpo = expo.reduceRealExp();
  SNode result;

  if(rBase.getSymbol() == POW) {
    result = pow(rBase.left(), multiplyExponents(rBase.right(), rExpo));
  } else if(rBase.isSameNode(base) && rExpo.isSameNode(expo)) { // nothing changed
    Rational r;
    if(rExpo.reducesToRationalConstant(&r)) {
      if((r == 1) && !rExpo.isOne()) {
        result = abs(rBase);
      } else if((r == -1) && !rExpo.isMinusOne()) {
        result = reciprocal(abs(rBase));
      } else {
        result = *this;
      }
    } else {
      result = *this;
    }
  } else {
    result = pow(rBase, rExpo);
  }
  Rational tmp;
  if(result.reducesToRationalConstant(&tmp)) {
    result = SNV(tmp);
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
    result *= factors;
    RETURNSHOWSTR( result );
  } else if(exponent.isZero()) {
    RETURNSHOWSTR( result ); // the empty list has value 1
  }

  Rational erat;
  if(exponent.reducesToRationalConstant(&erat)) {
    SNode er = SNode(getTree(), erat);
    for(size_t i = 0; i < factors.size(); i++) {
      SNode f = factors[i];
      result *= powerExp(f.base(), multiplyExponents(f.exponent(), er));
    }
  } else { // exponent is not rational. so no need to use multiplyExponents
    for(size_t i = 0; i < factors.size(); i++) {
      SNode f  = factors[i];
      result *= powerExp(f.base(), f.exponent() * exponent);
    }
  }
  RETURNSHOWSTR( result );
}

SNode SNode::multiplyExponents(SNode e1, SNode e2) const {
  return getTree().multiplyExponents(e1.node(), e2.node());
}

SNode SNode::divideExponents(SNode e1, SNode e2) const {
  return getTree().divideExponents(e1.node(), e2.node());
}

SNode SNode::multiplySumSum(SNode n1, SNode n2) const {
  ENTERMETHOD2(n1,n2);
  CHECKNODETYPE(n1,NT_SUM);
  CHECKNODETYPE(n2,NT_SUM);

  const AddentArray &aa1 = n1.getAddentArray();
  const AddentArray &aa2 = n2.getAddentArray();

  ParserTree &tree = n1.getTree();
  AddentArray newAddentArray(tree, aa1.size() * aa2.size());
  for(size_t i = 0; i < aa1.size(); i++) {
    SNode e1 = aa1[i], s1 = e1.left();
    for(size_t j = 0; j < aa2.size(); j++) {
      SNode e2 = aa2[j], s2 = e2.left();
      newAddentArray.add(addentExp(s1 * s2, e1.isPositive() == e2.isPositive()));
    }
  }
  SNode result = sumExp(newAddentArray);
  RETURNNODE( result );
}

SNode SNode::reduceTrigonometricFactors(SNode f1, SNode f2) {
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
    case COS: RETURNNODE( e1 == -e2 ? powerExp(tan(arg),f1.exponent()) : NULL );
    case TAN: RETURNNODE( e1 == -e2 ? powerExp(cos(arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case COS:
    switch(f2.base().getSymbol()) {
    case SIN: RETURNNODE( e1 == -e2 ? powerExp(tan(arg),f2.exponent()) : NULL );
    case TAN: RETURNNODE( e1 ==  e2 ? powerExp(tan(arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case TAN:
    switch(f2.base().getSymbol()) {
    case SIN: RETURNNODE( e1 == -e2 ? powerExp(cos(arg),f2.exponent()) : NULL );
    case COS: RETURNNODE( e1 ==  e2 ? powerExp(sin(arg),f1.exponent()) : NULL );
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
      SNode      f1                   = tmp[i1];
      SNode      base1                = f1.base();
      SNode      expo1                = f1.exponent();
      Rational   B1R, E1R;
      const bool b1IsRationalConstant = base1.reducesToRationalConstant(&B1R);
      const bool e1IsRationalConstant = expo1.reducesToRationalConstant(&E1R);
      SNode      reducedBase1         = b1IsRationalConstant ? SNV(B1R) : base1;

      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        SNode      f2    = tmp[i2];
        SNode      base2 = f2.base();
        SNode      expo2 = f2.exponent();
        Rational   B2R, E2R;
        const bool b2IsRationalConstant = base2.reducesToRationalConstant(&B2R);
        const bool e2IsRationalConstant = expo2.reducesToRationalConstant(&E2R);
        SNode      reducedBase2         = b2IsRationalConstant ? SNV(B2R) : base2;

        if(e1IsRationalConstant && e2IsRationalConstant) {
          if(E1R == E2R) {
            reduced *= powerExp(reducedBase1 * reducedBase2, E1R);
            done.add(i1);
            done.add(i2);
          } else if(E1R == -E2R) {
            if(E1R > 0) {
              reduced *= powerExp(reducedBase1 / reducedBase2, E1R);
            } else {
              reduced *= powerExp(reducedBase2 / reducedBase1, E2R);
            }
            done.add(i1);
            done.add(i2);
          } else if(base1.equal(base2)) {
            reduced *= powerExp(reducedBase1, E2R + E1R);
            done.add(i1);
            done.add(i2);
          }
        } else if(expo1.equal(expo2)) {
          reduced *= powerExp(reducedBase1 * reducedBase2, expo1);
          done.add(i1);
          done.add(i2);
        }
      }
    }
    reduced *= tmp.selectFactors(compl(done));
    if(reduced.size() == startSize) {
      break;
    }
  }

  Rational    rationalPart = 1;
  FactorArray fa(getTree());

  for(size_t i = 0; i < reduced.size(); i++) {
    SNode    f = reduced[i];
    Rational r;
    if(f.reducesToRationalConstant(&r)) {
      if(r == 0) {
        RETURNNODE( _0() );
      } else if(r == 1) {
        continue;
      } else {
        rationalPart *= r;
      }
    } else {
      SNode base = f.base();
      SNode expo = f.exponent();
      Rational baseR, expoR;
      if(base.reducesToRationalConstant(&baseR)) {
        if(expo.reducesToRationalConstant(&expoR)) {
          fa *= reduceRationalPower(baseR, expoR);
        } else { // base is rational. expo is irrational
          fa *= powerExp(SNV(baseR), expo);
        }
      } else {
        fa *= f;
      }
    }
  }

  if(rationalPart != 1) {
    const INT64 &num = rationalPart.getNumerator();
    const INT64 &den = rationalPart.getDenominator();
    fa *= num;
    fa /= den;
  }
  RETURNNODE( productExp(fa) );
}

SNode SNode::reduceRationalPower(const Rational &base, const Rational &exponent) {
  ENTERMETHOD2NUM(base, exponent);

  if(exponent.isInteger()) {
    RETURNNODE( SNV(pow(base, getInt(exponent))) );
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
    if((::abs(bn) == 1 || !bnRootFactors.isEmpty()) && (bd == 1 || !bdRootFactors.isEmpty())) {
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

    FactorArray fa(getTree());
    if(niceRootFactor != 1) {
      fa *= powerExp(SNV(niceRootFactor), exponent.getNumerator());
    }

    const Rational nonRootFactor = pow(Rational(bnPrimeFactors.getProduct(), bdPrimeFactors.getProduct()), (int)exponent.getNumerator());
    if(nonRootFactor != 1) {
      if(nonRootFactor.getNumerator() == 1) {
        fa *= powerExp(SNV(nonRootFactor.getDenominator()), Rational(-1,ed));
      } else {
        fa *= powerExp(SNV(nonRootFactor), Rational(1,ed));
      }
    }
    RETURNNODE( productExp(fa) );
  }
}

SNode SNode::reduceModulus() const {
  ENTERMETHOD();
  CHECKSYMBOL(*this,MOD);
  if(isReduced()) RETURNTHIS;

  SNode l  = left();
  SNode r  = right();

  SNode Rl = l.reduceRealExp();
  SNode Rr = r.reduceRealExp();
  if(Rr.isNegativeNumber() || Rr.isUnaryMinus()) Rr = -Rr;

  if(Rl.isSameNode(l) && Rr.isSameNode(r)) {
    setReduced();
    RETURNTHIS;
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
  CHECKSYMBOL(*this,LN);
  if(isReduced()) RETURNTHIS;

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOfE();
  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) { // ln(a^b) = b * ln(a)
    RETURNNODE( Rarg.right() * ln( Rarg.left()) );
  }
  if(Rarg.isSameNode(arg)) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE(ln( Rarg) );
  }
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
  CHECKSYMBOL(*this,LOG10);
  if(isReduced()) RETURNTHIS;

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOf10();

  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) {
    RETURNNODE( Rarg.right() * log10( Rarg.left()) );
  }
  if( Rarg.isSameNode(arg) ) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( log10( Rarg) );
  }
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
  CHECKSYMBOL(*this,LOG2);
  if(isReduced()) RETURNTHIS;

  SNode arg  = left();
  SNode Rarg = arg.reduceRealExp();
  SNode p    = Rarg.getPowerOf2();

  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) {
    RETURNNODE( Rarg.right() * log2( Rarg.left()) );
  }
  if(Rarg.isSameNode(arg)) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( log2( Rarg) );
  }
}

SNode SNode::getPowerOf2() {
  ENTERMETHOD();

  if(isTwo()) {
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
  CHECKNODETYPE(*this,NT_TREE);
  if(isReduced()) RETURNTHIS;

  SNode arg  = left();
  if(getInverseFunction() == arg.getSymbol()) {
    RETURNNODE( arg.left() );
  }
  const SNode Rarg = arg.reduceRealExp();
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = -f(exp)
    RETURNNODE( -unaryExp(getSymbol(), Rarg.left()) );
  } else if( Rarg.isSameNode(arg) ) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( unaryExp(getSymbol(), Rarg) );
  }
}

SNode SNode::reduceSymmetricFunction() {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_TREE);
  if(isReduced()) RETURNTHIS;

  SNode arg  = left();
  if(getInverseFunction() == arg.getSymbol()) {
    RETURNNODE( arg.left() );
  }
  const SNode Rarg = arg.reduceRealExp();
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = f(exp)
    RETURNNODE( unaryExp(getSymbol(), Rarg.left()) );
  } else if( Rarg.isSameNode(arg) ) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( unaryExp(getSymbol(), Rarg) );
  }
}

/*
 * symbol = POLY
 * return no leading zeroes, all constant coefficients evaluated
 */
SNode SNode::reducePoly() {
  ENTERMETHOD();
  CHECKSYMBOL(*this,POLY);
  if(isReduced()) RETURNTHIS;

  SNodeArray &coefArray = getCoefArray();
  SNode       arg       = getArgument();  // arg is the parameter to the polynomial ex. poly[a,b,c,d](arg)

  SNodeArray newCoefArray(coefArray.getTree());
  bool leadingCoef = true;
  for(size_t i = 0; i < coefArray.size(); i++) {
    SNode coef = coefArray[i];
    if(!coef.isConstant()) {     // coef not constant
      newCoefArray.add(coef.reduceRealExp());
    } else if(coef.isNumber()) { // coef is constant
      if(coef.isZero() && leadingCoef) continue;
      newCoefArray.add(coef);
    } else {
      Rational r;
      if(coef.reducesToRationalConstant(&r)) {
        if(r.isZero() && leadingCoef) continue;
        newCoefArray.add(SNV(r));
      } else {
        const Real c = coef.evaluateReal();
        if(c == 0 && leadingCoef) continue;
        newCoefArray.add(SNV(c));
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
        setReduced();
        RETURNTHIS;
      } else {
        RETURNNODE( polyExp(newCoefArray, newArg) );
      }
    }
  }
}

SNode SNode::reduceCondExp() {
  ENTERMETHOD();
  CHECKSYMBOL(*this, IIF);
  if(isReduced()) RETURNTHIS;

  SNode cond      = child(0);
  SNode trueExp   = child(1);
  SNode falseExp  = child(2);
  SNode RtrueExp  = trueExp.reduceRealExp();
  SNode RfalseExp = falseExp.reduceRealExp();
  if(RtrueExp.equal(RfalseExp)) {
    RETURNNODE( RtrueExp );
  }
  SNode Rcond = cond.reduceBoolExp();
  if(Rcond.isTrue() ) RETURNNODE( RtrueExp  );
  if(Rcond.isFalse()) RETURNNODE( RfalseExp );

  if(Rcond.isSameNode(cond) && RtrueExp.isSameNode(trueExp) && RfalseExp.isSameNode(falseExp)) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( condExp(Rcond, RtrueExp, RfalseExp ) );
  }
}

SNode SNode::reduceTreeNode() {
  ENTERMETHOD();
  CHECKNODETYPE(*this, NT_TREE);
  if(isReduced()) RETURNTHIS;

  if(getInverseFunction() == left().getSymbol()) {
    RETURNNODE( left().left() );
  }

  SNodeArray  &a  = getChildArray();
  const size_t sz = a.size();
  SNodeArray newChildArray(a.getTree(),sz);
  for(size_t i = 0; i < sz; i++) {
    newChildArray.add(a[i].reduceRealExp());
  }
  if(newChildArray.isSameNodes(a)) {
    setReduced();
    RETURNTHIS;
  } else {
    RETURNNODE( treeExp(getSymbol(), newChildArray) );
  }
}

}; // namespace Expr
