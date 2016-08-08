#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/PrimeFactors.h>

class MainReducer : public ExpressionTransformer {
private:
  Expression     &m_expr;
  ExpressionState m_state;
public:
  MainReducer(Expression *expr) : m_expr(*expr) {
    m_state = EXPR_MAINREDUCTION1;
  }
  SNode transform(SNode n) {
    return m_expr.reduce(n);
  }
  ExpressionState getState() const {
    return m_state;
  }
  void setState(ExpressionState state) {
    m_state = state;
  }
};

/*
class RationalPowersReducer : public ExpressionTransformer {
private:
  Expression *m_expr;
public:
  RationalPowersReducer(Expression *expr) : m_expr(expr) {
  }
  const ExpressionNode *transform(const ExpressionNode *n) {
    return m_expr->replaceRationalPowers(n);
  }
  ExpressionState getState() const {
    return EXPR_RATIONALPOWERSREDUCTION;
  }
};
*/

void Expression::reduce() {

  try {
    const Array<ExpressionVariableWithValue> variables = getAllVariables();

    const ParserTreeForm startTreeForm = getTreeForm();
    if(startTreeForm != TREEFORM_CANONICAL) {
      toCanonicalForm();
      setState(EXPR_CANONICALFORM);
    }
    checkIsCanonicalForm();

    MainReducer mainTransformer(this);

    iterateTransformation(mainTransformer);

    mainTransformer.setState(EXPR_MAINREDUCTION2);
    setRoot(multiplyParentheses(getRoot()));
    iterateTransformation(mainTransformer);

  /*
    tmp = replaceRationalFactors(iterateTransformation(getRoot(), RationalPowersReducer(this)));
    if(!treesEqual(getRoot(), tmp, false)) {
      setRoot(tmp);
      checkIsCanonicalForm();
    }
  */

    if(startTreeForm == TREEFORM_STANDARD) {
      toStandardForm();
      setState(EXPR_STANDARDFORM);
    }

    buildSymbolTable();
    pruneUnusedNodes();

    setState(EXPR_REDUCTIONDONE);
  } catch(...) {
    clear();
    throw;
  }
}

void Expression::iterateTransformation(ExpressionTransformer &transformer) {
  DEFINEMETHODNAME;

  const int                  maxIterations = 30;
  SNode                      oldRoot       = getRoot();
  Expression                 bestReduction = *this;
  const ParserTreeComplexity startComplexity(bestReduction);
  ParserTreeComplexity       bestComplexity(startComplexity);
  STARTREDUCTION();

  setReduceIteration(0);
  setRoot(oldRoot);
  pruneUnusedNodes();

  setState(transformer.getState());
  checkIsCanonicalForm();

  SNode n    = getRoot();
  bool  done = false;

  for(setReduceIteration(1); getReduceIteration() < maxIterations; setReduceIteration(getReduceIteration()+1)) {
    SNode n1 = transformer.transform(n);

    setRoot(n1);
    setState(transformer.getState());
    checkIsCanonicalForm();

    if(n1 == n) {
      done = true;
      break;
    } else {
      const ParserTreeComplexity complexity(*this);
      if(complexity < bestComplexity) {
        bestReduction  = *this;
        bestComplexity = complexity;
      }
      n = n1;
    }
  }
  setRoot(oldRoot);
  if(!done) {
    throwMethodException(s_className, method, _T("Maxiterations reached for expression <%s>"), n.toString().cstr());
  }

  pruneUnusedNodes();

  if(bestComplexity < startComplexity) {
    *this = bestReduction;
  }
}

SNode Expression::reduce(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SStmtList stmtList(n);
  SStmtList       newStmtList;

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    const SNode &stmt = stmtList[i];
    newStmtList.add(assignStmt(stmt.left(), reduceRealExp(stmt.right())));
  }
  SNode last = stmtList.last();
  switch(last.getSymbol()) {
  case RETURNREAL:
    newStmtList.add(unaryExp(RETURNREAL, reduceRealExp(last.left())));
    break;
  case RETURNBOOL:
    newStmtList.add(unaryExp(RETURNBOOL, reduceBoolExp(last.left())));
    break;
  default:
    throwUnknownSymbolException(method, last);
  }
  RETURNNODE( newStmtList.removeUnusedAssignments() );
}

SNode Expression::reduceBoolExp(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();
  SNode result;

  if(n.isConstant()) {
    RETURNNODE(booleanExpression(evaluateBoolExpr(n)));
  } else {
    switch(n.getSymbol()) {
    case NOT   : RETURNNODE( !reduceBoolExp(n.left()) );
    case AND   : RETURNNODE( reduceBoolExp(n.left()) && reduceBoolExp(n.right()) );
    case OR    : RETURNNODE( reduceBoolExp(n.left()) || reduceBoolExp(n.right()) );

    case EQ    :
    case NE    :
    case LE    :
    case LT    :
    case GE    :
    case GT    : RETURNNODE( binExp(n.getSymbol(), reduceRealExp(n.left()), reduceRealExp(n.right())) );
    default    :
      throwUnknownSymbolException(method, n);
      RETURNNULL;
    }
  }
}

SNode Expression::reduceRealExp(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  switch(n.getSymbol()) {
  case NUMBER         :
  case NAME           : RETURNNODE( n );
  case MINUS          : RETURNNODE( -reduceRealExp(n.left()) );
  case SUM            : RETURNNODE( reduceSum(    n) );
  case PRODUCT        : RETURNNODE( reduceProduct(n) );
  case POW            : RETURNNODE( reducePower(  n) );
  case MOD            : RETURNNODE( reduceModulus(n) );

  case LN             : RETURNNODE( reduceLn(     n) );
  case LOG10          : RETURNNODE( reduceLog10(  n) );

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
  case TANH           : RETURNNODE( reduceAsymmetricFunction(n) );

  case ABS            :
  case COS            :
  case COSH           :
  case GAUSS          : RETURNNODE( reduceSymmetricFunction(n) );

  case POLY           : RETURNNODE( reducePolynomial(n) );

  case ACOS           :
  case ACOT           :
  case ACOSH          :
  case BINOMIAL       :
  case CEIL           :
  case FAC            :
  case FLOOR          :
  case GAMMA          :
  case MAX            :
  case MIN            :
  case NORM           :
  case PROBIT         : RETURNNODE( reduceTreeNode(n) );

  case INDEXEDPRODUCT :
    RETURNNODE( indexProd(assignStmt(n.child(0).left(), reduceRealExp(n.child(0).right())) // startAssignment
                         ,reduceRealExp(n.child(1))                                        // endExpr
                         ,reduceRealExp(n.child(2))                                        // expr
                         )
                );
  case INDEXEDSUM     :
    RETURNNODE( indexSum(assignStmt(n.child(0).left(), reduceRealExp(n.child(0).right()))  // startAssignment
                        ,reduceRealExp(n.child(1))                                         // endExpr
                        ,reduceRealExp(n.child(2))                                         // expr
                        )
              );

  case IIF            : RETURNNODE( condExp(reduceBoolExp(n.child(0)), reduceRealExp(n.child(1)), reduceRealExp(n.child(2))) );

  default             :
    throwUnknownSymbolException(method, n);
    RETURNNULL;
  }
}

//------------------------------------ reduceSum ----------------------------------------

static ExpressionNodeSelector *getTrigonometricFunctionSelector() {
  static bool                         initDone = false;
  static ExpressionSymbolSet          functionSet;
  static ExpressionNodeSymbolSelector selector(&functionSet);
  if(!initDone) {
    functionSet.add(SIN);
    functionSet.add(COS);
    initDone = true;
  }
  return &selector;
}

static ExpressionNodeSelector *getLogarithmicFunctionSelector() {
  static bool                         initDone = false;
  static ExpressionSymbolSet          functionSet;
  static ExpressionNodeSymbolSelector selector(&functionSet);
  if(!initDone) {
    functionSet.add(LN);
    functionSet.add(LOG10);
    initDone = true;
  }
  return &selector;
}

// n.symbol == SUM
SNode Expression::reduceSum(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  bool anyChanges = false;
  bool hasTrigonometricFunctions = false, hasLogarithmicFunctions = false;
  const AddentArray &a = n.getAddentArray();

  AddentArray reduced;
  for(size_t i = 0; i < a.size(); i++) {
    SumElement     *e = a[i];
    ExpressionNode *reducedNode = reduceRealExp(e->getNode());
    if(reducedNode != e->getNode()) anyChanges = true;
    if(!hasTrigonometricFunctions) hasTrigonometricFunctions = reducedNode->getNodeCount(getTrigonometricFunctionSelector()) > 0;
    if(!hasLogarithmicFunctions  ) hasLogarithmicFunctions   = reducedNode->getNodeCount(getLogarithmicFunctionSelector())   > 0;
    reduced.add(reducedNode, e->isPositive());
  }

  if(reduced.size() <= 1) {
    RETURNNODE( addentsToNode(reduced) );
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
        if(treesEqual(e1->getNode(), e2->getNode())) {
          done.add(i1);
          done.add(i2);
          int factor = (e1->isPositive() == e2->isPositive()) ? 2 : 0;
          for(size_t i3 = i2+1; i3 < tmp.size(); i3++) { // check if there are more copies
            if(done.contains(i3)) continue;
            SumElement *e3 = tmp[i3];
            if(treesEqual(e1->getNode(), e3->getNode())) {
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
            reduced.add(productC(getTwo(), e1->getNode()),  (factor>0) == e1->isPositive());
            break;
          default:
            if(factor < 0) {
              reduced.add(productC(numberExpression(-factor), e1->getNode()), !e1->isPositive());
            } else {
              reduced.add(productC(numberExpression( factor), e1->getNode()),  e1->isPositive());
            }
          }
        } else if(hasTrigonometricFunctions && canUseIdiotRule(e1->getNode(), e2->getNode()) && e1->isPositive() == e2->isPositive()) {
          reduced.add(getOne(), e1->isPositive());
          done.add(i1);
          done.add(i2);
        } else if(hasTrigonometricFunctions && canUseReverseIdiotRule(e1, e2, sqrSinOrCos)) {
          reduced.add(sqrSinOrCos);
          done.add(i1);
          done.add(i2);
        } else if(hasLogarithmicFunctions && sameLogarithm(e1->getNode(), e2->getNode())) {
          reduced.add(mergeLogarithms(*e1, *e2));
          done.add(i1);
          done.add(i2);
        } else {
          SumElement *cf = (getState() == EXPR_MAINREDUCTION1) ? getCommonFactor(*e1, *e2) : NULL;
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
    ExpressionNode *n1 = changeFirstNegativeFactor(n);
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
    if(!reducesToRationalConstant(a, &r)) {
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

  if(!anyChanges) RETURNNODE( n );

  reduced.add(numberExpression(constantElements), true);

  RETURNNODE( addentsToNode(reduced) );
}
 
/*
 * n1,n2 expression in a sum
 * return true if n1 = cos^2(expression) and n2 = sin^2(expression) or vice versa
 */
bool Expression::canUseIdiotRule(SNode n1, SNode n2) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(n1, n2);

  if(n1.getSymbol() == POW && n2.getSymbol() == POW) {
    if(n1.right().isConstant() && n2.right().isConstant()) {
      const Real e1 = evaluateRealExpr(n1.right());
      const Real e2 = evaluateRealExpr(n2.right());
      if((e1 == 2) && (e2 == 2)) {
        const ExpressionInputSymbol f1 = n1.left().getSymbol();
        const ExpressionInputSymbol f2 = n2.left().getSymbol();
        RETURNBOOL ( (f1 == SIN && f2 == COS) || (f1 == COS && f2 == SIN) && (n1.left().left() == n2.left().left()) );
      }
    }
  }
  RETURNBOOL( false );
}

static ExpressionInputSymbol getDualTrigonometricFunction(ExpressionInputSymbol symbol) {
  switch(symbol) {
  case SIN: return COS;
  case COS: return SIN;
  default : throwInvalidArgumentException(_T("getDualTrigo"), _T("symbol=%s"), ExpressionTables->getSymbolName(symbol));
            return EOI;
  }
}

static bool isSinOrCos(SNode n) {
  DEFINEMETHODNAME;

  switch(n.getSymbol()) {
  case SIN:
  case COS:
    return true;
  default:
    return false;
  }
}

bool Expression::isSquareOfSinOrCos(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  switch(n.getSymbol()) {
  case POW:
    RETURNBOOL( isSinOrCos(n.left()) && n.right().isConstant() && (evaluateRealExpr(n.right()) == 2) );
  default :
    RETURNBOOL( false );
  }
}

bool Expression::canUseReverseIdiotRule(SumElement *e1, SumElement *e2, SumElement* &result) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(*e1, *e2);

  for(int i = 0; i < 2; i++) {
    Rational r1;
    if(reducesToRationalConstant(e1->getNode(), &r1)) {
      if(!e1->isPositive()) r1 = -r1;
      if((fabs(r1) == 1) && isSquareOfSinOrCos(e2->getNode()) && (r1 == 1) != (e2->isPositive())) {
        ExpressionNode *sinOrCos = e2->getNode()->left();
        ExpressionNode *n = powerC(functionExpression(getDualTrigonometricFunction(sinOrCos->getSymbol()), sinOrCos->left()), getTwo());
        result = new SumElement(n, r1 == 1);
        RETURNBOOL( true );
      }
    }
    ::swap(e1, e2);
  }
  RETURNBOOL( false );
}

bool Expression::sameLogarithm(SNode n1, SNode n2) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(n1, n2);

  RETURNBOOL( ((n1.getSymbol() == LN) && (n2.getSymbol() == LN)) || ((n2.getSymbol() == LOG10) && (n2.getSymbol() == LOG10)) );
}

/*
 * e1 assumes e1->getNode.getSymbol = LN or LOG10
 * e2 assumes e2->getNode.getSymbol = e1.getNode.getSymbol
 * return  log(a*b) =  log(a) + log(b),
 *        -log(a*b) = -log(a) - log(b),
 *         log(a/b) =  log(a) - log(b)
 *         log(b/a) = -log(a) + log(b) where log = LN or LOG10
 */
SumElement *Expression::mergeLogarithms(SumElement &e1, SumElement &e2) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(e1, e2);

  const SNode arg1        = e1.getNode()->left();
  const SNode arg2        = e2.getNode()->left();
  const ExpressionInputSymbol logFunction = e1.getNode()->getSymbol();

  if(e1.isPositive() == e2.isPositive()) { // log(arg1) + log(arg2) = log(arg1*arg2)
    RETURN( new SumElement(functionExpression(logFunction, reduceRealExp(arg1 * arg2)), e1.isPositive()));
  } else if(e1.isPositive()) {             // log(arg1) - log(arg2) = log(arg1/arg2)
    RETURN( new SumElement(functionExpression(logFunction, reduceRealExp(arg1 / arg2)),true) );
  } else {                                 // log(arg2) - log(arg1) = log(arg2/arg1)
    RETURN( new SumElement(functionExpression(logFunction, reduceRealExp(arg2 / arg1)),true) );
  }
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
SumElement *Expression::getCommonFactor(SumElement &e1, SumElement &e2) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(e1, e2);

  FactorArray fl1, fl2;
  getFactors(fl1, e1.getNode());
  getFactors(fl2, e2.getNode());

  FactorArray commonFactors;
  int signShiftCount = 0;

StartSearch:
  for(size_t i1 = 0; i1 < fl1.size(); i1++) {
    ExpressionFactor *factor1 = fl1[i1];
    for(size_t i2 = 0; i2 < fl2.size(); i2++) {
      ExpressionFactor *factor2 = fl2[i2];

      if(treesEqual(factor1, factor2)) {
        fl1.remove(i1);
        fl2.remove(i2);
        commonFactors.add(factor1);
        goto StartSearch;
      } else if((treesEqual(factor1->base(), factor2->base()))
            &&  (factor1->exponent()->isNumber() && factor2->exponent()->isNumber())) { // exponents are different
        fl1.remove(i1);
        fl2.remove(i2);
        ExpressionNode *base        = factor1->base(); // == factor2->base()
        const Number   &c1          = factor1->exponent()->getNumber();
        const Number   &c2          = factor2->exponent()->getNumber();
        const Number   *minExponent , *maxExponent;
        FactorArray    *flEMin      , *flEMax;

        if(c1 < c2) {
          minExponent  = &c1 ; maxExponent  = &c2;
          flEMin       = &fl1; flEMax       = &fl2;
        } else {
          minExponent  = &c2 ; maxExponent  = &c1;
          flEMin       = &fl2; flEMax       = &fl1;
        }
        commonFactors.add(base, numberExpression(*minExponent));
        flEMax->add(base, numberExpression(*maxExponent - *minExponent));
        goto StartSearch;
      } else {
        Rational eR1,eR2;
        if((reducesToRationalConstant(factor1->exponent(), &eR1) && reducesToRationalConstant(factor2->exponent(), &eR2))
         && treesEqualMinus(factor1->base(), factor2->base())) {

          if(isAsymmetricExponent(eR1) && isAsymmetricExponent(eR2)) {
            signShiftCount++;
          } else if(isSymmetricExponent(eR1) && isSymmetricExponent(eR2)) {
            // do nothing
          } else {
            continue;
          }
          fl1.remove(i1);
          fl2.remove(i2);

          ExpressionNode *base        = factor1->base(); // == -factor2->base()
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
            commonFactors.add(base, numberExpression(*minExponent));
            flEMax->add(base, numberExpression(*maxExponent - *minExponent));
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
  const SNode a = factorsToNode(commonFactors);

  bool positive = true;
  const bool e2positive = (e2.isPositive() == isEven(signShiftCount));

  if(e1.isPositive() == e2positive) {
    bc = factorsToNode(fl1) + factorsToNode(fl2);
    positive = e1.isPositive();
  } else if(e1.isPositive()) { // a2 negative
    bc = factorsToNode(fl1) - factorsToNode(fl2);
  } else { // e1 negative and e2 positive
    bc = factorsToNode(fl2) - factorsToNode(fl1);
  }
  SumElement *result = new SumElement(a * bc, positive);
  RETURNPSHOWSTR( result );
}

SNode Expression::changeFirstNegativeFactor(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();
  switch(n.getSymbol()) {
  case NUMBER :
    if(n.isNegative()) {
      RETURNNODE( numberExpression(-n.getNumber()) );
    }
    break;
  case PRODUCT:
    { const FactorArray &factorArray = n.getFactorArray();
      if(factorArray.size() == 0) RETURNNULL;
      FactorArray newFactorArray;
      ExpressionFactor *f0 = factorArray[0];
      if(f0->base()->isNegative() && f0->exponent()->isOdd()) {
        newFactorArray.add(minusC(f0->base()), f0->exponent());
        for(size_t i = 1; i < factorArray.size(); i++) newFactorArray.add(factorArray[i]);
        RETURNNODE( factorsToNode(newFactorArray) );
      }
    }
    break;
  case POW    :
    if(n.left().isNegative() && n.right().isOdd()) {
      RETURNNODE( powerC(numberExpression(-n.left().getNumber()), n.right()) );
    }
    break;
  }
  RETURNNULL;
}

SNode Expression::factorsToNode(FactorArray &a) {
  return getProduct(a);
}

SNode Expression::addentsToNode(AddentArray &a) {
  return getSum(a);
}

//------------------------------------ reduceProduct ----------------------------------------

SNode Expression::reduceProduct(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  FactorArray unreducedFactors, nonConstantFactors, constantFactors;
  getFactors(unreducedFactors, n);

  for(size_t i = 0; i < unreducedFactors.size(); i++) { // first remove all number factors, multiplied together in constantFactor
    ExpressionFactor *f = unreducedFactors[i];
    if(!f->isConstant()) {
      nonConstantFactors.add(f); // contains non-constant factors
    } else {
      Rational r;
      if(!reducesToRational(f, &r)) {
        constantFactors.add(f);
      } else if(r == 0) {
        RETURNNODE( getZero() );
      } else if(r != 1) { // No need to add 1 as a factor
        constantFactors.add(numberExpression(r));
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
          if(treesEqual(f1->base(),f2->base())) { // Common base
            ExpressionNode *newExponent = reduceRealExp(sumC(f1->exponent(), f2->exponent()));
            if(!newExponent->isZero()) {
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
                if(!reducesToRational(f, &r)) {
                  constantFactors.add(f);
                } else if(r == 0) {        // No need to go further. The product is 0
                  RETURNNODE( getZero() );
                } else if(r != 1) {        // No need to add 1 as a factor
                  constantFactors.add(numberExpression(r));
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

  ExpressionNode *constantFactor = reduceConstantFactors(constantFactors);
  if(!constantFactor->isOne()) {
    reduced.add(constantFactor);
  }
  const SNode result = factorsToNode(reduced);
  RETURNNODE( result );
}

FactorArray &Expression::getFactors(FactorArray &result, SNode n) {
  return getFactors(result, n, _1());
}

FactorArray &Expression::getFactors(FactorArray &result, SNode n, SNode exponent) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(n, exponent);

  switch(n.getSymbol()) {
  case NUMBER:
    if(!n.isOne()) { // 1 should not be added
      const Number &v = n.getNumber();
      if(v.isRational()) {
        const Rational r = v.getRationalValue();
        if(abs(r.getNumerator()) == 1) {
          result.add(numberExpression(sign(r.getNumerator()) * r.getDenominator()), reduceRealExp(-exponent));
          break;
        }
      }
      result.add(n, exponent);
    }
    break;
  case PRODUCT:
    { const FactorArray &a = n.getFactorArray();
      for(size_t i = 0; i < a.size(); i++) getFactors(result, a[i], exponent);
    }
    break;
  case POW :
    getFactorsInPower(result, n, exponent);
    break;
  default:
    result.add(reduceRealExp(n), exponent);
    break;
  }
  RETURNSHOWSTR( result );
}

/*
 * n.symbol = POW
 * exponent is the outer exponent of n. value = pow(pow(n.left,n.right),exponent)
 * return n split into as many separate factors as possible
 */
FactorArray &Expression::getFactorsInPower(FactorArray &result, SNode n, SNode exponent) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(n, exponent);
  FactorArray tmp1, tmp2;
  const SNode &base = n.left();
  switch(base.getSymbol()) {
  case POW :
    multiplyExponents(tmp2, getFactors(tmp1, reduceRealExp(base.left()), reduceRealExp(base.right())), reduceRealExp(n.right()));
    break;
  default                   :
    getFactors(tmp2, reduceRealExp(base), reduceRealExp(n.right()));
    break;
  }
  multiplyExponents(result, tmp2, exponent);
  RETURNSHOWSTR( result );
}

// n.symbol = POW
SNode Expression::reducePower(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode base  = n.left();
  const SNode expo  = n.right();

  const SNode rBase = reduceRealExp(base);
  const SNode rExpo = reduceRealExp(expo);
  SNode result;

  if(rBase.getSymbol() == POW) {
    result = pow(rBase.left(), rBase.right() * rExpo);
  } else if(rBase.isSameNode(base) && rExpo.isSameNode(expo)) { // nothing changed
    result = n;
  } else {
    result = pow(rBase, rExpo);
  }
  Rational tmp;
  if(reducesToRationalConstant(result, &tmp)) {
    result = numberExpression(tmp);
  }
  RETURNNODE( result );
}

/* factors list of ExpressionFactor
 * factor factor to multiply with
 * return list of expressionFactors, result[i] = fetchFactorNode(factors[i].base,factors[i].exponent * factor)
 */
FactorArray &Expression::multiplyExponents(FactorArray &result, FactorArray &factors, SNode exponent) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(factors, exponent);

  if(exponent.isOne()) {
    result.addAll(factors);
    RETURNSHOWSTR( result );
  } else if(exponent.isZero()) {
    RETURNSHOWSTR( result ); // the empty list has value 1
  }

  Rational   exponentR                  = 1;
  const bool exponentIsRationalConstant = reducesToRationalConstant(exponent, &exponentR);

  for(size_t i = 0; i < factors.size(); i++) {
    ExpressionFactor     *f  = factors[i];
    const SNode           e  = f->exponent();
    Rational              eR = 1;
    const bool            eIsRationalConstant = reducesToRationalConstant(e, &eR);
    if(exponentIsRationalConstant && eIsRationalConstant) {
      if(rationalExponentsMultiply(eR, exponentR)) {
        result.add(f->base(), numberExpression(eR * exponentR));
      } else {
        const Rational newE = eR * exponentR;
        result.add(f->base(), quotientC(numberExpression(newE.getNumerator()*2), numberExpression(newE.getDenominator()*2)));
      }
    } else {
      result.add(f, exponent);
    }
  }
  RETURNSHOWSTR( result );
}

bool Expression::rationalExponentsMultiply(const Rational &r1, const Rational &r2) { // static
  return isAsymmetricExponent(r1) || isAsymmetricExponent(r2);
}

ExpressionFactor *Expression::reduceTrigonometricFactors(ExpressionFactor &f1, ExpressionFactor &f2) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(f1,f2);

  if(!f1.base()->isTrigonomtricFunction() || !f2.base()->isTrigonomtricFunction()) {
    RETURNNULL;
  }
  ExpressionNode *arg = f1.base()->left();
  if(!treesEqual(arg,f2.base()->left())) {
    RETURNNULL;
  }

  if(!f1.exponent()->isNumber() || !f2.exponent()->isNumber()) {
    RETURNNULL;
  }
  const Real e1 = f1.exponent()->getReal();
  const Real e2 = f2.exponent()->getReal();

  switch(f1.base()->getSymbol()) {
  case SIN:
    switch(f2.base()->getSymbol()) {
    case COS: RETURN( e1 == -e2 ? fetchFactorNode(functionExpression(TAN,arg),f1.exponent()) : NULL );
    case TAN: RETURN( e1 == -e2 ? fetchFactorNode(functionExpression(COS,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case COS:
    switch(f2.base()->getSymbol()) {
    case SIN: RETURN( e1 == -e2 ? fetchFactorNode(functionExpression(TAN,arg),f2.exponent()) : NULL );
    case TAN: RETURN( e1 ==  e2 ? fetchFactorNode(functionExpression(SIN,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  case TAN:
    switch(f2.base()->getSymbol()) {
    case SIN: RETURN( e1 == -e2 ? fetchFactorNode(functionExpression(COS,arg),f2.exponent()) : NULL );
    case COS: RETURN( e1 ==  e2 ? fetchFactorNode(functionExpression(SIN,arg),f1.exponent()) : NULL );
    }
    RETURNNULL;
  }
  RETURNNULL;
}

SNode Expression::reduceConstantFactors(FactorArray &factorArray) {
  DEFINEMETHODNAME;
  ENTERMETHOD1(factorArray);

  if(factorArray.size() == 0) {
    RETURNNODE( getOne() );
  }

  FactorArray reduced = factorArray;
  for(int startSize = (int)reduced.size(); startSize > 0; startSize = (int)reduced.size()) {
    const FactorArray tmp = reduced;
    reduced.clear();
    BitSet done(tmp.size());
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      if(done.contains(i1)) continue;
      ExpressionFactor       *f1                   = tmp[i1];
      ExpressionNode         *base1                = f1->base();
      ExpressionNode         *expo1                = f1->exponent();
      Rational                B1R, E1R;
      const bool              b1IsRationalConstant = reducesToRationalConstant(base1, &B1R);
      const bool              e1IsRationalConstant = reducesToRationalConstant(expo1, &E1R);
      ExpressionNode         *reducedBase1         = b1IsRationalConstant ? numberExpression(B1R) : base1;

      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        ExpressionFactor *f2                   = tmp[i2];
        ExpressionNode   *base2                = f2->base();
        ExpressionNode   *expo2                = f2->exponent();
        Rational          B2R, E2R;
        const bool        b2IsRationalConstant = reducesToRationalConstant(base2, &B2R);
        const bool        e2IsRationalConstant = reducesToRationalConstant(expo2, &E2R);
        ExpressionNode   *reducedBase2         = b2IsRationalConstant ? numberExpression(B2R) : base2;

        if(e1IsRationalConstant && e2IsRationalConstant) {
          if(E1R == E2R) {
            reduced.add(productC(reducedBase1, reducedBase2), numberExpression(E1R));
            done.add(i1);
            done.add(i2);
          } else if(E1R == -E2R) {
            if(E1R > 0) {
              reduced.add(quotientC(reducedBase1, reducedBase2), numberExpression(E1R));
            } else {
              reduced.add(quotientC(reducedBase2, reducedBase1), numberExpression(E2R));
            }
            done.add(i1);
            done.add(i2);
          } else if(treesEqual(base1, base2)) {
            reduced.add(reducedBase1, numberExpression(E2R + E1R));
            done.add(i1);
            done.add(i2);
          }
        } else if(treesEqual(expo1, expo2)) {
          reduced.add(productC(reducedBase1, reducedBase2), expo1);
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
    if(reducesToRationalConstant(f, &r)) {
      if(r == 0) {
        RETURNNODE( getZero() );
      } else if(r == 1) {
        continue;
      } else {
        rationalPart *= r;
      }
    } else {
      ExpressionNode *base = f->base();
      ExpressionNode *expo = f->exponent();
      Rational baseR, expoR;
      if(reducesToRationalConstant(base, &baseR)) {
        if(reducesToRationalConstant(expo, &expoR)) {
          fa.add(reduceRationalPower(baseR, expoR));
        } else { // base is rational. expo is irrational
          fa.add(numberExpression(baseR), expo);
        }
      } else {
        fa.add(f);
      }
    }
  }

  if(rationalPart != 1) {
    const __int64 &num = rationalPart.getNumerator();
    const __int64 &den = rationalPart.getDenominator();
    if(num != 1) {
      fa.add(numberExpression(num));
    }
    if(den != 1) {
      fa.add(numberExpression(den), getMinusOne());
    }
  }
  RETURNNODE( factorsToNode(fa) );
}

SNode Expression::reduceRationalPower(const Rational &base, const Rational &exponent) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(base, exponent);

  if(exponent.isInteger()) {
    RETURNNODE( numberExpression(pow(base, getInt(exponent))) );
  } else {
    const __int64 &ed = exponent.getDenominator();
    const __int64 &bn = base.getNumerator();
    const __int64 &bd = base.getDenominator();
    PrimeFactorArray bnPrimeFactors(bn);
    PrimeFactorArray bdPrimeFactors(bd);

    PrimeFactorSet bnRootFactors = bnPrimeFactors.findFactorsWithMultiplicityAtLeast((unsigned int)ed);
    PrimeFactorSet bdRootFactors = bdPrimeFactors.findFactorsWithMultiplicityAtLeast((unsigned int)ed);
    Rational niceRootFactor = 1;
    __int64 bnR = 1, bdR = 1;
    if((abs(bn) == 1 || !bnRootFactors.isEmpty()) && (bd == 1 || !bdRootFactors.isEmpty())) {
      for(Iterator<size_t> it1 = bnRootFactors.getIterator(); it1.hasNext();) {
        PrimeFactor &pf = bnPrimeFactors[it1.next()];
        do {
          bnR *= pf.m_prime;
          pf.m_multiplicity -= (unsigned int)ed;
        } while(pf.m_multiplicity >= (unsigned int)ed);
      }
      for(Iterator<size_t> it2 = bdRootFactors.getIterator(); it2.hasNext();) {
        PrimeFactor &pf = bdPrimeFactors[it2.next()];
        do {
          bdR *= pf.m_prime;
          pf.m_multiplicity -= (unsigned int)ed;
        } while(pf.m_multiplicity >= (unsigned int)ed);
      }
    }

    if(bnPrimeFactors.isPositive()) {
      niceRootFactor = Rational(bnR, bdR);
    } else if(isAsymmetricExponent(exponent)) {
      niceRootFactor = Rational(-bnR, bdR);
      bnPrimeFactors.setPositive();
    } else {
      throwMethodInvalidArgumentException(s_className, method, _T("Base:%s, exponent:%s"), base.toString().cstr(), exponent.toString().cstr());
    }

    FactorArray fa;
    if(niceRootFactor != 1) {
      fa.add(numberExpression(niceRootFactor), numberExpression(exponent.getNumerator()));
    }
    
    const Rational nonRootFactor = pow(Rational(bnPrimeFactors.getProduct(), bdPrimeFactors.getProduct()), (int)exponent.getNumerator());
    if(nonRootFactor != 1) {
      if(nonRootFactor.getNumerator() == 1) {
        fa.add(numberExpression(nonRootFactor.getDenominator()), numberExpression(Rational(-1,ed)));
      } else {
        fa.add(numberExpression(nonRootFactor), numberExpression(Rational(1,ed)));
      }
    }
    RETURNNODE( factorsToNode(fa) );
  }
}

bool Expression::reducesToRationalConstant(SNode n, Rational *r) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  if(!n.isConstant()) {
    RETURNBOOL( false );
  }
  const bool result = reducesToRational(n, r);
//  debugLog(_T("reducesToRational(\"%s\"):%s\n"), n->toString().cstr(), boolToStr(res));
  RETURNBOOL( result );
}

bool Expression::reducesToRational(SNode n, Rational *r) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  switch(n.getSymbol()) {
  case NUMBER         : 
    if(!n.isRational()) {
      RETURNBOOL( false ); 
    } else {
      if(r) *r = n.getRational();
      RETURNBOOL( true );
    }
  case NAME           :
    { const ExpressionVariable &var = n.variable();
      RETURNBOOL( var.isConstant() && Rational::isRealRational(getValue(var), r) );
    }

  case MINUS          :
    { assert(n.isUnaryMinus());
      Rational tmp, *tmpp = r ? &tmp : NULL;
      if(!reducesToRational(n.left(), tmpp)) {
        RETURNBOOL( false );
      }
      if(r) *r = -tmp;
      RETURNBOOL( true );
    }

  case SUM            :
    { const AddentArray &a = n.getAddentArray();
      Rational sum = 0, tmp, *tmpp = r ? &tmp : NULL;
      for(size_t i = 0; i < a.size(); i++) {
        SumElement *e = a[i];
        if(!reducesToRational(e->getNode(), tmpp)) {
          RETURNBOOL( false );
        }
        if(tmpp) {
          if(e->isPositive()) sum += tmp; else sum -= tmp;
        }
      }
      if(r) *r = sum;
    }
    RETURNBOOL( true );

  case PRODUCT        :
    { const FactorArray &a = n.getFactorArray();
      Rational product = 1, tmp, *tmpp = r ? &tmp : NULL;
      for(size_t i = 0; i < a.size(); i++) {
        if(!reducesToRational(a[i], tmpp)) {
          RETURNBOOL( false );
        }
        if(r) product *= tmp;
      }
      if(r) *r = product;
    }
    RETURNBOOL( true );

  case POW            :
    { Rational base, exponent;
      if(reducesToRational(n.left(), &base) && reducesToRational(n.right(), &exponent)) {
        const ExpressionNode *tmp = reduceRationalPower(base, exponent);
        if(tmp->isRational()) {
          if(r) *r = tmp->getRational();
          RETURNBOOL( true );
        }
      }
    }
    RETURNBOOL( false );

  default:
    { const Real tmp = evaluateRealExpr(n);
      RETURNBOOL( Rational::isRealRational(tmp, r) );
    }
  }
}

SNode Expression::reduceModulus(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode l  = n.left();
  const SNode r  = n.right();

  const SNode Rl = reduceRealExp(l);
  SNode Rr = reduceRealExp(r);
  if(Rr.isNegative() || Rr.isUnaryMinus()) Rr = -Rr;

  if(Rl.isSameNode(l) && Rr.isSameNode(r)) {
    RETURNNODE( n );
  } else {
    RETURNNODE( Rl % Rr );
  }
}

/* ------------------------------------------- multiplyParentheses ----------------------------------------- */

SNode Expression::multiplyParentheses(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  switch(n.getSymbol()) {
  case NUMBER   : RETURNNODE( n );
  case NAME     : RETURNNODE( n );
  case SUM      : RETURNNODE( multiplyParenthesesInSum(n) );
  case PRODUCT  : RETURNNODE( multiplyParenthesesInProduct(n) );
  case POLY     : RETURNNODE( multiplyParenthesesInPoly(n) );
  default       : RETURNNODE( multiplyTreeNode(n) );
  }
}

SNode Expression::multiplyParenthesesInSum(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const AddentArray &a = n.getAddentArray();
  AddentArray        newAddentArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    SumElement *e = a[i];
    newAddentArray.add(multiplyParentheses(e->getNode()), e->isPositive());
  }
  SNode result = getSum(n, newAddentArray);
  RETURNNODE( result );
}

SNode Expression::multiplyParenthesesInProduct(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const FactorArray &a = n.getFactorArray();
  FactorArray        newFactorArray;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f = a[i];
    newFactorArray.add(multiplyParentheses(f->base()), multiplyParentheses(f->exponent()));
  }

  BitSet done(newFactorArray.size() + 1);
  do {
    FactorArray tmp = newFactorArray;
    newFactorArray.clear();
    done.setCapacity(tmp.size() + 1);
    done.clear();
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      if(done.contains(i1)) continue;
      ExpressionFactor *f1 = tmp[i1];
      if((f1->base()->getSymbol() == SUM) && !f1->exponent()->isOne()) {
        continue;
      }
      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        ExpressionFactor *f2 = tmp[i2];
        if(f2->base()->getSymbol() == SUM && !f2->exponent()->isOne()) {
          continue;
        }
        if(f1->base()->getSymbol() == SUM) {
          newFactorArray.add(multiply(f2, (ExpressionNodeSum*)(f1->base())));
          done.add(i1);
          done.add(i2);
        } else if(f2->base()->getSymbol() == SUM) {
          newFactorArray.add(multiply(f1, (ExpressionNodeSum*)(f2->base())));
          done.add(i1);
          done.add(i2);
        }
      }
    }
    for(size_t i = 0; i < tmp.size(); i++) {
      if(!done.contains(i)) {
        newFactorArray.add(tmp[i]);
      }
    }
  } while(!done.isEmpty());
  SNode result = getProduct(n, newFactorArray);
  RETURNNODE( result );
}

SNode Expression::multiplyParenthesesInPoly(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();
  const ExpressionNodeArray &coef    = n.getCoefficientArray();
  SNode                      newArg  = multiplyParentheses(n.getArgument());

  ExpressionNodeArray newCoef(coef.size());
  for(size_t i = 0; i < coef.size(); i++) {
    newCoef.add(multiplyParentheses(coef[i]));
  }
  SNode result = getPoly(n, newCoef, newArg);
  RETURNNODE( result );
}

SNode Expression::multiply(ExpressionFactor *a, ExpressionNodeSum *s) {
  DEFINEMETHODNAME;
  ENTERMETHOD2(*a,*s);

  if(a->base()->getSymbol() == SUM && a->exponent()->isOne()) {
    RETURNNODE( fetchFactorNode(ExpressionNodeSum::multiply((ExpressionNodeSum*)(a->base()),s)) );
  } else {
    AddentArray        tmp;
    const AddentArray &sa = s->getAddentArray();
    for(size_t i = 0; i < sa.size(); i++) {
      SumElement *e = sa[i];
      tmp.add(productC(a, e->getNode()),e->isPositive());
    }
    RETURNNODE( fetchFactorNode(addentsToNode(tmp)) );
  }
}

SNode Expression::multiplyTreeNode(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const ExpressionNodeArray &a = n.getChildArray();
  ExpressionNodeArray newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(multiplyParentheses(a[i]));
  }
  SNode result = getTree(n, newChildArray);
  RETURNNODE( result );
}

/*
 * n.symbol = LN
 * return if argument is an integer power p of e then p else LN(reduce(leftChild))
 */
SNode Expression::reduceLn(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode arg  = n.left();
  const SNode Rarg = reduceRealExp(arg);
  const SNode p    = getPowerOfE(Rarg);
  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) { // ln(a^b) = b * ln(a)
    RETURNNODE( Rarg.right() * unaryExp(n.getSymbol(), Rarg.left()) );
  }
  RETURNNODE( Rarg.isSameNode(arg) ? n : unaryExp(n.getSymbol(), Rarg) );
}

SNode Expression::getPowerOfE(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  if(n.isEulersConstant()) {
    RETURNNODE( _1() );
  } else if(n.isOne()) {
    RETURNNODE( _0() );
  } else if((n.getSymbol() == POW) && n.left().isEulersConstant()) {
    RETURNNODE( n.right() );
  } else {
    RETURNNULL;
  }
}

/*
 * n.symbol = LOG10
 * return if argument is an integer power p of 10 then p else LOG10(reduce(leftChild))
 */
SNode Expression::reduceLog10(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode arg  = n.left();
  const SNode Rarg = reduceRealExp(arg);
  const SNode p    = getPowerOf10(Rarg);

  if(!p.isEmpty()) {
    RETURNNODE( p );
  }
  if(Rarg.getSymbol() == POW) {
    RETURNNODE( Rarg.right() * unaryExp(n.getSymbol(), Rarg.left()) );
  }
  RETURNNODE( Rarg.isSameNode(arg) ? n : unaryExp(n.getSymbol(), Rarg) );
}

SNode Expression::getPowerOf10(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  if(n.isTen()) {
    RETURNNODE( _1() );
  } else if(n.isOne()) {
    RETURNNODE( _0() );
  } else if((n.getSymbol() == POW) && n.left().isTen()) {
    RETURNNODE( n.right() );
  } else {
    RETURNNULL;
  }
}

SNode Expression::reduceAsymmetricFunction(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode arg  = n.left();
  if(n.getInverseFunction() == arg.getSymbol()) RETURNNODE( arg.left() );
  const SNode Rarg = reduceRealExp(arg);
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = -f(exp)
    RETURNNODE( -unaryExp(n.getSymbol(), Rarg.left()) );
  } else {
    RETURNNODE( Rarg.isSameNode(arg) ? n : unaryExp(n.getSymbol(), Rarg) );
  }
}

SNode Expression::reduceSymmetricFunction(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SNode arg  = n.left();
  if(n.getInverseFunction() == arg.getSymbol()) RETURNNODE( arg.left() );
  const SNode Rarg = reduceRealExp(arg);
  if(Rarg.isUnaryMinus()) {                                                       // f(-exp) = f(exp)
    RETURNNODE( unaryExp(n.getSymbol(), Rarg.left()) );
  } else {
    RETURNNODE( Rarg.isSameNode(arg) ? n : unaryExp(n.getSymbol(), Rarg) );
  }
}

/*
 * n.symbol = POLY
 * return no leading zeroes, all constant coefficients evaluated
 */
SNode Expression::reducePolynomial(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  const SExprList coefficients(n.getCoefficientArray());
  const SNode     u            = n.getArgument();  // u is the parameter to the polynomial ex. poly[a,b,c,d](u)

  SExprList newCoefficients;
  bool leadingCoef = true;
  for(size_t i = 0; i < coefficients.size(); i++) {
    const SNode &coef = coefficients[i];
    if(!coef.isConstant()) {     // coef not constant
      newCoefficients.add(reduceRealExp(coef));
    } else if(coef.isNumber()) { // coef is constant
      if(coef.isZero() && leadingCoef) continue;
      newCoefficients.add(coef);
    } else {
      Rational r;
      if(reducesToRationalConstant(coef, &r)) {
        if(r.isZero() && leadingCoef) continue;
        newCoefficients.add(numberExpression(r));
      } else {
        const Real c = evaluateRealExpr(coef);
        if(c == 0 && leadingCoef) continue;
        newCoefficients.add(numberExpression(c));
      }
    }
    leadingCoef = false;
  }
  switch(newCoefficients.size()) {
  case 0 : RETURNNODE( _0() );               // 0 polynomial == 0
  case 1 : RETURNNODE( newCoefficients[0] ); // Independent of u
  default:
    { const SNode newU = reduceRealExp(u);
      if((newCoefficients == coefficients) && (newU == u)) {
        RETURNNODE(n);
      } else {
        RETURNNODE( polyExp(newCoefficients, newU) );
      }
    }
  }
}

SNode Expression::reduceTreeNode(SNode n) {
  DEFINEMETHODNAME;
  ENTERMETHOD();

  if(n.getInverseFunction() == n.left().getSymbol()) RETURNNODE( n.left().left() );

  switch(n.getSymbol()) {
  case IIF:
    { const SNode cond   = reduceBoolExp(n.child(0));
      const SNode eTrue  = reduceRealExp(n.child(1));
      const SNode eFalse = reduceRealExp(n.child(2));
      if(cond.isTrue() ) RETURNNODE( eTrue  );
      if(cond.isFalse()) RETURNNODE( eFalse );
      RETURNNODE( condExp(cond, eTrue, eFalse) );
    }
  default:
    { const ExpressionNodeArray &a = n.getChildArray();
      ExpressionNodeArray newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(reduceRealExp(a[i]));
      }
      SNode result = getTree(n, newChildArray);
      RETURNNODE( result );
    }
  }
}
