#include "pch.h"
#include <Math/Expression/Expression.h>

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr) : ExpressionNode(tree, symbol) {
  initChildArray(argptr);
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, const ExpressionNodeArray &childArray) : ExpressionNode(tree, symbol) {
  m_childArray.setCapacity(childArray.size());
  for(size_t i = 0; i < childArray.size(); i++) {
    m_childArray.add((ExpressionNode*)childArray[i]);
  }
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, const ExpressionNodeTree *src) : ExpressionNode(tree, src->getSymbol()) {
  const ExpressionNodeArray &sa = src->getChildArray();
  m_childArray.setCapacity(sa.size());
  for(size_t i = 0; i < sa.size(); i++) {
    m_childArray.add(sa[i]->clone(tree));
  }
}

ExpressionNodeTree::ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, ...) : ExpressionNode(tree, symbol) {
  va_list argptr;
  va_start(argptr, symbol);
  initChildArray(argptr);
  va_end(argptr);
}

static int countVargs(va_list argptr) {
  int count = 0;
  for(ExpressionNode *p = va_arg(argptr, ExpressionNode*); p; p = va_arg(argptr, ExpressionNode*)) {
    count++;
  }
  return count;
}

void ExpressionNodeTree::initChildArray(va_list argptr) {
  int count = countVargs(argptr);;
  m_childArray.clear(count);
  for (ExpressionNode *p = va_arg(argptr, ExpressionNode*); p; p = va_arg(argptr, ExpressionNode*)) {
    m_childArray.add(p);
  }
}

ExpressionNode *ExpressionNodeTree::expand() {
  if(!isExpandable()) {
    return this;
  }
  switch(getSymbol()) {
  case POW:
    { Expression     *expr = getExpr();
      ExpressionNode *expo = right();
      Rational        expoR;
      if(!expr->reducesToRationalConstant(expo, &expoR)) {
        return false;
      }
      return expr->expandPower(left(), expoR);
    }
  default :
    return this;
  }
}

bool ExpressionNodeTree::isExpandable() {
  switch(getSymbol()) {
  case POW:
    { Expression     *expr = getExpr();
      ExpressionNode *expo = right();
      Rational        expoR;
      if(!expr->reducesToRational(expo, &expoR) || (abs(expoR.getNumerator()) <= 1)) {
        return false;
      }
      const ExpressionNode *base = left();
      switch(base->getSymbol()) {
      case MINUS:
        if(base->isUnaryMinus()) return false;
        // NB continue case;
      case SUM  :
      case PLUS :
        return true;
      default   :
        return false;
      }
    }
  default:
    return false;
  }
}

int ExpressionNodeTree::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
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
    const ExpressionNodeArray &a1 = getChildArray();
    const ExpressionNodeArray &a2 = n->getChildArray();
    for(size_t i = 0; i < a1.size(); i++) { // the have the same symbol => a1.size() == a2.size()
      const int c = a1[i]->compare(a2[i]);
      if(c) return c;
    }
  }
  return 0;
}

ExpressionNode *ExpressionNodeTree::clone(ParserTree *tree) const {
  return new ExpressionNodeTree(tree, this);
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

bool ExpressionNodeTree::isConstant() const {
  switch(getSymbol()) {
  case POW:
    if(right()->isZero()) {
      return true;
    }
    break;
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    { const ExpressionNode     *startAssignment = child(0);
      const ExpressionVariable &loopVar         = startAssignment->left()->getVariable();
      const ExpressionNode     *beginExpr       = startAssignment->right();
      const ExpressionNode     *endExpr         = child(1);
      const ExpressionNode     *expr            = child(2);
      return beginExpr->isConstant() && endExpr->isConstant() && !exprDependsOnNonConstantNames(expr, loopVar);
    }
  }
  return m_childArray.isConstant();
}

bool ExpressionNodeTree::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;
  const ExpressionNodeArray &a = getChildArray();
  level++;
  for(size_t i = 0; i < a.size(); i++) {
    if(!a[i]->traverseExpression(handler, level)) return false;
  }
  return true;
}


void ExpressionNodeTree::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(size_t i = 0; i < m_childArray.size(); i++) {
    m_childArray[i]->dumpNode(s, level+1);
  }
}

#define CHILDSTR(i)    m_childArray[i]->toString()
#define CHILDPARSTR(i) m_childArray[i]->parenthesizedExpressionToString(this)

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
  case AND           : return CHILDPARSTR(0) + _T(" and ") + CHILDPARSTR(1);
  case OR            : return CHILDPARSTR(0) + _T(" or " ) + CHILDPARSTR(1);
  case NOT           : return _T("not "      ) + CHILDPARSTR(0);
  case ROOT          : return _T("root("     ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case BINOMIAL      : return _T("binomial(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case CHI2DENS      : return _T("chi2dens(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case CHI2DIST      : return _T("chi2dist(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case LINCGAMMA     : return _T("lincgamma(") + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case MAX           : return _T("max("      ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case MIN           : return _T("min("      ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case RAND          : return _T("rand("     ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case NORMRAND      : return _T("normrand(" ) + CHILDSTR(0) + COMMASTR + CHILDSTR(1) + RPSTR;
  case EQ            : return CHILDSTR(0) + _T(" == ") + CHILDSTR(1);
  case NE            : return CHILDSTR(0) + _T(" <> ") + CHILDSTR(1);
  case LE            : return CHILDSTR(0) + _T(" <= ") + CHILDSTR(1);
  case LT            : return CHILDSTR(0) + _T(" < " ) + CHILDSTR(1);
  case GE            : return CHILDSTR(0) + _T(" >= ") + CHILDSTR(1);
  case GT            : return CHILDSTR(0) + _T(" > " ) + CHILDSTR(1);
  case ABS           : return _T("abs("   ) + CHILDSTR(0) + RPSTR;
  case ACOS          : return _T("acos("  ) + CHILDSTR(0) + RPSTR;
  case ACOSH         : return _T("acosh(" ) + CHILDSTR(0) + RPSTR;
  case ACOT          : return _T("acot("  ) + CHILDSTR(0) + RPSTR;
  case ACSC          : return _T("acsc("  ) + CHILDSTR(0) + RPSTR;
  case ASEC          : return _T("asec("  ) + CHILDSTR(0) + RPSTR;
  case ASIN          : return _T("asin("  ) + CHILDSTR(0) + RPSTR;
  case ASINH         : return _T("asinh(" ) + CHILDSTR(0) + RPSTR;
  case ATAN          : return _T("atan("  ) + CHILDSTR(0) + RPSTR;
  case ATANH         : return _T("atanh(" ) + CHILDSTR(0) + RPSTR;
  case CEIL          : return _T("ceil("  ) + CHILDSTR(0) + RPSTR;
  case COS           : return _T("cos("   ) + CHILDSTR(0) + RPSTR;
  case COSH          : return _T("cosh("  ) + CHILDSTR(0) + RPSTR;
  case COT           : return _T("cot("   ) + CHILDSTR(0) + RPSTR;
  case CSC           : return _T("csc("   ) + CHILDSTR(0) + RPSTR;
  case ERF           : return _T("erf("   ) + CHILDSTR(0) + RPSTR;
  case EXP           : return _T("exp("   ) + CHILDSTR(0) + RPSTR;
  case FAC           : return _T("fac("   ) + CHILDSTR(0) + RPSTR;
  case FLOOR         : return _T("floor(" ) + CHILDSTR(0) + RPSTR;
  case GAMMA         : return _T("gamma(" ) + CHILDSTR(0) + RPSTR;
  case GAUSS         : return _T("gauss(" ) + CHILDSTR(0) + RPSTR;
  case INVERF        : return _T("inverf(") + CHILDSTR(0) + RPSTR;
  case LN            : return _T("ln("    ) + CHILDSTR(0) + RPSTR;
  case LOG10         : return _T("log10(" ) + CHILDSTR(0) + RPSTR;
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
  case SEMI          : return m_childArray[0]->statementListToString() + CHILDSTR(1);
  case ASSIGN        : return CHILDSTR(0) + _T(" = ") + CHILDSTR(1);
  case RETURNREAL    : return CHILDSTR(0);
  case RETURNBOOL    : return CHILDSTR(0);
  case INDEXEDSUM    : return _T("sum(")     + CHILDSTR(0) + _T(" to ") + CHILDSTR(1) + _T(") ") + CHILDSTR(2);
  case INDEXEDPRODUCT: return _T("product(") + CHILDSTR(0) + _T(" to ") + CHILDSTR(1) + _T(") ") + CHILDSTR(2);
  default            : return _T("Unknown symbol:") + getSymbolName();
  }
}
