#include "pch.h"
#include <Math/Expression/Expression.h>

namespace Expr {

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr)
: ExpressionNode(tree, symbol)
, m_childArray(*tree)
{
  const int count = initChildArray(argptr);
#ifdef _DEBUG
  if(count) {
    SETDEBUGSTRING();
  }
#endif // _DEBUG
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, const SNodeArray &childArray)
  : ExpressionNode(tree, symbol)
  , m_childArray(*tree)
{
  m_childArray.addAll(childArray);
  SETDEBUGSTRING();
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, const ExpressionNodeTree *src)
: ExpressionNode(tree, src->getSymbol())
, m_childArray(*tree)
{
  src->getChildArray().cloneNodes(m_childArray, tree);
  SETDEBUGSTRING();
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, ...)
: ExpressionNode(tree, symbol)
, m_childArray(*tree)
{
  va_list argptr;
  va_start(argptr, symbol);
  const int count = initChildArray(argptr);
  va_end(argptr);
#ifdef _DEBUG
  if(count) {
    SETDEBUGSTRING();
  }
#endif // _DEBUG
}

ExpressionNode *ExpressionNodeTree::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeTree(tree, this); TRACE_NEW(n);
  return n;
}

static int countVargs(va_list argptr) {
  int count = 0;
  for(ExpressionNode *p = va_arg(argptr, ExpressionNode*); p; p = va_arg(argptr, ExpressionNode*)) {
    count++;
  }
  return count;
}

int ExpressionNodeTree::initChildArray(va_list argptr) {
  const int count = countVargs(argptr);
  m_childArray.clear(count);
  for(ExpressionNode *p = va_arg(argptr, ExpressionNode*); p; p = va_arg(argptr, ExpressionNode*)) {
    m_childArray.add(p);
  }
  return count;
}

int ExpressionNodeTree::compare(const ExpressionNode *n) const {
  if(n->getSymbol() != getSymbol()) {
    return __super::compare(n);
  }
  if(isBinaryOperator()) {
    const int leftCompare = left()->compare(n->left());
    const bool um1 = isUnaryMinus();
    const bool um2 = n->isUnaryMinus();
    if(um1 == um2) {
      if(um1) return leftCompare; // both are unary minus
      // else both are binary minus
      const bool leftNumeric  = left()->isNumber()  && n->left()->isNumber();
      const bool rightNumeric = right()->isNumber() && n->right()->isNumber();
      const int  rightCompare = right()->compare(n->right());
      if(leftNumeric == rightNumeric) {
        return (leftCompare == 0) ? rightCompare : leftCompare;
      } else if(leftNumeric) { // !rightNumeric
        return rightCompare;
      } else {                 // rightNumeric && !leftNumeric
        return leftCompare;
      }
    } else if(um1) {           // && !um2
      return left()->compare(n);
    } else if(um2) {           // && !um1
      return compare(n->left());
    }
  } else {
    const SNodeArray &a1 = getChildArray();
    const SNodeArray &a2 = n->getChildArray();
    for(size_t i = 0; i < a1.size(); i++) { // the have the same symbol => a1.size() == a2.size()
      const int c = a1[i].node()->compare(a2[i].node());
      if(c) return c;
    }
  }
  return 0;
}

bool ExpressionNodeTree::equal(const ExpressionNode *n) const {
  switch(getSymbol()) {
  case MINUS :
    assert(isUnaryMinus());
    assert(n->isUnaryMinus());
    return Expr::equal(left(), n->left());

  case MOD      :
    if(!Expr::equal(left(), n->left())) {
      return false;
    }
    return Expr::equal(right(), n->right()) || Expr::equalMinus(right(), n->right());

  case PLUS  :
  case PROD  :
  case QUOT  :
  case SQR   :
  case ROOT  :
  case SQRT  :
  case EXP   :
  case SEC   :
  case CSC   :
  case COT   :
    throwInvalidSymbolForTreeMode(__TFUNCTION__);

  default:
    if(isSymmetricFunction()) {
      return Expr::equal(left(), n->left()) || Expr::equalMinus(left(), n->left());
    }
    return getChildArray().equal(n->getChildArray());
  }
}

bool ExpressionNodeTree::equalMinus(const ExpressionNode *n) const {
  switch(getSymbol()) {
  case MINUS :
    assert(isUnaryMinus());
    assert(n->isUnaryMinus());
    return Expr::equalMinus(left(), n->left());

  case MOD   :
    if(!Expr::equalMinus(left(), n->left())) {
      return false;
    }
    return Expr::equal(right(), n->right()) || Expr::equalMinus(right(), n->right());

  case INDEXEDSUM    :
    return Expr::equal(left(), n->left()) && Expr::equal(right(), n->right())
        && Expr::equalMinus(child(2).node(), n->child(2).node());

  case PLUS  :
  case PROD  :
  case QUOT  :
  case SQR   :
  case ROOT  :
  case SQRT  :
  case EXP   :
  case SEC   :
  case CSC   :
  case COT   :
    throwInvalidSymbolForTreeMode(__TFUNCTION__);

  default:
    if(isAsymmetricFunction()) {
      return Expr::equalMinus(left(), n->left());
    }
    return false;
  }
}

class IndexedExpressionDependencyChecker : public ExpressionNodeHandler {
private:
  const ExpressionVariable &m_loopVar;
  bool                      m_nonConstantFound;
public:
  IndexedExpressionDependencyChecker(const ExpressionVariable &loopVar) : m_loopVar(loopVar) {
    m_nonConstantFound = false;
  }

  bool handleNode(ExpressionNode *n, int level);
  bool dependsOnNonConstantNames() const {
    return m_nonConstantFound;
  }
};

bool IndexedExpressionDependencyChecker::handleNode(ExpressionNode *n, int level) {
  if(!n->isName()) {
    return true;
  }
  const ExpressionVariable &v = n->getVariable();
  if((&v != &m_loopVar) && !v.isConstant()) {
    m_nonConstantFound = true;
    return false; // terminate treescan
  }
  return true;
}

static bool exprDependsOnNonConstantNames(const ExpressionNode *expr, const ExpressionVariable &loopVar) {
  IndexedExpressionDependencyChecker nodeHandler(loopVar);
  ((ExpressionNode*)expr)->traverseExpression(nodeHandler, 0);
  return nodeHandler.dependsOnNonConstantNames();
}

bool ExpressionNodeTree::isConstant(Number *v) const {
  bool result;
  switch(getSymbol()) {
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    { const SNode startAssignment = child(0);
      const ExpressionVariable &loopVar         = startAssignment.left().getVariable();
      const SNode               beginExpr       = startAssignment.right();
      const SNode               endExpr         = child(1);
      const SNode               expr            = child(2);
      result = beginExpr.isConstant() && endExpr.isConstant() && !exprDependsOnNonConstantNames(expr.node(), loopVar);
    }
    break;
  default:
    result = m_childArray.isConstant();
    break;
  }
  if(result && (v != NULL)) {
    *v = evaluateReal();
  }
  return result;
}

bool ExpressionNodeTree::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;
  SNodeArray &a = getChildArray();
  level++;
  for(size_t i = 0; i < a.size(); i++) {
    if(!a[i].node()->traverseExpression(handler, level)) return false;
  }
  return true;
}


void ExpressionNodeTree::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(size_t i = 0; i < m_childArray.size(); i++) {
    m_childArray[i].node()->dumpNode(s, level+1);
  }
}

#define CHILDSTR(i)    m_childArray[i].node()->toString()
#define CHILDPARSTR(i) m_childArray[i].node()->parenthesizedExpressionToString(this)

#define RPSTR    _T(")")
#define COMMASTR _T(",")


String ExpressionNodeTree::toString() const {
  switch(getSymbol()) {
  case MINUS     :
    if(isUnaryMinus()) {
      return _T("-") + CHILDPARSTR(0);
    } else {
      return CHILDPARSTR(0) + _T(" - ") + CHILDPARSTR(1);
    }

  case PLUS          : return CHILDPARSTR(0) + _T(" + "  ) + CHILDPARSTR(1);
  case PROD          : return CHILDPARSTR(0) + _T("*"    ) + CHILDPARSTR(1);
  case QUOT          : return CHILDPARSTR(0) + _T("/"    ) + CHILDPARSTR(1);
  case MOD           : return CHILDPARSTR(0) + _T("%"    ) + CHILDPARSTR(1);
  case POW           : return CHILDPARSTR(0) + _T("^"    ) + CHILDPARSTR(1);
  case ROOT          : return _T("root("     ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case BINOMIAL      : return _T("binomial(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case CHI2DENS      : return _T("chi2dens(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case CHI2DIST      : return _T("chi2dist(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case LINCGAMMA     : return _T("lincgamma(") + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case MAX           : return _T("max("      ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case MIN           : return _T("min("      ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case RAND          : return _T("rand("     ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case NORMRAND      : return _T("normrand(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case ABS           : return _T("abs("   ) + CHILDSTR(0) + RPSTR;
  case ACOS          : return _T("acos("  ) + CHILDSTR(0) + RPSTR;
  case ACOSH         : return _T("acosh(" ) + CHILDSTR(0) + RPSTR;
  case ACOT          : return _T("acot("  ) + CHILDSTR(0) + RPSTR;
  case ACSC          : return _T("acsc("  ) + CHILDSTR(0) + RPSTR;
  case ASEC          : return _T("asec("  ) + CHILDSTR(0) + RPSTR;
  case ASIN          : return _T("asin("  ) + CHILDSTR(0) + RPSTR;
  case ASINH         : return _T("asinh(" ) + CHILDSTR(0) + RPSTR;
  case ATAN          : return _T("atan("  ) + CHILDSTR(0) + RPSTR;
  case ATAN2         : return _T("atan2(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case ATANH         : return _T("atanh(" ) + CHILDSTR(0) + RPSTR;
  case CEIL          : return _T("ceil("  ) + CHILDSTR(0) + RPSTR;
  case COS           : return _T("cos("   ) + CHILDSTR(0) + RPSTR;
  case COSH          : return _T("cosh("  ) + CHILDSTR(0) + RPSTR;
  case COT           : return _T("cot("   ) + CHILDSTR(0) + RPSTR;
  case CSC           : return _T("csc("   ) + CHILDSTR(0) + RPSTR;
  case ERF           : return _T("erf("   ) + CHILDSTR(0) + RPSTR;
  case EXP           : return _T("exp("   ) + CHILDSTR(0) + RPSTR;
  case EXP10         : return _T("exp10(" ) + CHILDSTR(0) + RPSTR;
  case EXP2          : return _T("exp2("  ) + CHILDSTR(0) + RPSTR;
  case FAC           : return _T("fac("   ) + CHILDSTR(0) + RPSTR;
  case FLOOR         : return _T("floor(" ) + CHILDSTR(0) + RPSTR;
  case GAMMA         : return _T("gamma(" ) + CHILDSTR(0) + RPSTR;
  case GAUSS         : return _T("gauss(" ) + CHILDSTR(0) + RPSTR;
  case INVERF        : return _T("inverf(") + CHILDSTR(0) + RPSTR;
  case LN            : return _T("ln("    ) + CHILDSTR(0) + RPSTR;
  case LOG10         : return _T("log10(" ) + CHILDSTR(0) + RPSTR;
  case LOG2          : return _T("log2("  ) + CHILDSTR(0) + RPSTR;
  case NORM          : return _T("norm("  ) + CHILDSTR(0) + RPSTR;
  case PROBIT        : return _T("probit(") + CHILDSTR(0) + RPSTR;
  case SEC           : return _T("sec("   ) + CHILDSTR(0) + RPSTR;
  case SIGN          : return _T("sign("  ) + CHILDSTR(0) + RPSTR;
  case SIN           : return _T("sin("   ) + CHILDSTR(0) + RPSTR;
  case SINH          : return _T("sinh("  ) + CHILDSTR(0) + RPSTR;
  case SQR           : return _T("sqr("   ) + CHILDSTR(0) + RPSTR;
  case SQRT          : return _T("sqrt("  ) + CHILDSTR(0) + RPSTR;
  case TAN           : return _T("tan("   ) + CHILDSTR(0) + RPSTR;
  case TANH          : return _T("tanh("  ) + CHILDSTR(0) + RPSTR;
  case IIF           : return _T("if("    ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + COMMASTR + CHILDSTR(2) + RPSTR;
  case INDEXEDSUM    : return _T("sum(")     + CHILDSTR(0) + _T(" to ") + CHILDSTR(1) + _T(") ") + CHILDSTR(2);
  case INDEXEDPRODUCT: return _T("product(") + CHILDSTR(0) + _T(" to ") + CHILDSTR(1) + _T(") ") + CHILDSTR(2);
  default            : return _T("Unknown symbol:") + getSymbolName();
  }
}

}; // namespace Expr
