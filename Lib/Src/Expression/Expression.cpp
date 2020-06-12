#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/ExpressionSymbolTable.h>
#include <FileNameSplitter.h>

namespace Expr {

Expression::Expression(TrigonometricMode mode) : m_trigonometricMode(mode) {
  initialize();
}

Expression::Expression(const Expression &src) : m_trigonometricMode(src.getTrigonometricMode()) {
  initialize(src.getReturnType());
  if(src.m_tree) {
    new ParserTree(this, src.getRoot()); TRACE_NEW(m_tree);
  }
  setMachineCode(src.isMachineCode());
}

Expression::Expression(const ExpressionNode *root) : m_trigonometricMode(root->getTrigonometricMode()) {
  initialize(root->getReturnType());
  new ParserTree(this, root); TRACE_NEW(m_tree);
}

Expression::~Expression() {
  clear();
  uninitialize();
}

Expression &Expression::operator=(const Expression &src) {
  if(&src == this) {
    return *this;
  }

  clear();
  setTrigonometricMode(src.getTrigonometricMode());
  if(src.m_tree) {
    new ParserTree(this, src.getRoot()); TRACE_NEW(m_tree);
  }
  setMachineCode(src.isMachineCode());
  setReturnType( src.getReturnType());
  return *this;
}

void Expression::initialize(ExpressionReturnType returnType) {
  m_symbolTable       = new ExpressionSymbolTable(this);
  m_tree              = NULL;
  m_returnType        = returnType;
  m_machineCode       = false;
  m_code              = NULL;
  m_listFile          = NULL;
  m_ok                = false;
  updateEvalPointers();
}

void Expression::uninitialize() {
  SAFEDELETE(m_symbolTable);
}

bool Expression::hasSyntaxTree() const {
  return m_tree && !m_tree->isEmpty();
}

void Expression::clear() {
  SAFEDELETE(m_tree);
  m_symbolTable->clear();
  m_ok = false;
  setReturnType(EXPR_NORETURNTYPE);
  setMachineCode(false);
}

SourcePosition Expression::decodeErrorString(String &error) { // static
  Tokenizer tok(error, _T(":"));
  String posStr = tok.next();
  int line, col;
  if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line, &col) == 2) {
    error = tok.getRemaining();
  } else {
    throwException(_T("No sourceposition"));
  }
  return SourcePosition(line, col);
}

UINT Expression::decodeErrorString(const String &expr, String &error) { // static
  return decodeErrorString(error).findCharIndex(expr);
}

void Expression::setReturnType(ExpressionReturnType returnType) {
  setProperty(EP_RETURNTYPE, m_returnType, returnType);
  updateEvalPointers();
}

void Expression::updateEvalPointers() {
  switch(getReturnType()) {
  case EXPR_NORETURNTYPE:
    setEvalPointers(&Expression::evalRealError, &Expression::evalBoolError);
    break;
  case EXPR_RETURN_FLOAT :
    setEvalPointers(isMachineCode() ? &Expression::evalRealFast
                                    : hasSyntaxTree() ? &Expression::evalRealTree : &Expression::evalRealError
                   ,&Expression::evalBoolError);
    break;
  case EXPR_RETURN_BOOL  :
    setEvalPointers(&Expression::evalRealError
                   ,isMachineCode() ? &Expression::evalBoolFast
                                    : hasSyntaxTree() ? &Expression::evalBoolTree : &Expression::evalBoolError);
    break;
  }
}

Real Expression::evalRealError() const {
  throwException(_T("Cannot evaluate real. Returntype is %s")
                ,ExpressionNode::getReturnTypeName(m_returnType).cstr()
                );
  return 0;
}

bool Expression::evalBoolError() const {
  throwException(_T("Cannot evaluate bool. Returntype is %s")
                ,ExpressionNode::getReturnTypeName(m_returnType).cstr()
                );
  return false;
}

Real Expression::evalRealTree() const {
  return getRoot()->evaluateReal();
}
bool Expression::evalBoolTree() const {
  return getRoot()->evaluateBool();
}

void Expression::setTrigonometricMode(TrigonometricMode mode) {
  if(mode != m_trigonometricMode) {
    const TrigonometricMode oldMode = m_trigonometricMode;
    m_trigonometricMode = mode;
    if(isMachineCode()) {
      genMachineCode();
    }
    notifyPropertyChanged(EP_TRIGONOMETRICMODE, &oldMode, &mode);
  }
}

ExpressionNode *Expression::getRoot() const {
  return m_tree ? m_tree->getRoot() : NULL;
}

Expression &Expression::setValue(const String &name, const Real &value) {
  m_symbolTable->setValue(name, value);
  return *this;
}

const ExpressionVariable *Expression::getVariable(const String &name) const {
  return m_symbolTable->getVariable(name);
}

Real &Expression::getValueRef(const ExpressionVariable &var) const {
  return m_symbolTable->getValueRef(var.getValueIndex());
}

Real *Expression::getValueRef(const String &name) const {
  const ExpressionVariable *var = getVariable(name);
  return (var != NULL) ? &getValueRef(*var) : NULL;
}

Expression &Expression::expandMarkedNodes() {
  if(hasSyntaxTree()) m_tree->expandMarkedNodes();
  return *this;
}
// return this
Expression &Expression::multiplyMarkedNodes() {
  if(hasSyntaxTree()) m_tree->multiplyMarkedNodes();
  return *this;
}

bool Expression::equal(const Expression &e) const {
  if(m_tree == NULL) return false;
  const ParserTree *t = e.getTree();
  if(t == NULL) return false;
  return m_tree->equal(*t);
}

bool Expression::equalMinus(const Expression &e) const {
  if(m_tree == NULL) return false;
  const ParserTree *t = e.getTree();
  if(t == NULL) return false;
  return m_tree->equalMinus(*t);
}

String Expression::toString() const {
  const ExpressionNode *root = getRoot();
  return root ? root->toString() : EMPTYSTRING;
}

void Expression::print(const ExpressionNode *n, FILE *f) const {
  _ftprintf(f,_T("%s"), n->toString().cstr());
}

void Expression::print(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}

String Expression::getDefaultListFileName() { // static
#if defined(LONGDOUBLE)
#define _REALSTR_ "TB/"
#else
#define _REALSTR_ "QW/"
#endif // LONGDOUBLE
  FileNameSplitter info(getTestFileName(_T("expr"), _T("lst")));
  return info.setDir(FileNameSplitter::getChildName(info.getDir(), _T("ExprList/" _PLATFORM_ _CONFIGURATION_ _REALSTR_))).getAbsolutePath();
}

}; // namespace Expr
