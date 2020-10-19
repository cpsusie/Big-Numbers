#include "pch.h"
#include <Math.h>
#include <Math/Expression/ExpressionWrapper.h>

Real ExpressionWrapper::s_dummy = 0;

static Expr::Expression *allocateExpression() {
  Expr::Expression *e = new Expr::Expression(); TRACE_NEW(e);
  return e;
}
static void deleteExpression(Expr::Expression *e) {
  SAFEDELETE(e);
}

ExpressionWrapper::ExpressionWrapper() {
  m_expr = allocateExpression();
  compile(_T("0"),false);
}

ExpressionWrapper::~ExpressionWrapper() {
  deleteExpression(m_expr);
}

ExpressionWrapper::ExpressionWrapper(const String &text, bool machineCode, FILE *listFile) {
  m_expr = allocateExpression();
  m_ok   = compile(text, machineCode, listFile);
  if(!ok()) {
    const String msg = getErrorMessage();
    deleteExpression(m_expr);
    throwException(msg);
  }
}

bool ExpressionWrapper::compile(const String &text, bool machineCode, FILE *listFile) {
  m_ok = m_expr->compile(text, m_errors, machineCode, false, listFile);

  m_xp = getVariableByName(_T("x"));
  m_yp = getVariableByName(_T("y"));
  m_zp = getVariableByName(_T("z"));
  m_tp = getVariableByName(_T("t"));

  return m_ok;
}

Real ExpressionWrapper::operator()(const Point2D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  return m_expr->evaluate();
}

Real ExpressionWrapper::operator()(const Point3D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  *m_zp = p.z;
  return m_expr->evaluate();
}

Real *ExpressionWrapper::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr->getVariable(name);
  return (var == nullptr) ? &s_dummy : &m_expr->getValueRef(*var);
}

String ExpressionWrapper::getErrorMessage() const {
  return m_errors.isEmpty() ? _T("No errors") : m_errors.first();
}

String ExpressionWrapper::getDefaultFileName() { // static
  return Expr::Expression::getDefaultListFileName();
}
