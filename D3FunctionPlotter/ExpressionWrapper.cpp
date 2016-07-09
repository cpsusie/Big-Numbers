#include "stdafx.h"
#include <Math.h>
#include <Math/MathFunctions.h>
#include <Math/Expression/Expression.h>
#include "ExpressionWrapper.h"

static Real dummy = 0;

ExpressionWrapper::ExpressionWrapper() {
  m_expr = new Expression();
  compile("0",false);
}

ExpressionWrapper::~ExpressionWrapper() {
  delete (Expression*)m_expr;
}

void ExpressionWrapper::compile(const String &text, bool machineCode) {
  Expression *e = (Expression*)m_expr;
  e->compile(text, machineCode);

  m_xp = getVariableByName(_T("x"));
  m_yp = getVariableByName(_T("y"));
  m_zp = getVariableByName(_T("z"));
  m_tp = getVariableByName(_T("t"));
}

double ExpressionWrapper::operator()(const Point2D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  return ((Expression*)m_expr)->evaluate();
}

double ExpressionWrapper::operator()(const Point3D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  *m_zp = p.z;
  return ((Expression*)m_expr)->evaluate();
}

bool ExpressionWrapper::ok() {
  Expression *e = (Expression*)m_expr;
  return e->isOk();
}

Real *ExpressionWrapper::getVariableByName(const String &name) {
  ExpressionVariable *var = ((Expression*)m_expr)->getVariable(name);
  return var == NULL ? &dummy : &var->getValue();
}

Real ExpressionWrapper::evaluate() {
  return ((Expression*)m_expr)->evaluate();
}

String ExpressionWrapper::getErrorMessage() {
  Expression *e = (Expression*)m_expr;
  if(e->getErrors().size() == 0) {
    return "No errors";
  }
  return e->getErrors()[0];
}

bool ExpressionWrapper::isReturnTypeReal() const {
  return ((Expression*)m_expr)->getReturnType() == EXPR_RETURN_REAL;
}

bool ExpressionWrapper::isReturnTypeBool() const {
  return ((Expression*)m_expr)->getReturnType() == EXPR_RETURN_BOOL;
}
