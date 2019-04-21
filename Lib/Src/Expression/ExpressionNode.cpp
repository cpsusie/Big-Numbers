#include "pch.h"
#include <Math/Expression/ExpressionParser.h>
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionNode::ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol) : m_tree(*tree), m_info(symbol) {
  m_tree.m_nodeTable.add(this);
}

String ExpressionNode::getNodeTypeName(ExpressionNodeType nt) { // static
  switch(nt) {
#define CASESTR(t) case NT_##t: return _T("NT_" #t);
  CASESTR(NUMBER   );
  CASESTR(BOOLCONST);
  CASESTR(VARIABLE );
  CASESTR(TREE     );
  CASESTR(BOOLEXPR );
  CASESTR(POLY     );
  CASESTR(ASSIGN   );
  CASESTR(STMTLIST );
  CASESTR(SUM      );
  CASESTR(PRODUCT  );
  CASESTR(ADDENT   );
  CASESTR(POWER    );
  default: return format(_T("Unknown nodetype:%d"),nt);
  }
#undef CASESTR
}

String ExpressionNode::getReturnTypeName(ExpressionReturnType rt) { // static
#define CASESTR(t) case EXPR_##t: return _T("EXPR_" #t);
  switch(rt) {
  CASESTR(NORETURNTYPE);
  CASESTR(RETURN_REAL );
  CASESTR(RETURN_BOOL );
  default: return format(_T("Unknown returntype:%d"),rt);
  }
#undef CASESTR
}

String PackedSyntaxNodeInfo::toString() const {
  String result;
  const TCHAR *del = NULL;
#ifdef ADD_ATTR
#undef ADD_ATTR
#endif
#define ADD_ATTR(a) if(m_##a) { if(del) result += del; else del = _T(","); result += _T(#a); }

  ADD_ATTR(marked);
  ADD_ATTR(breakPoint);
  ADD_ATTR(coefArrayConstant);
  ADD_ATTR(coefArrayChecked);
  ADD_ATTR(reduced);

#undef ADD_ATTR

  return result;
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

bool ExpressionNode::traverseExpression(ExpressionNodeHandler &handler) {
  handler.m_path.push(this);
  try {
    const bool result = traverseNode(handler);
    handler.m_path.pop();
    return result;
  } catch (...) {
    handler.m_path.pop();
    throw;
  }
}

class NameSelector : public ExpressionNodeSelector {
private:
  const String &m_name;
public:
  NameSelector(const String &name) : m_name(name) {
  }
  bool select(const ExpressionNode * const &n) {
    return n->isName() && (n->getName().equalsIgnoreCase(m_name));
  }
  AbstractSelector *clone() const {
    return new NameSelector(m_name);
  }
};

class NodeCounter : public ExpressionNodeHandler {
private:
  UINT                    m_count;
  ExpressionNodeSelector *m_selector;
public:
  NodeCounter(ExpressionNodeSelector *selector) : m_selector(selector) {
    m_count = 0;
  }
  bool handleNode(ExpressionNode *n) {
    if((m_selector == NULL) || m_selector->select(n)) m_count++;
    return true;
  }
  inline UINT getCount() const {
    return m_count;
  }
};

bool ExpressionNode::dependsOn(const String &name) const {
  return getNodeCount(&NameSelector(name)) > 0;
}

UINT ExpressionNode::getNodeCount(ExpressionNodeSelector *selector) const {
  NodeCounter nodeCounter(selector);
  ((ExpressionNode*)this)->traverseExpression(nodeCounter);
  return nodeCounter.getCount();
}

UINT ExpressionNode::getNodeCount(const ExpressionSymbolSet &validSymbolSet) const {
  return getNodeCount(&ExpressionNodeSymbolSelector(&validSymbolSet));
}

class MaxLevelFinder : public ExpressionNodeHandler {
private:
  UINT m_maxLevel;
public:
  MaxLevelFinder() : m_maxLevel(0) {
  }
  bool handleNode(ExpressionNode *n);
  UINT getMaxLevel() const {
    return m_maxLevel;
  }
};

bool MaxLevelFinder::handleNode(ExpressionNode *n) {
  const UINT level = getLevel();
  if(level > m_maxLevel) {
    m_maxLevel = level;
  }
  return true;
}

UINT ExpressionNode::getMaxTreeDepth() const {
  MaxLevelFinder handler;
  ((ExpressionNode*)this)->traverseExpression(handler);
  return handler.getMaxLevel();
}

String ExpressionNode::parenthesizedExpressionToString(const ExpressionNode *parent) const {
  if(needParentheses(parent)) {
    return _T("(") + toString() + _T(")");
  } else {
    return toString();
  }
}

bool ExpressionNode::containsFunctionCall() const {
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
  return getNodeCount(functionSet) > 0;
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

bool ExpressionNode::isLogarithmicPowExponent(int e) { // static
  return ::abs(e) <= 64;
}

bool ExpressionNode::isLogarithmicPowExponent() const {
  Number v;
  return isConstant(&v) && v.isInteger() && isLogarithmicPowExponent(getInt(v));
}

bool ExpressionNode::isLogarithmicRoot(const Rational &r) { // static
  return (::abs(r.getNumerator()) == 1) && isPow2(r.getDenominator()) && (r.getDenominator() <= 64);
}

bool ExpressionNode::isLogarithmicRoot() const {
  Rational r;
  return reducesToRational(&r) && isLogarithmicRoot(r);
}

TrigonometricMode ExpressionNode::getTrigonometricMode() const {
  return m_tree.getTrigonometricMode();
}

bool ExpressionNode::needParentheses(const ExpressionNode *parent) const {
  if(isName()) return false;
  if(isNumber()) return isNegativeNumber() && (parent->getSymbol() == POW) && (parent->child(0).node() == this);

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

int ExpressionNode::compare(const ExpressionNode *n) const {
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
void ExpressionNode::throwUnknownReturnTypeException(const TCHAR *method) const {
  throwException(_T("%s:Unexpected returnType in expression tree:%d")
                ,method, getReturnType());
}

void ExpressionNode::throwUnExpandableException() const {
  throwException(_T("Cannot expand expression %s"), toString().cstr());
}

void ExpressionNode::checkNodeType(const TCHAR *method, const ExpressionNode *n, ExpressionNodeType expectedNodeType) { // static
  if((n==NULL) || (n->getNodeType() != expectedNodeType)) {
    throwInvalidArgumentException(method
                                 ,_T("Expected nodetype:%s. Type=%s")
                                 ,getNodeTypeName(expectedNodeType).cstr()
                                 ,n?n->getNodeTypeName().cstr():_T("NULL")
                                 );
  }
}

void ExpressionNode::checkNodeType(const TCHAR *method, const ExpressionNode *n, const NodeTypeSet &validTypes) { // static
  if((n==NULL) || !validTypes.contains(n->getNodeType())) {
    throwInvalidArgumentException(method
                                 ,_T("Valid nodetypes:%s. Type=%s")
                                 ,validTypes.toString().cstr()
                                 ,n?n->getNodeTypeName().cstr():_T("NULL")
                                 );
  }
}

void ExpressionNode::checkReturnType(const TCHAR *method, const ExpressionNode *n, ExpressionReturnType expectedReturnType) { // static
  if((n==NULL) || (n->getReturnType() != expectedReturnType)) {
    throwInvalidArgumentException(method
                                 ,_T("Expected returntype:%s. Type=%s")
                                 ,getReturnTypeName(expectedReturnType).cstr()
                                 ,n?n->getReturnTypeName().cstr():_T("NULL")
                                 );
  }
}

void ExpressionNode::checkSymbol(const TCHAR *method, const ExpressionNode *n, ExpressionInputSymbol expectedSymbol) { // static
  if((n==NULL) || (n->getSymbol() != expectedSymbol)) {
    throwInvalidArgumentException(method
                                 ,_T("Expected symbol:%s. Symbol=%s")
                                 ,getSymbolName(expectedSymbol).cstr()
                                 ,n?n->getSymbolName().cstr():_T("NULL")
                                 );
  }
}

#ifdef CHECK_CONSISTENCY
bool ExpressionNode::isConsistentSymbolAndType() const {
  ExpressionNodeType type;
  switch(getSymbol()) {
  case NUMBER   : type = NT_NUMBER;    break;
  case NAME     : type = NT_VARIABLE;  break;
  case BOOLCONST: type = NT_BOOLCONST; break;
  case AND      :
  case OR       :
  case NOT      :
  case EQ       :
  case NE       :
  case LT       :
  case LE       :
  case GT       :
  case GE       : type = NT_BOOLEXPR;  break;
  case POLY     : type = NT_POLY;      break;
  case ASSIGN   : type = NT_ASSIGN;    break;
  case STMTLIST : type = NT_STMTLIST;  break;
  case SUM      : type = NT_SUM;       break;
  case ADDENT   : type = NT_ADDENT;    break;
  case PRODUCT  : type = NT_PRODUCT;   break;
  case POW      : type = NT_POWER;     break;
  default       : type = NT_TREE;      break;
  }

  if(getNodeType() != type) {
    return false;
  }
  return true;
}

class ConsistencyCheck : public ExpressionNodeHandler {
private:
  const ExpressionNode *m_failureNode;
  UINT                  m_nodesVisited;
public:
  ConsistencyCheck() : m_failureNode(NULL), m_nodesVisited(0) {
  }
  bool handleNode(ExpressionNode *n) {
    m_nodesVisited++;
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
  UINT getNodesVisited() const {
    return m_nodesVisited;
  }
};

UINT ExpressionNode::checkIsConsistent() const {
  ConsistencyCheck checker;
  ((ExpressionNode*)(this))->traverseExpression(checker);
  if(!checker.isOK()) {
    throwException(_T("Inconsistent ExpressionNode"));
  }
  return checker.getNodesVisited();
}

#endif // CHECK_CONSISTENCY

}; // namespace Expr
