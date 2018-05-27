#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

namespace Expr {

SNode::SNode(ParserTree &tree, int v) {
  m_node = tree.numberExpression(v);
}

SNode::SNode(ParserTree &tree, INT64 v) {
  m_node = tree.numberExpression(v);
}

SNode::SNode(ParserTree &tree, const Rational &v) {
  m_node = tree.numberExpression(v);
}

SNode::SNode(ParserTree &tree, const Real &v) {
  m_node = tree.numberExpression(v);
}

SNode::SNode(ParserTree &tree, const Number &v) {
  m_node = tree.numberExpression(v);
}

SNode::SNode(ParserTree &tree, bool v) {
  m_node = v ? tree.getTrue() : tree.getFalse();
}

SNode::SNode(ParserTree &tree, AddentArray &a) {
  m_node = tree.getSum(a);
}

SNode::SNode(ParserTree &tree, FactorArray &a) {
  m_node = tree.getProduct(a);
}

SNode SNode::_0()     const { return getTree().getZero();     }
SNode SNode::_1()     const { return getTree().getOne();      }
SNode SNode::_m1()    const { return getTree().getMinusOne(); }
SNode SNode::_2()     const { return getTree().getTwo();      }
SNode SNode::_10()    const { return getTree().getTen();      }
SNode SNode::_05()    const { return getTree().getHalf();     }
SNode SNode::_false() const { return getTree().getFalse();    } // false
SNode SNode::_true()  const { return getTree().getTrue();     } // true

ExpressionInputSymbol SNode::getSymbol() const {
  return m_node->getSymbol();
}

ParserTree &SNode::getTree() const {
  return m_node->getTree();
}

String SNode::getSymbolName() const {
  return getSymbolName(getSymbol());
}

String SNode::getSymbolName(ExpressionInputSymbol symbol) { // static
  return ExpressionNode::getSymbolName(symbol);
}

void SNode::mark() {
  m_node->mark();
}

void SNode::unMark() {
  m_node->unMark();
}

bool SNode::isMarked() const {
  return m_node->isMarked();
}

SNode SNode::left() const {
  return m_node->child(0);
}

SNode SNode::right() const {
  return m_node->child(1);
}

SNode SNode::getArgument() const {
  return m_node->getArgument();
}

SNode SNode::child(UINT index) const {
  return m_node->child(index);
}

const SNodeArray &SNode::getChildArray() const {
  return m_node->getChildArray();
}

SNodeArray &SNode::getChildArray() {
  return m_node->getChildArray();
}

const FactorArray &SNode::getFactorArray() const {
  return m_node->getFactorArray();
}

FactorArray &SNode::getFactorArray() {
  return m_node->getFactorArray();
}

const AddentArray &SNode::getAddentArray() const {
  return m_node->getAddentArray();
}

AddentArray &SNode::getAddentArray() {
  return m_node->getAddentArray();
}

const SNodeArray &SNode::getCoefArray() const {
  return m_node->getCoefArray();
}

SNodeArray &SNode::getCoefArray() {
  return m_node->getCoefArray();
}

int SNode::getFirstCoefIndex() const {
  return m_node->getFirstCoefIndex();
}

int SNode::getDegree() const {
  return m_node->getDegree();
}

const String &SNode::getName() const {
  return m_node->getName();
}

ExpressionVariable &SNode::getVariable() const {
  return m_node->getVariable();
}

const Number &SNode::getNumber() const {
  return m_node->getNumber();
}

int SNode::getValueIndex() const {
  return m_node->getValueIndex();
}

bool SNode::isConstant() const {
  return m_node->isConstant();
}

bool SNode::isBooleanOperator() const {
  return m_node->isBooleanOperator();
}

bool SNode::isCompareOperator() const {
  return m_node->isCompareOperator();
}

Real &SNode::doAssignment() const {
  return m_node->doAssignment();
}

Real SNode::evaluateReal() const {
  return m_node->evaluateReal();
}

bool SNode::evaluateBool() const {
  return m_node->evaluateBool();
}

ExpressionNodeType SNode::getNodeType() const {
  return m_node->getNodeType();
}

ExpressionReturnType SNode::getReturnType() const {
  return m_node->getReturnType();
}

String SNode::toString() const {
  return isEmpty() ? _T("") : m_node->toString();
}

bool SNode::isNumber() const {
  return m_node->isNumber();
}

bool SNode::isRational() const {
  return m_node->isRational();
}

bool SNode::isInteger() const {
  return m_node->isInteger();
}

bool SNode::isNameOrNumber() const {
  return m_node->isNameOrNumber();
}

bool SNode::isEven() const {
  return m_node->isEven();
}

bool SNode::isOdd() const {
  return m_node->isOdd();
}

Real SNode::getReal() const {
  return m_node->getReal();
}

Rational SNode::getRational() const {
  return m_node->getRational();
}

bool SNode::isUnaryMinus() const {
  return m_node->isUnaryMinus();
}

bool SNode::isEulersConstant() const {
  return m_node->isEulersConstant();
}

bool SNode::isPi() const {
  return m_node->isPi();
}

bool SNode::isZero() const {
  return m_node->isZero();
}

bool SNode::isOne() const {
  return m_node->isOne();
}

bool SNode::isTwo() const {
  return m_node->isTwo();
}

bool SNode::isTen() const {
  return m_node->isTen();
}

bool SNode::isMinusOne() const {
  return m_node->isMinusOne();
}

bool SNode::isMinusTwo() const {
  return m_node->isMinusTwo();
}

bool SNode::isNegative() const {
  return m_node->isNegative();
}

bool SNode::isPositive() const {
  return m_node->isPositive();
}

bool SNode::isTrue() const {
  return m_node->isTrue();
}

bool SNode::isFalse() const {
  return m_node->isFalse();
}

ExpressionInputSymbol SNode::getInverseFunction() const {
  return m_node->getInverseFunction();
}

bool SNode::isCoefArrayConstant() const {
  return m_node->isCoefArrayConstant();
}

bool SNode::dependsOn(const String &name) const {
  return m_node->dependsOn(name);
}

bool SNode::containsFunctionCall() const {
  return m_node->containsFunctionCall();
}

const Real &SNode::getValueRef() const {
  return m_node->getValueRef();
}

bool SNode::isTrigonomtricFunction() const {
  return m_node->isTrigonomtricFunction();
}

bool SNode::isSymmetricFunction() const {
  return m_node->isSymmetricFunction();
}

bool SNode::isAsymmetricFunction() const {
  return m_node->isAsymmetricFunction();
}

bool SNode::reducesToRationalConstant(Rational *r) const {
  return m_node->reducesToRationalConstant(r);
}

bool SNode::reducesToRational(Rational *r) const {
  return m_node->reducesToRational(r);
}

bool SNode::needParentheses(SNode parent) const {
  return m_node->needParentheses(parent.m_node);
}

SNode SNode::base() const {
  assert(getNodeType() == NT_FACTOR);
  return ((ExpressionFactor*)m_node)->base();
}

SNode SNode::exponent() const {
  assert(getNodeType() == NT_FACTOR);
  return ((ExpressionFactor*)m_node)->exponent();
}

#ifdef CHECK_CONSISTENCY
void SNode::checkIsConsistent() const {
  m_node->checkIsConsistent();
}
#endif // CHECK_CONSISTENCY

SNode SNode::operator+(const SNode &n) const {
  return getTree().sum(m_node, n.m_node);
}

SNode SNode::operator-(const SNode &n) const { // binary -
  return getTree().diff(m_node, n.m_node);
}

SNode SNode::operator-() const { // unary -
  return getTree().minus(m_node);
}

SNode SNode::operator*(const SNode &n) const {
  return getTree().prod(m_node, n.m_node);
}

SNode SNode::operator/(const SNode &n) const {
  return getTree().quot(m_node, n.m_node);
}

SNode &SNode::operator+=(const SNode &n) {
  m_node = (*this + n).node();
  return *this;
}

SNode &SNode::operator-=(const SNode &n) {
  m_node = (*this - n).node();
  return *this;
}

SNode &SNode::operator*=(const SNode &n) {
  m_node = (*this * n).node();
  return *this;
}

SNode &SNode::operator/=(const SNode &n) {
  m_node = (*this / n).node();
  return *this;
}

SNode SNode::operator%(const SNode &n) const {
  return getTree().mod(m_node, n.m_node);
}

SNode SNode::operator&&(const SNode &n) const {
  return boolExp(AND, *this, n);
}

SNode SNode::operator||(const SNode &n) const {
  return boolExp(OR, *this, n);
}

SNode SNode::operator!() const {
  return boolExp(NOT,*this);
}

bool SNode::equal(const SNode &n) const {
  return Expr::equal(m_node, n.m_node);
}

bool SNode::equalMinus(const SNode &n) const {
  return Expr::equalMinus(m_node, n.m_node);
}

void SNode::throwInvalidSymbolForTreeMode(const TCHAR *method) const {
  m_node->throwInvalidSymbolForTreeMode(method);
}
void SNode::throwUnknownSymbolException(const TCHAR *method) const {
  m_node->throwUnknownSymbolException(method);
}

void SNode::throwUnknownNodeTypeException(const TCHAR *method) const {
  m_node->throwUnknownNodeTypeException(method);
}

// ----------------------------------- SNode member functions -------------------------------

SNode reciprocal(const SNode &x) {
  return x.getTree().reciprocal(x.m_node);
}

SNode sqrt(const SNode &x) {
  return x.getTree().sqrt(x.m_node);
}

SNode sqr(const SNode &x) {
  return x.getTree().sqr(x.m_node);
}

SNode pow(const SNode &x, const SNode &y) {
  return x.getTree().power(x.m_node, y.m_node);
}

SNode root(const SNode &x, const SNode &y) {
  return x.getTree().root(x.m_node, y.m_node);
}

SNode exp(const SNode &x) {
  return x.getTree().exp(x.m_node);
}

SNode exp10(const SNode &x) {
  return x.getTree().exp10(x.m_node);
}

SNode exp2(const SNode &x) {
  return x.getTree().exp2(x.m_node);
}

SNode ln(const SNode &x) {
  return unaryExp(LN, x);
}

SNode log10(const SNode &x) {
  return unaryExp(LOG10, x);
}

SNode log2(const SNode &x) {
  return unaryExp(LOG2, x);
}

SNode sin(const SNode &x) {
  return unaryExp(SIN, x);
}

SNode cos(const SNode &x) {
  return unaryExp(COS, x);
}

SNode tan(const SNode &x) {
  return unaryExp(TAN, x);
}

SNode cot(const SNode &x) {
  return x.getTree().cot(x.m_node);
}

SNode asin(const SNode &x) {
  return unaryExp(ASIN, x);
}

SNode acos(const SNode &x) {
  return unaryExp(ACOS, x);
}

SNode atan(const SNode &x) {
  return unaryExp(ATAN, x);
}

SNode atan2(const SNode &y, const SNode &x) {
  return binExp(ATAN2, y, x);
}

SNode acot(const SNode &x) {
  return unaryExp(ACOT, x);
}

SNode csc(const SNode &x) {
  return x.getTree().csc(x.m_node);
}

SNode sec(const SNode &x) {
  return x.getTree().sec(x.m_node);
}

SNode sinh(const SNode &x) {
  return unaryExp(SINH, x);
}

SNode cosh(const SNode &x) {
  return unaryExp(COSH, x);
}

SNode tanh(const SNode &x) {
  return unaryExp(TANH, x);
}

SNode gauss(const SNode &x) {
  return unaryExp(GAUSS, x);
}

// -------------------------- SNodeArray -----------------------

SNodeArray::SNodeArray(int n, ...) {
  va_list argptr;
  va_start(argptr,n);
  setCapacity(n);
  for(int i = 0; i < n; i++) {
    add(va_arg(argptr,SNode));
  }
  va_end(argptr);
}

bool SNodeArray::isConstant() const {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].isConstant()) return false;
  }
  return true;
}

bool SNodeArray::isSameNodes(const SNodeArray &a) const {
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].isSameNode(a[i])) {
      return false;
    }
  }
  return true;
}

bool SNodeArray::equal(const SNodeArray &a) const { // recursive compare all nodes
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!Expr::equal((*this)[i].node(), a[i].node())) {
      return false;
    }
  }
  return true;
}

bool SNodeArray::equalMinus(const SNodeArray &a) const { // recursive compare all nodes ( deep compare)
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!Expr::equalMinus((*this)[i].node(), a[i].node())) {
      return false;
    }
  }
  return true;
}

SNodeArray &SNodeArray::cloneNodes(SNodeArray &dst, ParserTree *tree) const {
  dst.clear(-1);
  dst.setCapacity(size());
  for(size_t i = 0; i < size(); i++) {
    dst.add((*this)[i].node()->clone(tree));
  }
  return dst;
}

// -------------------------------SStmtList -------------------

SNodeArray &SStmtList::removeUnusedAssignments() {
  for(int i = (int)size()-1; i--;) { // Remove unused assignments
    const SNode  &stmt = (*this)[i];
    const String &varName = stmt.left().getName();
    bool isUsed = false;
    for(size_t j = i+1; j < size(); j++) {
      if((*this)[j].dependsOn(varName)) {
        isUsed = true;
        break;
      }
    }
    if(!isUsed) {
      remove(i);
    }
  }
  return *this;
}

// ---------------------------- helpers -----------------

SNode unaryExp(ExpressionInputSymbol symbol, SNode n) {
  return n.getTree().unaryExpr(symbol, n.node());
}

SNode binExp(ExpressionInputSymbol symbol, SNode n1, SNode n2) {
  return n1.getTree().binaryExpr(symbol, n1.node(), n2.node());
}

SNode treeExp(ExpressionInputSymbol symbol, SNodeArray &a) {
  switch (symbol) {
  case STMTLIST: return stmtList(a);
  case ASSIGN  : return assignStmt(a);
  case AND     :
  case OR      :
  case NOT     :
  case EQ      :
  case NE      :
  case LT      :
  case LE      :
  case GT      :
  case GE      : return boolExp(symbol, a);
  default      : return a.getTree().treeExpr(symbol, a);
  }
}

SNode condExp(SNode condition, SNode nTrue, SNode nFalse) {
  return condition.getTree().condExpr(condition.node(), nTrue.node(), nFalse.node());
}

SNode boolExp(ExpressionInputSymbol symbol, SNode left, SNode right) {
  return left.getTree().boolExpr(symbol, left.node(), right.node());
}

SNode boolExp(ExpressionInputSymbol symbol, SNode child) {
  return child.getTree().boolExpr(symbol, child.node(), NULL);
}

SNode boolExp(ExpressionInputSymbol symbol, SNodeArray &a) {
  return a[0].getTree().getBoolExpr(symbol, a);
}

SNode polyExp(SNodeArray &coefArray, SNode arg) {
  return arg.getTree().getPoly(coefArray, arg.node());
}

SNode assignStmt(SNode leftSide, SNode expr) {
  return leftSide.getTree().assignStmt(leftSide.node(), expr.node());
}

SNode assignStmt(SNodeArray &list) {
  assert(list.size() == 2);
  return assignStmt(list[0],list[1]);
}

SNode stmtList(SNodeArray &list) {
  return list.getTree().getStmtList(list);
}

SNode indexSum(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.getTree().indexedSum(assignStmt.node(), endExpr.node(), expr.node());
}

SNode indexProd(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.getTree().indexedProduct(assignStmt.node(), endExpr.node(), expr.node());
}

SNode factorExp(SNode base, SNode expo) {
  return base.getTree().factorExpr(base.node(),expo.node());
}

}; // namespace Expr
