#include "pch.h"
#include <Math/Expression/ExpressionParser.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

namespace Expr {

ExpressionNode::ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol) : m_tree(*tree), m_info(symbol) {
  m_tree.m_nodeTable.add(this);
}

Exception ExpressionNode::createAttributeNotSupportedException(const char *attribute) const {
  const String attr = attribute;
  return Exception(format(_T("Attribute %s not defined for syntaxNode with symbol=%s"), attr.cstr(), getSymbolName().cstr()));
}

/*static*/ String ExpressionNode::getSymbolName(ExpressionInputSymbol symbol) {
  const ParserTables &tables = ExpressionParser::getTables();
  if((UINT)symbol >= tables.getSymbolCount()) {
    return format(_T("Unknown symbol (=%u)"), symbol);
  }
  return tables.getSymbolName(symbol);
}

Real &ExpressionNode::getValueRef() const {
  return m_tree.getValueRef(getValueIndex());
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

ExpressionSymbolSet::ExpressionSymbolSet() : BitSet(ParserTree::getTerminalCount()) {
}

// terminate list with EOI
ExpressionSymbolSet::ExpressionSymbolSet(ExpressionInputSymbol s1,...) : BitSet(ParserTree::getTerminalCount()) {
  va_list argptr;
  va_start(argptr, s1);
  add(s1);
  for(ExpressionInputSymbol s=va_arg(argptr, ExpressionInputSymbol); s != EOI; s = va_arg(argptr, ExpressionInputSymbol)) {
    add(s);
  }
  va_end(argptr);
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

static ExpressionNodeSelector *getBuiltInFunctionSelector() {
  // All functions in this array use call to evaluate in compiled code
  static const ExpressionSymbolSet functionSet(
      MOD      , POW      , ROOT     , SIN      , COS      , TAN      , COT      , CSC
    , SEC      , ASIN     , ACOS     , ATAN     , ATAN2    , ACOT     , ACSC     , ASEC
    , COSH     , SINH     , TANH     , ACOSH    , ASINH    , ATANH    , LN       , LOG10
    , LOG2     , EXP      , EXP10    , EXP2     , FLOOR    , CEIL     , BINOMIAL , GAMMA
    , GAUSS    , FAC      , NORM     , PROBIT   , ERF      , INVERF   , SIGN     , MAX
    , MIN      , HYPOT    , RAND     , NORMRAND , POLY     , CHI2DIST , CHI2DENS , LINCGAMMA
    , EOI
  );
  static ExpressionNodeSymbolSelector selector(&functionSet);
  return &selector;
}

bool ExpressionNode::containsFunctionCall() const {
  return getNodeCount(getBuiltInFunctionSelector()) > 0;
}

bool ExpressionNode::isBinaryOperator(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(POW  , ROOT , PROD , QUOT , PLUS , MINUS, MOD  , EOI);
  return set.contains(symbol);
}

bool ExpressionNode::isTrigonomtricFunction(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(SIN  , COS  , SEC  , CSC  , TAN
                                      ,COT  , ASIN , ACOS , ASEC , ACSC
                                      ,ATAN , ACOT , SINH , COSH , TANH
                                      ,ASINH, ACOSH, ATANH, EOI );
  return set.contains(symbol);
}

bool ExpressionNode::isSymmetricFunction(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(ABS  , COS  ,COSH  ,GAUSS , SEC ,  SQR ,  EOI);
  return set.contains(symbol);
}

bool ExpressionNode::isAsymmetricFunction(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(ACSC , ASIN , ASINH, ATAN  , ATANH
                                      ,COT  , CSC  , ERF  , INVERF, SIGN
                                      ,SIN  , SINH , TAN  , TANH  , EOI );
  return set.contains(symbol);
}

typedef struct {
  ExpressionInputSymbol m_f,m_invf;
} FunctionInverseFunctionPair;

class InverseFunctionMap : public CompactSymbolHashMap<ExpressionInputSymbol> {
public:
  InverseFunctionMap();
};

InverseFunctionMap::InverseFunctionMap() {
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

  for(size_t i = 0; i < ARRAYSIZE(table); i++) {
    const FunctionInverseFunctionPair &fp = table[i];
    put(fp.m_f, fp.m_invf);
  }
}

ExpressionInputSymbol ExpressionNode::getInverseFunction(ExpressionInputSymbol symbol) { // static
  static const InverseFunctionMap map;
  const ExpressionInputSymbol *s = map.get(symbol);
  return s ? *s : EOI;
}

bool ExpressionNode::isSymmetricExponent() const {
  return isRational() && ::isSymmetricExponent(getRational());
}

bool ExpressionNode::isAsymmetricExponent() const {
  return isRational() && ::isAsymmetricExponent(getRational());
}

TrigonometricMode ExpressionNode::getTrigonometricMode() const {
  return m_tree.getTrigonometricMode();
}

bool ExpressionNode::needParentheses(const ExpressionNode *parent) const {
  if(isName()) return false;
  if(isNumber()) return isNegative() && (parent->getSymbol() == POW) && (parent->child(0).node() == this);

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

bool ExpressionNode::isBooleanOperator(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(AND,OR,NOT,EQ,NE,LT,LE,GT,GE,EOI);
  return set.contains(symbol);
}

bool ExpressionNode::isCompareOperator(ExpressionInputSymbol symbol) { // static
  static const ExpressionSymbolSet set(EQ,NE,LT,LE,GT,GE,EOI);
  return set.contains(symbol);
}

ExpressionInputSymbol ExpressionNode::reverseComparator(ExpressionInputSymbol symbol) { // static
  switch(symbol) {
  case EQ :
  case NE : return symbol;
  case LE : return GE;
  case LT : return GT;
  case GE : return LE;
  case GT : return LT;
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("symbol=%d"), symbol);
  return EQ;
}

ExpressionInputSymbol ExpressionNode::negateComparator(ExpressionInputSymbol symbol) { // static
  switch(symbol) {
  case EQ : return NE;
  case NE : return EQ;
  case LE : return GT;
  case LT : return GE;
  case GE : return LT;
  case GT : return LE;
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("symbol=%d"), symbol);
  return EQ;
}

class SymbolOrderMap : public CompactSymbolHashMap<int> {
public:
  SymbolOrderMap();
  inline int compare(ExpressionInputSymbol s1, ExpressionInputSymbol s2) const {
    return *get(s1) - *get(s2);
  }
};

SymbolOrderMap::SymbolOrderMap() {
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
  for(UINT i = 0; i < ARRAYSIZE(symbols); i++) {
    put(symbols[i], order++);
  }
  const UINT termCount = ParserTree::getTerminalCount();
  for(UINT i = 1; i < termCount; i++) {
    if(get((ExpressionInputSymbol)i) == NULL) {
      put((ExpressionInputSymbol)i, order++);
    }
  }
}

int ExpressionNode::compare(ExpressionNode *n) {
  static const SymbolOrderMap orderMap;
  const ExpressionInputSymbol s1 = getSymbol();
  const ExpressionInputSymbol s2 = n->getSymbol();
  return (s1 != s2) ? orderMap.compare(s1, s2) : 0;
}

String &ExpressionNode::addLeftMargin(String &s, int level) { // static
  s += format(_T("%*.*s%2d:"), level*2, level*2, EMPTYSTRING, level);
  return s;
}

void ExpressionNode::throwInvalidSymbolForTreeMode(const TCHAR *method) const {
  throwException(_T("%s:Invalid symbol in tree form %s:<%s>")
                ,method, getTree().getTreeFormName().cstr(),  getSymbolName().cstr());
}

void ExpressionNode::throwUnknownSymbolException(const TCHAR *method) const {
  throwException(_T("%s:Unexpected symbol in expression tree:%s")
                ,method, getSymbolName().cstr());
}

void ExpressionNode::throwUnknownNodeTypeException(const TCHAR *method) const {
  throwException(_T("%s:Unexpected nodeType in expression tree:%d")
                ,method, getNodeType());
}

#ifdef CHECK_CONSISTENCY
bool ExpressionNode::isConsistentSymbolAndType() const {
  ExpressionNodeType type1, type2;
  switch(getSymbol()) {
  case NUMBER  : type1 = type2  = NT_NUMBER;    break;
  case TYPEBOOL: type1 = type2  = NT_BOOLCONST; break;
  case NAME    : type1 = type2  = NT_VARIABLE;  break;
  case AND     :
  case OR      :
  case NOT     :
  case EQ      :
  case NE      :
  case LT      :
  case LE      :
  case GT      :
  case GE      : type1 = type2  = NT_BOOLEXPR;  break;
  case POLY    : type1 = type2  = NT_POLY;      break;
  case ASSIGN  : type1 = type2  = NT_ASSIGN;    break;
  case STMTLIST: type1 = type2  = NT_STMTLIST;  break;
  case SUM     : type1 = type2  = NT_SUM;       break;
  case PRODUCT : type1 = type2  = NT_PRODUCT;   break;
  case POW     : type1 = NT_FACTOR;  type2  = NT_TREE;  break;
  default      : type1 = type2  = NT_TREE;      break;
  }

  if((getNodeType() != type1) && (getNodeType() != type2)) {
    return false;
  }
  return true;
}

class ConsistencyCheck : public ExpressionNodeHandler {
  const ExpressionNode *m_failureNode;
public:
  ConsistencyCheck() : m_failureNode(NULL) {
  }
  bool handleNode(ExpressionNode *n, int level) {
    if(!n->isConsistentSymbolAndType()) {
      m_failureNode = n;
      return false;
    }
    return true;
  }
  bool isOK() const {
    return m_failureNode == NULL;
  }
  const ExpressionNode *getInconsistentNode() const {
    return m_failureNode;
  }
};

void ExpressionNode::checkIsConsistent() const {
  ConsistencyCheck checker;
  ((ExpressionNode*)(this))->traverseExpression(checker,0);
  if(!checker.isOK()) {
    throwException(_T("Inconsistent ExpressionNode"));
  }
}

#endif // CHECK_CONSISTENCY

}; // namespace Expr
