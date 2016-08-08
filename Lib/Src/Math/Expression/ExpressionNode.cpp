#include "pch.h"
#include <Math/Expression/ExpressionParser.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

ExpressionNode::ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol) : m_tree(*tree), m_info(symbol) {
  m_tree.m_nodeTable.add(this);
}

Exception ExpressionNode::createAttributeNotSupportedException(const char *attribute) const {
  const String attr = attribute;
  return Exception(format(_T("Attribute %s not defined for syntaxNode with symbol=%s"), attr.cstr(), getSymbolName().cstr()));
}

const Expression *ExpressionNode::getExpr() const {
  return (const Expression*)getTree();
}

Expression *ExpressionNode::getExpr() {
  return (Expression*)getTree();
}

String ExpressionNode::getSymbolName() const {
  const ExpressionInputSymbol sym = getSymbol();
  if((unsigned int)sym >= ExpressionTables->getSymbolCount()) {
    return format(_T("Unknown symbol (=%u)"), sym);
  }
  return ExpressionTables->getSymbolName(sym);
}

Real &ExpressionNode::getValue() const {
  return m_tree.m_valueTable[getValueIndex()];
}

class NameChecker : public ExpressionNodeHandler {
private:
  const String &m_name;
  bool          m_nameFound;
public:
  NameChecker(const String &name) : m_name(name) {
    m_nameFound = false;
  }
  bool handleNode(ExpressionNode *n, int level) {
    if(n->isName() && (n->getName() == m_name)) {
      m_nameFound = true;
      return false;
    }
    return true;
  }
  bool isFound() const {
    return m_nameFound;
  }
};

ExpressionSymbolSet::ExpressionSymbolSet() : BitSet(ExpressionTables->getTerminalCount()) {
}

class NodeCounter : public ExpressionNodeHandler {
private:
  int                     m_count;
  ExpressionNodeSelector *m_selector;
public:
  NodeCounter(ExpressionNodeSelector *selector) : m_selector(selector) {
    m_count = 0;
  }
  bool handleNode(ExpressionNode *n, int level) {
    if((m_selector == NULL) || m_selector->select(n)) m_count++;
    return true;
  }
  int getCount() const {
    return m_count;
  }
};

bool ExpressionNode::dependsOn(const String &name) const {
  NameChecker nameChecker(name);
  ((ExpressionNode*)this)->traverseExpression(nameChecker,0);
  return nameChecker.isFound();
}

int ExpressionNode::getNodeCount(ExpressionNodeSelector *selector) const {
  NodeCounter nodeCounter(selector);
  ((ExpressionNode*)this)->traverseExpression(nodeCounter, 0);
  return nodeCounter.getCount();
}

class MaxLevelFinder : public ExpressionNodeHandler {
private:
  int m_maxLevel;
public:
  MaxLevelFinder() : m_maxLevel(0) {
  }
  bool handleNode(ExpressionNode *n, int level);
  int getMaxLevel() const {
    return m_maxLevel;
  }
};

bool MaxLevelFinder::handleNode(ExpressionNode *n, int level) {
  if(level > m_maxLevel) {
    m_maxLevel = level;
  }
  return true;
}

int ExpressionNode::getMaxTreeDepth() const {
  MaxLevelFinder handler;
  ((ExpressionNode*)this)->traverseExpression(handler, 0);
  return handler.getMaxLevel();
}

String ExpressionNode::parenthesizedExpressionToString(const ExpressionNode *parent) const {
  if(needParentheses(parent)) {
    return _T("(") + toString() + _T(")");
  } else {
    return toString();
  }
}

String ExpressionNode::statementListToString() const {
  String result;
  ExpressionNodeArray list = getStatementList((ExpressionNode*)this);
  for(size_t i = 0; i < list.size(); i++) {
    result += list[i]->toString();
    result += _T(";\n");
  }
  return result;
}

static ExpressionNodeSelector *getBuiltInFunctionSelector() {
  // all functions in this array use call to evaluate in cmopiled code
  static const ExpressionInputSymbol builtInSymbols[] = {
     MOD, POW, ROOT, SIN, COS, TAN, COT, CSC
    ,SEC, ASIN, ACOS, ATAN, ACOT, ACSC, ASEC, COSH
    ,SINH, TANH, ACOSH, ASINH, ATANH, LN, LOG10, EXP
    ,SQR, SQRT, ABS, FLOOR, CEIL, BINOMIAL, GAMMA, GAUSS
    ,FAC, NORM, PROBIT, ERF, INVERF, SIGN, MAX, MIN
    ,RAND, NORMRAND, POLY
  };
  static bool                         initDone = false;
  static ExpressionSymbolSet          functionSet;
  static ExpressionNodeSymbolSelector selector(&functionSet);
  if (!initDone) {
    for (int i = 0; i < ARRAYSIZE(builtInSymbols); i++) {
      functionSet.add(builtInSymbols[i]);
    }
    initDone = true;
  }
  return &selector;
}

bool ExpressionNode::containsFunctionCall() const {
  return getNodeCount(getBuiltInFunctionSelector()) > 0;
}

bool ExpressionNode::isBinaryOperator() const {
  switch(getSymbol()) {
  case POW  :
  case ROOT :
  case PROD :
  case QUOT :
  case PLUS :
  case MINUS:
  case MOD  :
    return true;
  default:
    return false;
  }
}

bool ExpressionNode::isBooleanOperator() const {
  switch(getSymbol()) {
  case OR :
  case AND:
  case NOT:
  case EQ :
  case GE :
  case GT :
  case LE :
  case LT :
  case NE :
    return true;
  default:
    return false;
  }
}

bool ExpressionNode::isTrigonomtricFunction() const {
  switch(getSymbol()) {
  case SIN:
  case COS:
  case SEC:
  case CSC:
  case TAN:
  case COT:
  case ASIN:
  case ACOS:
  case ASEC:
  case ACSC:
  case ATAN:
  case ACOT:
  case SINH:
  case COSH:
  case TANH:
  case ASINH:
  case ACOSH:
  case ATANH:
    return true;
  default:
    return false;
  }
}

bool ExpressionNode::isSymmetricFunction() const {
  switch(getSymbol()) {
  case ABS     :
  case COS     :
  case COSH    :
  case GAUSS   :
  case SEC     :
  case SQR     :
    return true;
  default      :
    return false;
  }
}

bool ExpressionNode::isAsymmetricFunction() const {
  switch(getSymbol()) {
  case ACSC    :
  case ASIN    :
  case ASINH   :
  case ATAN    :
  case ATANH   :
  case COT     :
  case CSC     :
  case ERF     :
  case INVERF  :
  case SIGN    :
  case SIN     :
  case SINH    :
  case TAN     :
  case TANH    :
    return true;
  default      :
    return false;
  }
}

bool ExpressionNode::isSymmetricExponent() const {
  return isRational() && ::isSymmetricExponent(getRational());
}

bool ExpressionNode::isAsymmetricExponent() const {
  return isRational() && ::isAsymmetricExponent(getRational());
}

bool ExpressionNode::needParentheses(const ExpressionNode *parent) const {
  if(isName()) return false;
  if(isNumber()) return isNegative() && (parent->getSymbol() == POW) && (parent->left() == this);

  const int precedence       = getPrecedence();
  const int parentPrecedence = parent->getPrecedence();
  if(precedence < parentPrecedence) {
    return true;
  } else if(precedence > parentPrecedence) {
    return false;
  } else {
    switch(parent->getSymbol()) {
    case MINUS :
      if(parent->isUnaryMinus()) {
        return true;
      }
      // continue case
    case QUOT  :
    case MOD   :
    case POW   :
      return parent->right() == this; // these left-associative operators need parentheses, when right child has same precedence
    default:
      return false;
    }
  }
}

int ExpressionNode::getPrecedence() const {
  switch(getSymbol()) {
  case OR     : return 1;
  case AND    : return 2;
  case NOT    : return 3;
  case PLUS   :
  case MINUS  :
  case SUM    : return 4;
  case PROD   :
  case QUOT   :
  case MOD    :
  case PRODUCT: return 5;
  case POW    : return 6;
  default     : return 7;
  }
}

bool ExpressionNode::isCoefficientArrayConstant() const {
  if(!m_info.m_coefChecked) {
    m_info.m_coefficientsConstant = getCoefficientArray().isConstant() ? 1 : 0;
    m_info.m_coefChecked          = 1;
  }
  return m_info.m_coefficientsConstant ? true : false;
}

void SymbolOrderMap::init() {
  static const ExpressionInputSymbol symbols[] = {
    POW
   ,PROD
   ,QUOT
   ,MOD
   ,PLUS
   ,MINUS
   ,NAME
   ,NUMBER
  };

  int order = 1;
  for(int i = 0; i < ARRAYSIZE(symbols); i++) {
    put(symbols[i], order++);
  }
  for(unsigned int i = 1; i < ExpressionTables->getTerminalCount(); i++) {
    if(get((ExpressionInputSymbol)i) == NULL) {
      put((ExpressionInputSymbol)i, order++);
    }
  }
  m_initDone = true;
}

int SymbolOrderMap::compare(ExpressionInputSymbol s1, ExpressionInputSymbol s2) {
  if(!m_initDone) init();
  return *get(s1) - *get(s2);
}

typedef struct {
  ExpressionInputSymbol m_f,m_invf;
} FunctionInverseFunctionPair;

void InverseFunctionMap::init() {
  static const FunctionInverseFunctionPair table[] = {
    SIN   , ASIN
   ,COS   , ACOS
   ,TAN   , ATAN
   ,COT   , ACOT
   ,CSC   , ACSC
   ,SEC   , ASEC
   ,SINH  , ASINH
   ,ASINH , SINH
   ,COSH  , ACOSH
   ,TANH  , ATANH
   ,ATANH , TANH
   ,NORM  , PROBIT
   ,PROBIT, NORM
   ,ERF   , INVERF
   ,INVERF, ERF
  };

  for(int i = 0; i < ARRAYSIZE(table); i++) {
    const FunctionInverseFunctionPair &fp = table[i];
    put(fp.m_f, fp.m_invf);
  }
  m_initDone = true;
}

ExpressionInputSymbol InverseFunctionMap::getInverse(ExpressionInputSymbol symbol) {
  if(!m_initDone) init();
  const ExpressionInputSymbol *s = get(symbol);
  return s ? *s : (ExpressionInputSymbol)0;
}

SymbolOrderMap     ExpressionNode::s_orderMap;
InverseFunctionMap ExpressionNode::s_inverseFunctionMap;

int ExpressionNode::compare(ExpressionNode *n) {
  const ExpressionInputSymbol s1 = getSymbol();
  const ExpressionInputSymbol s2 = n->getSymbol();
  return (s1 != s2) ? s_orderMap.compare(s1, s2) : 0;
}

String &ExpressionNode::addLeftMargin(String &s, int level) { // static
  s += format(_T("%*.*s%2d:"), level*2, level*2, _T(""), level);
  return s;
}

ExpressionNode *ExpressionNodeArray::toTree(ExpressionInputSymbol delimiter) const {
  ExpressionNode *result = (*this)[0];
  ParserTree *tree = result->getTree();
  for(size_t i = 1; i < size(); i++) {
    result = tree->binaryExpression(delimiter, result, (*this)[i]);
  }
  return result;
}

bool ExpressionNodeArray::isConstant() const {
  for(size_t i = 0; i < size(); i++) {
    if(!(*this)[i]->isConstant()) return false;
  }
  return true;
}

String ExpressionNodeArray::toString() const {
  if(size() == 0) {
    return _T("");
  } else {
    String result = format(_T("(%s)"), (*this)[0]->toString().cstr());
    for(size_t i = 1; i < size(); i++) {
      result += format(_T(",(%s)"), (*this)[i]->toString().cstr());
    }
    return result;
  }
}
