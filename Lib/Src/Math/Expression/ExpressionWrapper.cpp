#include "pch.h"
#include <Math.h>
#include <Math/Expression/Expression.h>
#include <Math/Expression/ExpressionWrapper.h>

static Real dummy = 0;
#define M_EXPR ((Expression*)m_expr)

ExpressionWrapper::ExpressionWrapper() {
  m_expr = new Expression();
  compile(_T("0"),false);
}

ExpressionWrapper::~ExpressionWrapper() {
  delete M_EXPR;
}

ExpressionWrapper::ExpressionWrapper(const String &text, bool machineCode) {
  m_expr = new Expression();
  compile(text, machineCode);
  if(!ok()) {
    const String msg = getErrorMessage();
    delete M_EXPR;
    throwException(msg);
  }
}

void ExpressionWrapper::compile(const String &text, bool machineCode) {
  Expression *e = M_EXPR;
  e->compile(text, machineCode);

  m_xp = getVariableByName(_T("x"));
  m_yp = getVariableByName(_T("y"));
  m_zp = getVariableByName(_T("z"));
  m_tp = getVariableByName(_T("t"));
}

Real ExpressionWrapper::operator()(const Point2D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  return M_EXPR->evaluate();
}

Real ExpressionWrapper::operator()(const Point3D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  *m_zp = p.z;
  return M_EXPR->evaluate();
}

bool ExpressionWrapper::ok() {
  Expression *e = M_EXPR;
  return e->isOk();
}

Real *ExpressionWrapper::getVariableByName(const String &name) {
  const ExpressionVariable *var = M_EXPR->getVariable(name);
  return var == NULL ? &dummy : &M_EXPR->getValueRef(*var);
}

Real ExpressionWrapper::evaluate() {
  return M_EXPR->evaluate();
}

String ExpressionWrapper::getErrorMessage() {
  Expression *e = M_EXPR;
  if(e->getErrors().size() == 0) {
    return _T("No errors");
  }
  return e->getErrors()[0];
}

bool ExpressionWrapper::isReturnTypeReal() const {
  return M_EXPR->getReturnType() == EXPR_RETURN_REAL;
}

bool ExpressionWrapper::isReturnTypeBool() const {
  return M_EXPR->getReturnType() == EXPR_RETURN_BOOL;
}
