#pragma once

#include "ExpressionNode.h"
#include <Math/Expression/SNodeReduceDbgStack.h>

// Wrapper class til ExpressionNode
class SNode {
private:
  ExpressionNode *m_node;

  SNode DPoly(    const String &name) const;
  SNode DStmtList(const String &name) const;

  static ExpressionFactor *factor(SNode b, ExpressionNode *e);

  SNode              reduceBoolExp();
  SNode              reduceRealExp();
  SNode              reduceTreeNode();
  SNode              reduceSum() const;
  SNode              reduceProduct();
  SNode              reduceModulus() const;

  SNode              reducePower();
  SNode              reduceConstantFactors(       FactorArray            &factors);
  SNode              reduceRationalPower(         const Rational         &base, const Rational &exponent);
  SNode              multiplyParenthesesInSum();
  SNode              multiplyParenthesesInProduct();
  SNode              multiplyParenthesesInPoly();
  SNode              multiplyTreeNode();
  SNode              multiply(                    ExpressionFactor *a, ExpressionNodeSum *s);

  SumElement        *getCommonFactor(             SumElement  &e1, SumElement &e2) const;
  FactorArray       &getFactors(                  FactorArray &result);
  FactorArray       &getFactors(                  FactorArray &result, const SNode exponent);
  FactorArray       &getFactorsInPower(           FactorArray &result, const SNode exponent);
  FactorArray       &multiplyExponents(           FactorArray &result, FactorArray &factors, const SNode exponent);
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
protected:
  inline ParserTree *getTree() const {
    return m_node->getTree();
  }
public:
  inline SNode() : m_node(NULL) {
  }
  inline SNode(ExpressionNode *node) : m_node(node) {
  }
  SNode _0()  const; // xero
  SNode _1()  const; // 1
  SNode _m1() const; // -1
  SNode _2()  const; // 2
  SNode _10() const; // 10
  SNode _05() const; // 1/2

  SNode(ParserTree *tree, int                v);
  SNode(ParserTree *tree, INT64              v);
  SNode(ParserTree *tree, const Rational    &v);
  SNode(ParserTree *tree, const Real        &v);
  SNode(ParserTree *tree, const Number      &v);
  SNode(ParserTree *tree, bool               v);
  SNode(ParserTree *tree, FactorArray       &a);
  SNode(ParserTree *tree, AddentArray       &a);
  inline bool isEmpty() const {
    return m_node == NULL;
  }
  inline ExpressionNode *node() {
    return m_node;
  }
  inline const ExpressionNode *node() const {
    return m_node;
  }
  inline operator ExpressionNode*() {
    return m_node;
  }
  inline operator const ExpressionNode*() const {
    return m_node;
  }
  inline ExpressionInputSymbol getSymbol() const   {
    return m_node->getSymbol();
  }
  ExpressionReturnType getReturnType() const {
    return m_node->getReturnType();
  }
  inline String getSymbolName() const {
    return m_node->getSymbolName();
  }
  inline static String getSymbolName(ExpressionInputSymbol symbol) {
    return ExpressionNode::getSymbolName(symbol);
  }
  inline ExpressionVariable &variable() const {
    return m_node->getVariable();
  }
  inline const String       &name()  const { return m_node->getName(); }
  inline SNode               left()  const { return m_node->left();    }
  inline SNode               right() const { return m_node->right();   }
  inline SNode               child(UINT index) const {
    return m_node->child(index);
  }
  inline ExpressionNodeArray &getChildArray() const {
    return m_node->getChildArray();
  }
  inline FactorArray &getFactorArray() const {
    return m_node->getFactorArray();
  }
  inline AddentArray &getAddentArray() const {
    return m_node->getAddentArray();
  }
  inline ExpressionNodeArray &getCoefficientArray() const {
    return m_node->getCoefficientArray();
  }
  inline SNode getArgument() const {
    return m_node->getArgument();
  }
  inline int getDegree() const {
    return m_node->getDegree();
  }
  inline bool isUnaryMinus() const {
    return m_node->isUnaryMinus();
  }
  inline bool isNumber() const {
    return m_node->isNumber();
  }
  inline bool isInteger() const {
    return m_node->isInteger();
  }
  inline bool isRational() const {
    return m_node->isRational();
  }
  inline bool isBooleanOperator() const {
    return m_node->isBooleanOperator();
  }
  inline const Number &getNumber() const {
    return m_node->getNumber();
  }
  inline Real getReal() const {
    return m_node->getReal();
  }
  inline Rational getRational() const {
    return m_node->getRational();
  }
  inline const Real &getValueRef() const {
    return m_node->getValueRef();
  }
  inline bool isZero() const {
    return m_node->isZero();
  }
  inline bool isOne() const {
    return m_node->isOne();
  }
  inline bool isTen() const {
    return m_node->isTen();
  }
  inline bool isTwo() const {
    return m_node->isTwo();
  }
  inline bool isEulersConstant() const {
    return m_node->isEulersConstant();
  }
  inline bool isNegative() const {
    return m_node->isNegative();
  }
  inline bool isOdd() const {
    return m_node->isOdd();
  }
  inline bool isConstant() const {
    return m_node->isConstant();
  }
  inline bool isSymmetricFunction() const {
    return m_node->isSymmetricFunction();
  }
  inline bool isAsymmetricFunction() const {
    return m_node->isAsymmetricFunction();
  }
  inline bool isTrue() const {
    return m_node->isTrue();
  }
  inline bool isFalse() const {
    return m_node->isFalse();
  }
  inline ExpressionInputSymbol getInverseFunction() const {
    return m_node->getInverseFunction();
  }
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
  bool operator==(  const SNode &n) const;
  inline bool operator!=(const SNode &n) const {
    return !(*this == n);
  }
  bool reducesToRationalConstant(Rational *r) const;
  bool reducesToRational(Rational *r) const;

  inline bool isSameNode(const SNode n) const {
    return m_node == n.m_node;
  }
  inline String toString() const {
    return isEmpty() ? _T("") : m_node->toString();
  }

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
    // Differentiation
  SNode D(const String &name) const;
  SNode reduce();
  Real  evaluateReal() const;
  bool  evaluateBool() const;
  SNode multiplyParentheses();

  inline void throwInvalidSymbolForTreeMode(const TCHAR *method) const {
    m_node->throwInvalidSymbolForTreeMode(method);
  }
  inline void throwUnknownSymbolException(const TCHAR *method) const {
    m_node->throwUnknownSymbolException(method);
  }
};

class SNodeArray : public CompactArray<SNode> {
protected:
  SNode toTree(ExpressionInputSymbol delimiter);
public:
  SNodeArray() {}
  explicit SNodeArray(size_t capacity) : CompactArray(capacity) {
  }
};

class SStmtList : public SNodeArray {
public:
  SStmtList() {
  };
  explicit SStmtList(size_t capacity) : SNodeArray(capacity) {
  };
  SStmtList(SNode n);
  SStmtList &removeUnusedAssignments();

  operator SNode() {
    return toTree(SEMI);
  }
};

class SExprList : public SNodeArray {
public:
  SExprList() {
  }
  explicit SExprList(size_t capacity) : SNodeArray(capacity) {
  }
  SExprList(ExpressionNodeArray &a);
  operator ExpressionNodeArray();
};

SNode unaryExp(  ExpressionInputSymbol symbol, SNode n);
SNode binExp(    ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode treeExp(   ExpressionInputSymbol symbol, const ExpressionNodeArray &a); // assumy a.size() > 0
SNode condExp(   SNode condition , SNode nTrue  , SNode nFalse);
SNode polyExp(   const ExpressionNodeArray &coefArray, SNode arg);
SNode stmtList(  const ExpressionNodeArray &list);
SNode indexSum(  SNode assignStmt, SNode endExpr, SNode expr  );
SNode indexProd( SNode assignStmt, SNode endExpr, SNode expr  );
SNode assignStmt(SNode leftSide  , SNode expr);
SNode factorExp( SNode b         , SNode e);

bool equal(const ExpressionNode *n1, const ExpressionNode *n2);
bool equalMinus(SNode n1, SNode n2);
