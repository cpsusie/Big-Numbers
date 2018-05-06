#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

using namespace std;

ParserTree::ParserTree() {
  init(PS_EMPTY,0);
  m_root             = NULL;
  m_ops              = NodeOperators::s_stdForm;
  m_ok               = false;
}

ParserTree::ParserTree(const ParserTree &src) {
  init(src.getState(), src.getReduceIteration());
  m_errors           = src.m_errors;
  m_root             = src.m_root ? src.m_root->clone(this) : NULL;
  m_ops              = src.m_ops;
  m_ok               = src.m_ok;
}

void ParserTree::init(ParserTreeState      state
                     ,UINT                 reduceIteration) {
  m_state           = state;
  m_reduceIteration = reduceIteration;
  resetSimpleConstants();
}

ParserTree &ParserTree::operator=(const ParserTree &src) {
  if(&src == this) {
    return *this;
  }
  releaseAll();
  m_errors = src.m_errors;
  setRoot(           src.m_root ? src.m_root->clone(this) : NULL);
  setTreeForm(       src.getTreeForm());
  setOk(             src.m_ok);
  setReduceIteration(src.getReduceIteration());
  setState(          src.getState());
  return *this;
}

ParserTree::~ParserTree() {
  releaseAll();
}

void ParserTree::setRoot(ExpressionNode *n) {
  setProperty(PP_ROOT, m_root, n);
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
    switch(form) {
    case TREEFORM_STANDARD :
    case TREEFORM_CANONICAL:
    case TREEFORM_NUMERIC  :
    default                :
      throwInvalidArgumentException(__TFUNCTION__,_T("form=%d"), form);
    }
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

static void getListFromTree(ExpressionNode *n, int delimiterSymbol, ExpressionNodeArray &list) {
  if(n->getSymbol() == delimiterSymbol) {
    getListFromTree(n->left(),delimiterSymbol,list);
    list.add(n->right());
  } else {
    list.add(n);
  }
}

ExpressionNodeArray getExpressionList(ExpressionNode *n) {
  ExpressionNodeArray result(10);
  getListFromTree(n, COMMA, result);
  return result;
}

ExpressionNodeArray getStatementList(ExpressionNode *n) {
  ExpressionNodeArray result(10);
  getListFromTree(n, SEMI, result);
  return result;
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

ExpressionNode *ParserTree::traverseSubstituteNodes(ExpressionNode *n, CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  DEFINEMETHODNAME;
  ExpressionNode **n1 = nodeMap.get(n);
  if(n1) {
    return *n1;
  }
  // not found;
  switch(n->getNodeType()) {
  case EXPRESSIONNODENUMBER    :
  case EXPRESSIONNODEBOOLEAN   :
  case EXPRESSIONNODEVARIABLE  : return n;
  case EXPRESSIONNODEFACTOR    :
    { ExpressionFactor *oldFactor = (ExpressionFactor*)n;
      ExpressionNode   *newBase   = traverseSubstituteNodes(oldFactor->base()    , nodeMap);
      ExpressionNode   *newExpo   = traverseSubstituteNodes(oldFactor->exponent(), nodeMap);
      return getFactor(oldFactor, newBase, newExpo);
    }
  case EXPRESSIONNODETREE      :
    { const ExpressionNodeArray  &a = n->getChildArray();
      ExpressionNodeArray         newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) newChildArray.add(traverseSubstituteNodes(a[i], nodeMap));
      return getTree(n, newChildArray);
    }
  case EXPRESSIONNODESUM       :
    { const AddentArray &a = n->getAddentArray();
      AddentArray newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        SumElement     *e       = a[i];
        ExpressionNode *oldNode = e->getNode();
        ExpressionNode *newNode = traverseSubstituteNodes(oldNode, nodeMap);
        if(newNode == oldNode) {
          newAddentArray.add(e);
        } else {
          newAddentArray.add(newNode, e->isPositive());
        }
      }
      return getSum(n, newAddentArray);
    }
  case EXPRESSIONNODEPRODUCT   :
    { const FactorArray           &a = n->getFactorArray();
      FactorArray                  newFactorArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        ExpressionNode *newNode = traverseSubstituteNodes(a[i], nodeMap);
        if(newNode->getNodeType() == EXPRESSIONNODEFACTOR) {
          newFactorArray.add((ExpressionFactor*)newNode);
        } else {
          newFactorArray.add(newNode);
        }
      }
      return getProduct(n, newFactorArray);
    }
  case EXPRESSIONNODEPOLYNOMIAL:
    { const ExpressionNodeArray  &coef = n->getCoefficientArray();
      ExpressionNode             *arg  = n->getArgument();
      ExpressionNodeArray         newCoef(coef.size());
      ExpressionNode             *newArg  = traverseSubstituteNodes(arg, nodeMap);
      for(size_t i = 0; i < coef.size(); i++) newCoef.add(traverseSubstituteNodes(coef[i], nodeMap));
      return getPoly(n, newCoef, newArg);
    }
  default:
    throwInvalidArgumentException(method, _T("Unknown nodeType:%d"), n->getNodeType());
    return NULL;
  }
}

void ParserTree::substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap) {
  ExpressionNode *root = getRoot();
  if(root) {
    setRoot(traverseSubstituteNodes(root, nodeMap));
  }
}

void ParserTree::traverseTree(ExpressionNodeHandler &handler) {
  if(m_root) {
    m_root->traverseExpression(handler, 0);
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
    putNodes(n, SNode(n).multiplyParentheses());
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

ExpressionNode *ParserTree::vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr) {
  ExpressionNode *n;
  switch(symbol) {
  case NUMBER: n = new ExpressionNodeNumberWithPos(   this, pos, va_arg(argptr,Real  )); break;
  case NAME  : n = new ExpressionNodeVariableWithPos( this, pos, va_arg(argptr,TCHAR*)); break;
  case POLY  : n = new ExpressionNodePolyWithPos(     this, pos, argptr); break;
  default    : n = new ExpressionNodeTreeWithPos(     this, pos, symbol, argptr); break;
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
  if(m_root) m_root->dumpNode(result, 0);
  result += getSymbolTable().toString();
  return result;
}

