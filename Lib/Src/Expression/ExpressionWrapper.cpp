#include "pch.h"
#include <Math.h>
#include <Math/Expression/ExpressionWrapper.h>

Real ExpressionWrapper::s_dummy = 0;

static Expression *allocateExpression() {
  Expression *e = new Expression(); TRACE_NEW(e);
  return e;
}
static void deleteExpression(Expression *e) {
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
  compile(text, machineCode, listFile);
  if(!ok()) {
    const String msg = getErrorMessage();
    deleteExpression(m_expr);
    throwException(msg);
  }
}

void ExpressionWrapper::compile(const String &text, bool machineCode, FILE *listFile) {
  m_expr->compile(text, machineCode, false, listFile);

  m_xp = getVariableByName(_T("x"));
  m_yp = getVariableByName(_T("y"));
  m_zp = getVariableByName(_T("z"));
  m_tp = getVariableByName(_T("t"));
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
  const ExpressionVariable *var = m_expr->getVariable(name);
  return (var == NULL) ? &s_dummy : &m_expr->getValueRef(*var);
}

String ExpressionWrapper::getErrorMessage() {
  if(m_expr->getErrors().size() == 0) {
    return _T("No errors");
  }
  return m_expr->getErrors()[0];
}

String ExpressionWrapper::getDefaultFileName() { // static
  return Expression::getDefaultListFileName();
}

