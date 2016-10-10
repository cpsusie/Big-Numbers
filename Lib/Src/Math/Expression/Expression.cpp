#include "pch.h"
#include <Math/Expression/Expression.h>

DEFINECLASSNAME(Expression);

Expression::Expression(TrigonometricMode mode) {
  m_machineCode       = false;
  m_returnType        = EXPR_NORETURNTYPE;
  m_state             = EXPR_EMPTY;
  m_reduceIteration   = 0;
  m_trigonometricMode = mode;
}

Expression::Expression(const Expression &src) : ParserTree(src) {
  m_machineCode       = src.m_machineCode;
  m_returnType        = src.m_returnType;
  m_state             = src.m_state;
  m_reduceIteration   = src.m_reduceIteration;
  m_trigonometricMode = src.m_trigonometricMode;

  buildSymbolTable();
  copyValues((ParserTree&)src);

  if(m_machineCode) {
    genCode();
  }
}

Expression &Expression::operator=(const Expression &src) {
  if(&src == this) {
    return *this;
  }

  clear();
  ParserTree::operator=(src);
  m_machineCode       = src.m_machineCode;
  setTrigonometricMode(src.getTrigonometricMode());

  buildSymbolTable();
  copyValues((ParserTree&)src);

  if(m_machineCode) {
    genCode();
  }

  setReduceIteration(src.getReduceIteration());
  setReturnType(src.getReturnType());
  setState(src.getState());

  return *this;
}

void Expression::clear() {
  releaseAll();
  m_code.clear();
  m_machineCode  = false;
  m_entryPoint   = NULL;
  setState(EXPR_EMPTY);
  setReduceIteration(0);
}

void Expression::parse(const String &expr) {
  clear();

  setOk(true);
  LexStringStream    stream(expr);
  ExpressionLex      lex(&stream);
  ExpressionParser   parser(*this, &lex);
  lex.setParser(&parser);
  parser.parse();
  if(isOk()) {
    buildSymbolTable();
    setTreeForm(TREEFORM_STANDARD);
    setState(EXPR_COMPILED);
  }
}

void Expression::throwUnknownSymbolException(const TCHAR *method, SNode n) { // static
  throwUnknownSymbolException(method, n.node());
}

void Expression::throwUnknownSymbolException(const TCHAR *method, const ExpressionNode *n) { // static
  throwException(_T("%s:Unexpected symbol in expression tree:%s")
                ,method, n->getSymbolName().cstr());
}

void Expression::throwInvalidSymbolForTreeMode(const TCHAR *method, const ExpressionNode *n) const {
  throwException(_T("%s:Invalid symbol in tree form %s:<%s>")
                ,method, getTreeFormName().cstr(),  n->getSymbolName().cstr());
}

void Expression::setState(ExpressionState newState) {
  const ExpressionState oldState = m_state;
  if(newState != oldState) {
    m_state = newState;
    notifyPropertyChanged(EXPR_STATE, &oldState, &m_state);
  }
}

void Expression::setReduceIteration(UINT iteration) {
  const UINT oldIteration = m_reduceIteration;
  if(iteration != oldIteration) {
    m_reduceIteration = iteration;
    notifyPropertyChanged(EXPR_REDUCEITERATION, &oldIteration, &m_reduceIteration);
  }
}

void Expression::setReturnType(ExpressionReturnType returnType) {
  const ExpressionReturnType oldReturnType = m_returnType;
  if(returnType != oldReturnType) {
    m_returnType = returnType;
    notifyPropertyChanged(EXPR_RETURNTYPE, &oldReturnType, &m_returnType);
  }
}

void Expression::setTrigonometricMode(TrigonometricMode mode) {
  const TrigonometricMode oldMode = m_trigonometricMode;
  if(mode != oldMode) {
    m_trigonometricMode = mode;
    if(m_machineCode) {
      genCode();
    }
    notifyPropertyChanged(EXPR_TRIGONOMETRICMODE, &oldMode, &m_trigonometricMode);
  }
}

class MarkedNodeTransformer : public ExpressionNodeHandler {
private:
  CompactNodeHashMap<ExpressionNode*> m_nodeMap;
protected:
  Expression &m_expr;
public:
  MarkedNodeTransformer(Expression *expr) : m_expr(*expr) {
  }
  inline void putNodes(const ExpressionNode *from, ExpressionNode *to) {
    if(to != from) m_nodeMap.put(from, to);
  }

  Expression &transform();
};

Expression &MarkedNodeTransformer::transform() {
  m_expr.traverseTree(*this);
  if(m_nodeMap.isEmpty()) {
    return m_expr;
  }
  m_expr.substituteNodes(m_nodeMap);
  m_expr.pruneUnusedNodes();
  return m_expr;
}

class MarkedNodeExpander : public MarkedNodeTransformer {
public:
  MarkedNodeExpander(Expression *expr) : MarkedNodeTransformer(expr) {
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
  MarkedNodeMultiplier(Expression *expr) : MarkedNodeTransformer(expr) {
  }
  bool handleNode(ExpressionNode *n, int level);
};

bool MarkedNodeMultiplier::handleNode(ExpressionNode *n, int level) {
  if(n->isMarked()) {
    putNodes(n, m_expr.multiplyParentheses(n));
  }
  return true;
}

Expression &Expression::expandMarkedNodes() {
  return MarkedNodeExpander(this).transform();
}

Expression &Expression::multiplyMarkedNodes() {
  return MarkedNodeMultiplier(this).transform();
}

String Expression::toString() const {
  const ExpressionNode *root = getRoot();
  return root ? root->toString() : _T("");
}

void Expression::print(const ExpressionNode *n, FILE *f) const {
  _ftprintf(f,_T("%s"), n->toString().cstr());
}

void Expression::print(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}
