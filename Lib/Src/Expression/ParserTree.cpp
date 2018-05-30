#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

namespace Expr {

#ifdef CHECK_CONSISTENCY
#define CHECKROOT_ISCONSISTENT() if(!isEmpty()) getRoot()->checkIsConsistent();
#else // CHECK_CONSISTENCY
  #define CHECKROOT_ISCONSISTENT()
#endif

ParserTree::ParserTree(TrigonometricMode mode) {
  init(mode, PS_EMPTY,0);
  m_root   = NULL;
  m_ops    = NodeOperators::s_stdForm;
  m_ok     = false;
}

ParserTree::ParserTree(const ParserTree &src) {
  init(src.getTrigonometricMode(), src.getState(), src.getReduceIteration());
  m_errors = src.m_errors;
  m_root   = src.isEmpty() ? NULL : src.getRoot()->clone(this);
  m_ops    = src.m_ops;
  m_ok     = src.m_ok;
  CHECKROOT_ISCONSISTENT();
}

void ParserTree::init(TrigonometricMode    mode
                     ,ParserTreeState      state
                     ,UINT                 reduceIteration) {
  m_trigonometricMode = mode;
  m_state             = state;
  m_reduceIteration   = reduceIteration;
  resetSimpleConstants();
}

ParserTree &ParserTree::operator=(const ParserTree &src) {
  if(&src == this) {
    return *this;
  }
  releaseAll();
  m_errors = src.m_errors;
  setRoot(             src.isEmpty() ? NULL : src.getRoot()->clone(this));
  setTreeForm(         src.getTreeForm());
  setOk(               src.m_ok);
  setReduceIteration(  src.getReduceIteration());
  setState(            src.getState());
  return *this;
}

ParserTree::~ParserTree() {
  releaseAll();
}

void ParserTree::setRoot(ExpressionNode *n) {
  setProperty(PP_ROOT, m_root, n);
  CHECKROOT_ISCONSISTENT();
}

void ParserTree::setOk(bool ok) {
  setProperty(PP_OK, m_ok, ok);
}

void ParserTree::setState(ParserTreeState newState) {
  setProperty(PP_STATE, m_state, newState);
}

void ParserTree::setReduceIteration(UINT iteration) {
  setProperty(PP_REDUCEITERATION, m_reduceIteration, iteration);
}

void ParserTree::setTreeForm(ParserTreeForm form) {
  const ParserTreeForm oldForm = getTreeForm();
  if(form != oldForm) {
    const ExpressionVariableArray oldVariables = getSymbolTable().getAllVariables();
    switch(form) {
    case TREEFORM_STANDARD : setRoot(toStandardForm( getRoot())); break;
    case TREEFORM_CANONICAL: setRoot(toCanonicalForm(getRoot())); break;
    case TREEFORM_NUMERIC  : setRoot(toNumericForm(  getRoot())); break;
    default                :
      throwInvalidArgumentException(__TFUNCTION__,_T("form=%d"), form);
    }
    pruneUnusedNodes();
    buildSymbolTable(&oldVariables);
    notifyPropertyChanged(PP_TREEFORM, &oldForm, &form);
  }
}


String ParserTree::getTreeFormName(ParserTreeForm treeForm) { // static
  switch(treeForm) {
  case TREEFORM_STANDARD : return _T("standard");
  case TREEFORM_CANONICAL: return _T("canonical");
  case TREEFORM_NUMERIC  : return _T("numeric");
  default                : return format(_T("Unknown treeForm:%d"), treeForm);
  }
}

void ParserTree::releaseAll() {
  for(size_t i = 0; i < m_nodeTable.size(); i++) {
    SAFEDELETE(m_nodeTable[i]);
  }
  for(size_t i = 0; i < m_addentTable.size(); i++) {
    SAFEDELETE(m_addentTable[i]);
  }
  resetSimpleConstants();
  setRoot(NULL);
  setOk(false);
  m_errors.clear();
  m_nodeTable.clear();
  m_addentTable.clear();
  m_symbolTable.clear(NULL);
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
  String tmp2;
  String tmp = vformat(format, argptr);
  if(pos != NULL) {
    tmp2 = ::format(_T("%s:%s"), pos->toString().cstr(), tmp.cstr());
  } else {
    tmp2 = ::format(_T("%s"), tmp.cstr());
  }
  tmp2.replace('\n',' ');
  m_errors.add(tmp2);
  setOk(false);
}

SourcePosition ParserTree::decodeErrorString(String &error) { // static
  Tokenizer tok(error, _T(":"));
  String posStr = tok.next();
  int line, col;
  if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line, &col) == 2) {
    error = tok.getRemaining();
  } else {
    throwException(_T("No sourceposition"));
  }
  return SourcePosition(line,col);
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

int ParserTree::getNodeCount(ExpressionNodeSelector *selector) {
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

int ParserTree::getNodeCount(bool ignoreMarked, ExpressionInputSymbol s1,...) { // terminate symbolset with 0. Only specified symbols will be counted
  ExpressionSymbolSet legalSymbols;
  bool                setIsEmpty = true;
  if(s1) {
    legalSymbols.add(s1);
    va_list argptr;
    va_start(argptr,s1);
    for(ExpressionInputSymbol s = va_arg(argptr, ExpressionInputSymbol); s; s = va_arg(argptr, ExpressionInputSymbol)) {
      legalSymbols.add(s);
    }
    va_end(argptr);
    setIsEmpty = false;
  }

  ExpressionNodeSymbolSelector selector(setIsEmpty ? NULL : &legalSymbols, ignoreMarked);

  if(setIsEmpty && !ignoreMarked) {
    return getNodeCount();
  } else {
    return getNodeCount(&selector);
  }
}

class Pow1NodeMarker : public ExpressionNodeHandler {
public:
  bool handleNode(ExpressionNode *n, int level);
};

bool Pow1NodeMarker::handleNode(ExpressionNode *n, int level) {
  if((n->getSymbol() == POW) && (n->right()->isOne())) {
    n->mark();
    n->right()->mark();
  }
  return true;
}

void ParserTree::markPow1Nodes() {
  traverseTree(Pow1NodeMarker());
}

int ParserTree::getTreeDepth() const {
  return getRoot() ? getRoot()->getMaxTreeDepth() : 0;
}

SNode ParserTree::traverseSubstituteNodes(SNode n, CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  DEFINEMETHODNAME;
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
    { const SNodeArray &coefArray = n.getCoefArray();
      SNode             arg       = n.getArgument();
      SNodeArray        newCoefArray(coefArray.size());
      SNode             newArg    = traverseSubstituteNodes(arg, nodeMap);
      for(size_t i = 0; i < coefArray.size(); i++) {
        newCoefArray.add(traverseSubstituteNodes(coefArray[i], nodeMap));
      }
      return getPoly(n, newCoefArray, newArg);
    }
  case NT_BOOLEXPR  :
    { const SNodeArray &a = n.getChildArray();
      SNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getBoolExpr(n, newChildArray);
    }
  case NT_TREE      :
    { const SNodeArray &a = n.getChildArray();
      SNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getTree(n, newChildArray);
    }
  case NT_ASSIGN    :
    { const SNodeArray &a = n.getChildArray();
      SNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getAssignStmt(n, newChildArray);
    }
  case NT_STMTLIST  :
    { const SNodeArray &a = n.getChildArray();
      SNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getStmtList(n, newChildArray);
    }
  case NT_SUM       :
    { const AddentArray &a = n.getAddentArray();
      AddentArray        newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        SumElement *e       = a[i];
        SNode oldNode = e->getNode();
        SNode newNode = traverseSubstituteNodes(oldNode, nodeMap);
        if(newNode.isSameNode(oldNode)) {
          newAddentArray.add(e);
        } else {
          newAddentArray.add(newNode, e->isPositive());
        }
      }
      return getSum(n, newAddentArray);
    }
  case NT_PRODUCT   :
    { const FactorArray &a = n.getFactorArray();
      FactorArray        newFactorArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newFactorArray.add(traverseSubstituteNodes(a[i], nodeMap));
      }
      return getProduct(n, newFactorArray);
    }
  case NT_FACTOR    :
    { ExpressionFactor *oldFactor = (ExpressionFactor*)n.node();
      SNode newBase   = traverseSubstituteNodes(oldFactor->base()    , nodeMap);
      SNode newExpo   = traverseSubstituteNodes(oldFactor->exponent(), nodeMap);
      return getFactor(oldFactor, newBase, newExpo);
    }

  default            :
    n.throwUnknownNodeTypeException(method);
    return NULL;
  }
}

void ParserTree::substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  ExpressionNode *root = getRoot();
  if(root) {
    setRoot(traverseSubstituteNodes(root, nodeMap).node());
  }
}

void ParserTree::traverseTree(ExpressionNodeHandler &handler) {
  if(!isEmpty()) {
    getRoot()->traverseExpression(handler, 0);
  }
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
  bool handleNode(ExpressionNode *n, int level);
};

bool MarkedNodeExpander::handleNode(ExpressionNode *n, int level) {
  if(n->isMarked() && n->isExpandable()) {
    putNodes(n, n->expand());
  }
  return true;
}

class MarkedNodeMultiplier : public MarkedNodeTransformer {
public:
  MarkedNodeMultiplier(ParserTree *tree) : MarkedNodeTransformer(tree) {
  }
  bool handleNode(ExpressionNode *n, int level);
};

bool MarkedNodeMultiplier::handleNode(ExpressionNode *n, int level) {
  if(n->isMarked()) {
    putNodes(n, SNode(n).multiplyParentheses().node());
  }
  return true;
}

void ParserTree::expandMarkedNodes() {
  MarkedNodeExpander(this).transform();
}

void ParserTree::multiplyMarkedNodes() {
  MarkedNodeMultiplier(this).transform();
}

// ---------------------------------------- only used by parser --------------------------------------------------

static SNodeArray &getListFromTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol, SNodeArray &list) {
  if(n->getSymbol() == delimiterSymbol) {
    getListFromTree(n->left(), delimiterSymbol,list);
    list.add(n->right());
  } else {
    list.add(n);
  }
  return list;
}

ExpressionNode *ParserTree::vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr) {
  ExpressionNode *n;
  switch(symbol) {
  case NUMBER  :
    n = new ExpressionNodeNumberWithPos(   this, pos, va_arg(argptr,Real  ));
    break;
  case NAME    :
    n = new ExpressionNodeVariableWithPos( this, pos, va_arg(argptr,TCHAR*));
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
    { SNodeArray stmtArray;
      getListFromTree(va_arg(argptr, ExpressionNode*), SEMI, stmtArray);
      n = new ExpressionNodeStmtList(      this, stmtArray);
    }
    break;
  case POLY    :
    { SNodeArray coefArray;
      getListFromTree(va_arg(argptr, ExpressionNode*), COMMA, coefArray);
      SNode arg = va_arg(argptr, ExpressionNode*);
      n = new ExpressionNodePolyWithPos(   this, pos, coefArray, arg);
    }
    break;
  default      :
    n = new ExpressionNodeTreeWithPos(     this, pos, symbol, argptr);
    break;
  }
  TRACE_NEW(n);
  return n;
}

// Used by derivate, reduce, graphics
ExpressionNodeVariable *ParserTree::fetchVariableNode(const String &name) {
  ExpressionNodeVariable *v = new ExpressionNodeVariable(this, name); TRACE_NEW(v);
  return v;
}

ExpressionNode *ParserTree::constExpression(const String &name) {
  DEFINEMETHODNAME;

  const ExpressionVariable *v = getVariable(name);
  if(v == NULL) {
    throwInvalidArgumentException(method, _T("%s not found in symbol table"), name.cstr());
  } else if(!v->isConstant()) {
    throwInvalidArgumentException(method, _T("%s is not a constant"), name.cstr());
  }
  ExpressionNode *n = new ExpressionNodeVariable(this, name, *(ExpressionVariable*)v); TRACE_NEW(n);
  return n;
}

String ParserTree::treeToString() const {
  ((ParserTree*)this)->unmarkAll();
  String result;
  if(!isEmpty()) {
    getRoot()->dumpNode(result, 0);
  }
  result += getSymbolTable().toString();
  return result;
}

}; // namespace Expr
