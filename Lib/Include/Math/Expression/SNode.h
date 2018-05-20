#pragma once

#include <Math/Number.h>
#include <Math/Expression/ExpressionSymbol.h>
#include "SNodeReduceDbgStack.h"

namespace Expr {

class ExpressionVariable;
class ExpressionNode;
class SumElement;
class ExpressionFactor;
class ExpressionNodeSum;
class AddentArray;
class FactorArray;
class ParserTree;

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_REAL
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;

typedef enum {
  EXPRESSIONNODENUMBER
 ,EXPRESSIONNODEBOOLEAN
 ,EXPRESSIONNODEVARIABLE
 ,EXPRESSIONNODEPOLYNOMIAL
 ,EXPRESSIONNODETREE
 ,EXPRESSIONNODESUM
 ,EXPRESSIONNODEPRODUCT
 ,EXPRESSIONNODEFACTOR
} ExpressionNodeType;

// Wrapper class til ExpressionNode
class SNode {
private:
  ExpressionNode *m_node;

  SNode DPoly(    const String &name) const;
  SNode DStmtList(const String &name) const;

  static ExpressionFactor *factor(SNode b, SNode e);

  SNode              reduceBoolExp();
  SNode              reduceRealExp();
  SNode              reduceTreeNode();
  SNode              reduceSum() const;
  SNode              reduceProduct();
  SNode              reduceModulus() const;

  SNode              reducePower();
  SNode              reduceConstantFactors(       FactorArray            &factors);
  SNode              reduceRationalPower(         const Rational         &base, const Rational &exponent);
  SNode              multiplyParenthesesInSum() const;
  SNode              multiplyParenthesesInProduct() const;
  SNode              multiplyParenthesesInPoly() const;
  SNode              multiplyTreeNode() const;
  SNode              multiplyFactorSum(SNode factor, SNode sum) const; //  ExpressionFactor *a, ExpressionNodeSum *s) const;

  SumElement        *getCommonFactor(             SumElement  &e1, SumElement &e2) const;
  FactorArray       &getFactors(                  FactorArray &result);
  FactorArray       &getFactors(                  FactorArray &result, const SNode exponent);
  FactorArray       &getFactorsInPower(           FactorArray &result, const SNode exponent);
  FactorArray       &multiplyExponents(           FactorArray &result, FactorArray &factors, const SNode exponent);
  SNode              multiplySumSum(              SNode n1, SNode n2) const; // assume n1 and n2 are EXPRESSIONNODESUM
  SNode              changeFirstNegativeFactor() const;
  SNode              reduceLn();
  SNode              reduceLog10();
  SNode              reduceLog2();
  SNode              getPowerOfE();
  SNode              getPowerOf10();
  SNode              getPowerOf2();
  SumElement        *mergeLogarithms(            SumElement        &e1, SumElement       &e2) const;
  SNode              reduceAsymmetricFunction();
  SNode              reduceSymmetricFunction();
  SNode              reducePolynomial();
  ExpressionFactor  *reduceTrigonometricFactors(  ExpressionFactor &f1, ExpressionFactor &f2);

  bool               canUseIdiotRule(             SNode             n1, const SNode n2) const;
  bool               canUseReverseIdiotRule(      SumElement       *e1,       SumElement *e2, SumElement* &result) const;
  bool               isSquareOfSinOrCos() const;
  bool               sameLogarithm(               const SNode n) const;
  static bool        rationalExponentsMultiply(   const Rational   &r1, const Rational &r2);

  friend class MarkedNodeMultiplier;
public:
  friend class SNodeArray;
  inline SNode() : m_node(NULL) {
  }
  inline SNode(ExpressionNode *node) : m_node(node) {
  }
  SNode(ParserTree *tree, int                v);
  SNode(ParserTree *tree, INT64              v);
  SNode(ParserTree *tree, const Rational    &v);
  SNode(ParserTree *tree, const Real        &v);
  SNode(ParserTree *tree, const Number      &v);
  SNode(ParserTree *tree, bool               v);
  SNode(ParserTree *tree, AddentArray       &a);
  SNode(ParserTree *tree, FactorArray       &a);

  SNode _0()  const; // zero
  SNode _1()  const; // 1
  SNode _m1() const; // -1
  SNode _2()  const; // 2
  SNode _10() const; // 10
  SNode _05() const; // 1/2

  inline bool isEmpty() const {
    return m_node == NULL;
  }
  inline ExpressionNode *node() const {
    return m_node;
  }
  ExpressionInputSymbol getSymbol()                            const;
  ParserTree           *getTree()                              const;
  String                getSymbolName()                        const;
  static String         getSymbolName(ExpressionInputSymbol symbol);
  void                  mark();
  void                  unMark();
  bool                  isMarked()                             const;
  SNode                 left()                                 const;
  SNode                 right()                                const;
  SNode                 getArgument()                          const;
  SNode                 child(UINT index)                      const;
  const SNodeArray     &getChildArray()                        const;
        SNodeArray     &getChildArray();
  const FactorArray    &getFactorArray()                       const;
        FactorArray    &getFactorArray();
  const AddentArray    &getAddentArray()                       const;
        AddentArray    &getAddentArray();
  const SNodeArray     &getCoefArray()                         const;
        SNodeArray     &getCoefArray();
  int                   getFirstCoefIndex()                    const;
  int                   getDegree()                            const;
  const String         &getName()                              const;
  ExpressionVariable   &getVariable()                          const;
  const Number         &getNumber()                            const;
  int                   getValueIndex()                        const;
  bool                  isConstant()                           const;
  bool                  isBooleanOperator()                    const;
  Real                 &doAssignment()                         const;
  Real                  evaluateReal()                         const;
  bool                  evaluateBool()                         const;
  ExpressionNodeType    getNodeType()                          const;
  ExpressionReturnType  getReturnType()                        const;

  String                toString()                             const;

  bool                  isNumber()                             const;
  bool                  isRational()                           const;
  bool                  isInteger()                            const;
  bool                  isNameOrNumber()                       const;
  bool                  isEven()                               const;
  bool                  isOdd()                                const;
  Real                  getReal()                              const;
  Rational              getRational()                          const;
  bool                  isUnaryMinus()                         const;
  bool                  isEulersConstant()                     const;
  bool                  isPi()                                 const;
  bool                  isZero()                               const;
  bool                  isOne()                                const;
  bool                  isTwo()                                const;
  bool                  isTen()                                const;
  bool                  isNegative()                           const;
  bool                  isPositive()                           const;
  bool                  isTrue()                               const;
  bool                  isFalse()                              const;
  ExpressionInputSymbol getInverseFunction()                   const;

  bool                  isCoefficientArrayConstant()           const;
  bool                  containsFunctionCall()                 const;
  const Real           &getValueRef()                          const;
  bool                  isTrigonomtricFunction()               const;
  bool                  isSymmetricFunction()                  const;
  bool                  isAsymmetricFunction()                 const;
  bool                  reducesToRationalConstant(Rational *r) const;
  bool                  reducesToRational(        Rational *r) const;
  bool                  needParentheses(SNode parent)          const;
  SNode                 base()                                 const;
  SNode                 exponent()                             const;

  SNode operator+(  const SNode &n) const;
  // binary -
  SNode operator-(  const SNode &n) const;
  // unary  -
  SNode operator-() const;
  SNode operator*(  const SNode &n) const;
  SNode operator/(  const SNode &n) const;
  SNode operator%(  const SNode &n) const;
  SNode &operator+=(const SNode &n);
  SNode &operator-=(const SNode &n);
  SNode &operator*=(const SNode &n);
  SNode &operator/=(const SNode &n);
  SNode operator&&( const SNode &n) const;
  SNode operator||( const SNode &n) const;
  SNode operator!() const;
  // compare trees recursively
  bool operator==(  const SNode &n) const;
  inline bool operator!=(const SNode &n) const {
    return !(*this == n);
  }

  inline bool isSameNode(const SNode n) const {
    return m_node == n.m_node;
  }

  SNode multiplyParentheses() const;

    // Differentiation
  SNode D(const String &name) const;
  SNode reduce();

  void throwInvalidSymbolForTreeMode(const TCHAR *method) const;
  void throwUnknownSymbolException(const TCHAR *method) const;

  friend SNode reciprocal(const SNode &x);
  friend SNode sqrt(      const SNode &x);
  friend SNode sqr(       const SNode &x);
  friend SNode pow(       const SNode &x, const SNode &y);
  friend SNode root(      const SNode &x, const SNode &y);
  friend SNode exp(       const SNode &x);
  friend SNode exp10(     const SNode &x);
  friend SNode exp2(      const SNode &x);
  friend SNode ln(        const SNode &x);
  friend SNode log10(     const SNode &x);
  friend SNode log2(      const SNode &x);
  friend SNode sin(       const SNode &x);
  friend SNode cos(       const SNode &x);
  friend SNode tan(       const SNode &x);
  friend SNode cot(       const SNode &x);
  friend SNode asin(      const SNode &x);
  friend SNode acos(      const SNode &x);
  friend SNode atan(      const SNode &x);
  friend SNode atan2(     const SNode &y, const SNode &x);
  friend SNode acot(      const SNode &x);
  friend SNode csc(       const SNode &x);
  friend SNode sec(       const SNode &x);

  friend SNode sinh(      const SNode &x);
  friend SNode cosh(      const SNode &x);
  friend SNode tanh(      const SNode &x);
  friend SNode gauss(     const SNode &x);
};

class SNodeArray : public CompactArray<SNode> {
protected:
  SNode toTree(ExpressionInputSymbol delimiter);
public:
  SNodeArray() {}
  SNodeArray(int n,...);
  explicit SNodeArray(size_t capacity) : CompactArray(capacity) {
  }
  inline ParserTree *getTree() const {
    if(isEmpty()) throwException(_T("SNodeArray:array is empty"));
    return (*this)[0].getTree();
  }
  bool isConstant() const;
  SNodeArray &cloneNodes(SNodeArray &dst, ParserTree *tree) const;
};

class SStmtList : public SNodeArray {
public:
  SStmtList() {}
  SStmtList(const SNodeArray &a) : SNodeArray(a) {
  };
  SStmtList &removeUnusedAssignments();

  operator SNode() {
    return toTree(SEMI);
  }
};

SNode unaryExp(  ExpressionInputSymbol symbol, SNode n);
SNode binExp(    ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode treeExp(   ExpressionInputSymbol symbol, const SNodeArray &a); // assumy a.size() > 0
SNode condExp(   SNode condition , SNode nTrue  , SNode nFalse);
SNode polyExp(   const SNodeArray &coefArray, SNode arg);
SNode stmtList(  const SNodeArray &list);
SNode indexSum(  SNode assignStmt, SNode endExpr, SNode expr  );
SNode indexProd( SNode assignStmt, SNode endExpr, SNode expr  );
SNode assignStmt(SNode leftSide  , SNode expr);
SNode factorExp( SNode b         , SNode e);

bool equal(     SNode n1, SNode n2);
bool equalMinus(SNode n1, SNode n2);

}; // namespace Expr
