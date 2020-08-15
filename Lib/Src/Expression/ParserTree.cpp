#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionSymbolTable.h>
#include "Math/Expression/ExpressionParser.h"

namespace Expr {

using namespace std;

#if defined(CHECK_CONSISTENCY)
void ParserTree::checkIsConsistent() const {
  try {
    if(!isEmpty()) {
      getRoot()->checkIsConsistent();
      m_ops->checkTreeFormConsistent(this);
    }
  } catch(Exception e) {
    int fisk = 1;
    throw;
  } catch(...) {
    int fisk = 1;
    throw;
  }
}
#define CHECKTREE_ISCONSISTENT(tree) { (tree).checkIsConsistent(); }
#else // CHECK_CONSISTENCY
#define CHECKTREE_ISCONSISTENT(tree)
#endif

ParserTree::ParserTree(Expression *expr, const String &str)
: m_expression(*expr)
{
  init(PS_EMPTY,0);
  m_root              = NULL;
  m_ops               = NodeOperators::s_stdForm;
  parse(str);
}

ParserTree::ParserTree(Expression *expression, const ExpressionNode *root)
: m_expression(*expression)
{
  if(root == NULL) {
    init(PS_EMPTY, 0);
    m_root   = NULL;
    m_ops    = NodeOperators::s_stdForm;
  } else {
    ParserTree &src = root->getTree();
    init(src.getState(), src.getReduceIteration());
    m_errors = src.m_errors;
    m_root   = root->clone(this);
    m_ops    = src.m_ops;
    buildSymbolTable(&src.getSymbolTable().getAllVariables());
  }
}

void ParserTree::init(ParserTreeState      state
                     ,UINT                 reduceIteration) {
  if(m_expression.m_tree != NULL) {
    throwException(_T("%s:m_expression.m_tree already set to another ParserTree"),__TFUNCTION__);
  }
  m_expression.m_tree = this;
  m_state             = state;
  m_reduceIteration   = reduceIteration;
  resetSimpleConstants();
}

ParserTree &ParserTree::operator=(const ParserTree &src) {
  if(&src == this) {
    return *this;
  }
  CHECKTREE_ISCONSISTENT(src)
  releaseAll();
  m_errors = src.m_errors;
  setTreeForm(         src.getTreeForm());
  setRoot(             src.isEmpty() ? NULL : src.getRoot()->clone(this));
  setReduceIteration(  src.getReduceIteration());
  setState(            src.getState());
  CHECKTREE_ISCONSISTENT(*this)
  return *this;
}

ParserTree::~ParserTree() {
  releaseAll();
  m_expression.m_tree = NULL;
}

void ParserTree::parse(const String &expr) {
  setTreeForm(TREEFORM_STANDARD);
  LexStringStream    stream(expr);
  ExpressionLex      lex(&stream);
  ExpressionParser   parser(*this, &lex);
  lex.setParser(&parser);
  parser.parse();
  if(isOk()) {
    buildSymbolTable();
    setState(PS_COMPILED);
  }
}

bool ParserTree::isValidName(const String &s) { // static
  LexStringStream stream(s);
  ExpressionLex lex(&stream);
  if(lex.getNextLexeme() != NAME) return false;
  if(lex.getNextLexeme() != EOI) return false;
  return true;
}

const ParserTables &ParserTree::getParserTables() { // static
  return ExpressionParser::getTables();
}

UINT ParserTree::getTerminalCount() { // static
  return getParserTables().getTerminalCount();
};

String ParserTree::getSymbolName(ExpressionInputSymbol symbol) { // static
  const ParserTables &tables = getParserTables();
  if((UINT)symbol >= tables.getSymbolCount()) {
    return format(_T("Unknown symbol (=%u)"), symbol);
  }
  return tables.getSymbolName(symbol);
}

void ParserTree::buildSymbolTable(const ExpressionVariableArray *oldVariables) {
  getSymbolTable().create(oldVariables);
}

void ParserTree::setRoot(ExpressionNode *n) {
  setProperty(PP_ROOT, m_root, n);
  CHECKTREE_ISCONSISTENT(*this);
}

void ParserTree::setState(ParserTreeState newState) {
  setProperty(PP_STATE, m_state, newState);
}

void ParserTree::setReduceIteration(UINT iteration) {
  setProperty(PP_REDUCEITERATION, m_reduceIteration, iteration);
}

bool ParserTree::equal(const ParserTree &tree) const {
  return Expr::equal(getRoot(), tree.getRoot());
}

bool ParserTree::equalMinus(const ParserTree &tree) const {
  return Expr::equalMinus(getRoot(), tree.getRoot());
}

ParserTree &ParserTree::setTreeForm(ParserTreeForm treeForm) {
  const ParserTreeForm oldForm = getTreeForm();
  if(treeForm != oldForm) {
    CHECKTREE_ISCONSISTENT(*this);
    const ExpressionVariableArray oldVariables = getSymbolTable().getAllVariables();
    switch(treeForm) {
    case TREEFORM_STANDARD : setRoot(toStandardForm( getRoot()).node()); break;
    case TREEFORM_CANONICAL: setRoot(toCanonicalForm(getRoot()).node()); break;
    case TREEFORM_NUMERIC  : setRoot(toNumericForm(  getRoot()).node()); break;
    default                :
      throwInvalidArgumentException(__TFUNCTION__, _T("form=%d"), treeForm);
    }
    pruneUnusedNodes();
    buildSymbolTable(&oldVariables);
    CHECKTREE_ISCONSISTENT(*this);
    notifyPropertyChanged(PP_TREEFORM, &oldForm, &treeForm);
  }
  return *this;
}


String ParserTree::getTreeFormName(ParserTreeForm treeForm) { // static
  switch(treeForm) {
  case TREEFORM_STANDARD : return _T("standard");
  case TREEFORM_CANONICAL: return _T("canonical");
  case TREEFORM_NUMERIC  : return _T("numeric");
  default                :
    throwInvalidArgumentException(__TFUNCTION__, _T("treeForm=%d"), treeForm);
    return EMPTYSTRING;
  }
}

String ParserTree::getStateName(ParserTreeState state) { // static
#define CASESTR(s) case PS_##s: return _T(#s)
  switch(state) {
  CASESTR(EMPTY         );
  CASESTR(COMPILED      );
  CASESTR(DERIVED       );
  CASESTR(CANONICALFORM );
  CASESTR(MAINREDUCTION1);
  CASESTR(MAINREDUCTION2);
  CASESTR(STANDARDFORM  );
  CASESTR(REDUCTIONDONE );
  default: throwInvalidArgumentException(__TFUNCTION__, _T("state=%d"), state);
  }
#undef CASESTR
  return EMPTYSTRING;
}

void ParserTree::releaseAll() {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    SAFEDELETE(m_nodeTable[i]);
  }
  resetSimpleConstants();
  setRoot(NULL);
  m_errors.clear();
  m_nodeTable.clear();
  m_nonRootNodes.clear();
  m_rationalConstantMap.clear();
  getSymbolTable().clear();
}

ExpressionNodeNumber *ParserTree::getRationalConstant(const Rational &r) {
  ExpressionNodeNumber **v = m_rationalConstantMap.get(r);
  if(v) {
    return *v;
  }
  ExpressionNodeNumber *n = new ExpressionNodeNumber(this, r); TRACE_NEW(n);
  m_rationalConstantMap.put(r,n);
  return n;
}

void ParserTree::addError(ExpressionNode *n, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  if((n == NULL) || !n->hasPos()) {
    vAddError(NULL, format, argptr);
  } else {
    vAddError(&n->getPos(), format, argptr);
  }
}

void ParserTree::addError(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(&pos, format, argptr);
  va_end(argptr);
}

void ParserTree::addError(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vAddError(NULL, format, argptr);
  va_end(argptr);
}

void ParserTree::vAddError(const SourcePosition *pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String tmp = (pos != NULL)
             ? ::format(_T("%s:%s"), pos->toString().cstr(), vformat(format, argptr).cstr())
             : vformat(format, argptr);
  tmp.replace('\n',' ');
  m_errors.add(tmp);
}

void ParserTree::listErrors(FILE *f) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    _ftprintf(f,_T("%s\n"), m_errors[i].cstr());
  }
  fflush(f);
}

void ParserTree::listErrors(tostream &out) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    out << m_errors[i] << endl;
  }
  out.flush();
}

void ParserTree::listErrors(const TCHAR *fname) const {
  FILE *f = fopen(fname,_T("w"));
  if(f == NULL) {
    _ftprintf(stdout,_T("Cannot open %s\n"), fname);
    listErrors(stdout);
  } else {
    listErrors(f);
    fclose(f);
  }
}

UINT ParserTree::getNodeCount(ExpressionNodeSelector *selector) const {
  if(getRoot() == NULL) {
    return 0;
  } else {
    if(selector) {
      unmarkAll();
      markPow1Nodes();
    }
    return getRoot()->getNodeCount(selector);
  }
}

// if(validSymbolSet != NULL, only node with symbols contained in set will be counted
UINT ParserTree::getNodeCount(bool ignoreMarked, const ExpressionSymbolSet *validSymbolSet) const {
  if((validSymbolSet == NULL) && !ignoreMarked) {
    return getNodeCount();
  } else {
    return getNodeCount(&ExpressionNodeSymbolSelector(validSymbolSet, ignoreMarked));
  }
}

class Pow1NodeMarker : public ExpressionNodeHandler {
public:
  bool handleNode(ExpressionNode *n);
};

bool Pow1NodeMarker::handleNode(ExpressionNode *n) {
  if((n->getSymbol() == POW) && (n->right()->isOne())) {
    n->mark();
    n->right()->mark();
  }
  return true;
}

void ParserTree::markPow1Nodes() const {
  ((ParserTree*)this)->traverseTree(Pow1NodeMarker());
}

UINT ParserTree::getTreeDepth() const {
  return getRoot() ? getRoot()->getMaxTreeDepth() : 0;
}

SNode ParserTree::traverseSubstituteNodes(SNode n, CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  ExpressionNode **n1 = nodeMap.get(n.node());
  if(n1) {
    return *n1;
  }
  // not found;
  switch(n.getNodeType()) {
  case NT_NUMBER    :
  case NT_BOOLCONST :
  case NT_VARIABLE  :
    return n;
  case NT_POLY:
    { const CoefArray &coefArray = n.getCoefArray();
      SNode            arg       = n.getArgument();
      CoefArray        newCoefArray(coefArray.getTree(),coefArray.size());
      SNode            newArg    = traverseSubstituteNodes(arg, nodeMap);
      for(size_t i = 0; i < coefArray.size(); i++) {
        newCoefArray.add(traverseSubstituteNodes(coefArray[i], nodeMap));
      }
      return getPoly(n, newCoefArray, newArg);
    }
  case NT_TREE      :
  case NT_BOOLEXPR  :
  case NT_ADDENT    :
  case NT_ASSIGN    :
  case NT_STMTLIST  :
    { const SNodeArray &a = n.getChildArray();
      SNodeArray        newChildArray(a.getTree(),a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getTreeNode(n, newChildArray);
    }
  case NT_SUM       :
    { const AddentArray &a = n.getAddentArray();
      AddentArray        newAddentArray(a.getTree(),a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newAddentArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getSum(n, newAddentArray);
    }
  case NT_PRODUCT   :
    { const FactorArray &a = n.getFactorArray();
      FactorArray        newFactorArray(a.getTree(),a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newFactorArray *= traverseSubstituteNodes(a[i], nodeMap);
      }
      return getProduct(n, newFactorArray);
    }
  case NT_POWER     :
    { ExpressionNode *oldPower = n.node();
      SNode newBase   = traverseSubstituteNodes(oldPower->base()    , nodeMap);
      SNode newExpo   = traverseSubstituteNodes(oldPower->exponent(), nodeMap);
      return getPower(oldPower, newBase, newExpo);
    }

  default            :
    n.throwUnknownNodeTypeException(__TFUNCTION__);
    return NULL;
  }
}

void ParserTree::substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  ExpressionNode *root = getRoot();
  if(root) {
    setRoot(traverseSubstituteNodes(root, nodeMap).node());
  }
}

void ParserTree::traverseTree(ExpressionNodeHandler &handler) const {
  if(!isEmpty()) {
    m_root->traverseExpression(handler);
  }
}

ExpressionNode *ParserTree::allocateLoopVarNode(const String &prefix) {
  ExpressionNodeName *result = fetchNameNode(getSymbolTable().getNewLoopName(prefix));
  getSymbolTable().allocateSymbol(result, false, true, true);
  return result;
}

class MarkedNodeTransformer : public ExpressionNodeHandler {
private:
  CompactNodeHashMap<ExpressionNode*> m_nodeMap;
protected:
  ParserTree &m_tree;
public:
  MarkedNodeTransformer(ParserTree *tree) : m_tree(*tree) {
  }
  inline void putNodes(const ExpressionNode *from, ExpressionNode *to) {
    if(to != from) m_nodeMap.put(from, to);
  }
  void transform();
};

void MarkedNodeTransformer::transform() {
  m_tree.traverseTree(*this);
  if(!m_nodeMap.isEmpty()) {
    m_tree.substituteNodes(m_nodeMap);
    m_tree.pruneUnusedNodes();
  }
}

class MarkedNodeExpander : public MarkedNodeTransformer {
public:
  MarkedNodeExpander(ParserTree *tree) : MarkedNodeTransformer(tree) {
  }
  bool handleNode(ExpressionNode *n);
};

bool MarkedNodeExpander::handleNode(ExpressionNode *n) {
  if(n->isMarked() && n->isExpandable()) {
    putNodes(n, n->expand().node());
  }
  return true;
}

class MarkedNodeMultiplier : public MarkedNodeTransformer {
public:
  MarkedNodeMultiplier(ParserTree *tree) : MarkedNodeTransformer(tree) {
  }
  bool handleNode(ExpressionNode *n);
};

bool MarkedNodeMultiplier::handleNode(ExpressionNode *n) {
  if(n->isMarked()) {
    putNodes(n, SNode(n).multiplyParentheses().node());
  }
  return true;
}

ParserTree &ParserTree::expandMarkedNodes() {
  MarkedNodeExpander(this).transform();
  return *this;
}

ParserTree &ParserTree::multiplyMarkedNodes() {
  MarkedNodeMultiplier(this).transform();
  return *this;
}

// ---------------------------------------- only used by parser --------------------------------------------------

ExpressionNode *ParserTree::vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr) {
  ExpressionNode *n;
  switch(symbol) {
  case NUMBER  :
    n = new ExpressionNodeNumberWithPos(   this, pos, va_arg(argptr,Real  ));
    break;
  case NAME    :
    n = new ExpressionNodeNameWithPos(     this, pos, va_arg(argptr,TCHAR*));
    break;
  case EQ      :
  case NE      :
  case LE      :
  case LT      :
  case GE      :
  case GT      :
  case AND     :
  case OR      :
  case NOT     :
    n = new ExpressionNodeBoolExprWithPos( this, pos, symbol, argptr);
    break;
  case ASSIGN  :
    n = new ExpressionNodeAssignWithPos(   this, pos,         argptr);
    break;
  case STMTLIST:
    { SNodeArray stmtArray(*this);
      stmtArray.convertFromParserTree(va_arg(argptr, ExpressionNode*), SEMI);
      n = new ExpressionNodeStmtList(      this, stmtArray);
    }
    break;
  case POLY    :
    { CoefArray coefArray(*this);
      coefArray.convertFromParserTree(va_arg(argptr, ExpressionNode*), COMMA);
      SNode arg = va_arg(argptr, ExpressionNode*);
      n = new ExpressionNodePolyWithPos(   this, pos, coefArray, arg);
    }
    break;
  case POW     :
    n = new ExpressionNodePowerWithPos(    this, pos,         argptr);
    break;
  default      :
    n = new ExpressionNodeTreeWithPos(     this, pos, symbol, argptr);
    break;
  }
  TRACE_NEW(n);
  return n;
}

// Used by derivate, reduce, graphics
ExpressionNodeName *ParserTree::fetchNameNode(const String &name) {
  ExpressionNodeName *v = new ExpressionNodeName(this, name); TRACE_NEW(v);
  return v;
}

ExpressionNodeNumber *ParserTree::numberExpr(const Number   &v) {
  if(isRational(v)) return numberExpr((Rational)v);
  ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
  return n;
}

ExpressionNodeNumber *ParserTree::numberExpr(const Real     &v) {
  ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
  return n;
}

ExpressionNodeBoolConst *ParserTree::boolConstExpr(bool b, bool checkIsSimple) {
  if(checkIsSimple) return b ? getTrue() : getFalse();
  ExpressionNodeBoolConst *n = new ExpressionNodeBoolConst(this, b); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::constExpr(const String &name) {
  ExpressionVariable *v = getSymbolTable().getVariable(name);
  if(v == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%s not found in symbol table"), name.cstr());
  } else if(!v->isConstant()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%s is not a constant"), name.cstr());
  }
  ExpressionNode *n = new ExpressionNodeName(this, name, *v); TRACE_NEW(n);
  return n;
}

String ParserTree::toString() const {
  unmarkAll();
  String result = format(_T("Mode:%-10s, Trigonometic:%-8s, State:%-14s, Reduceiteration:%3u\n")
                        ,getTreeFormName().cstr()
                        ,::toString(getTrigonometricMode()).cstr()
                        ,getStateName().cstr()
                        ,getReduceIteration());

  result += getSymbolTable().toString();
  if(isEmpty()) {
    result += _T("Root = NULL\n");
  } else {
    getRoot()->dumpNode(result, 0);
  }
  return result;
}

}; // namespace Expr
