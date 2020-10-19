#pragma once

#include <Math/Point2D.h>
#include <Math/Point3D.h>
#include <Math/Expression/Expression.h>

// this class is only here to avoid nameclashes between MFC and mytools
class ExpressionWrapper : public FunctionR2R1, public FunctionR3R1 {
private:
  static Real s_dummy;
  Expr::Expression *m_expr; // real expression-evaluater
  Real             *m_xp, *m_yp, *m_zp, *m_tp;
  StringArray       m_errors;
  bool              m_ok;
  ExpressionWrapper(const ExpressionWrapper &src);            // Not defined. Class not cloneable
  ExpressionWrapper &operator=(const ExpressionWrapper &src); // Not defined. Class not cloneable
public:
  ExpressionWrapper();
  ExpressionWrapper(const String &text, bool machineCode, FILE *listFile = nullptr); // throws exception on error
  virtual ~ExpressionWrapper();
  bool compile(const String &text, bool machineCode, FILE *listFile = nullptr); // doesn't throw on error
  static String getDefaultFileName();
  inline bool ok() const {
    return m_ok;
  }

  Real *getVariableByName(const String &name);
  inline Real evaluate() const {
    return m_expr->evaluate();
  }

  String getErrorMessage() const;
  inline const StringArray &getAllErrors() const {
    return m_errors;
  }
  Real operator()(const Point2D &p);
  Real operator()(const Point3D &p);
  inline void setT(double t) {
    *m_tp = t;
  }
  inline bool isReturnTypeFloat() const {
    return m_expr->getReturnType() == Expr::EXPR_RETURN_FLOAT;
  }
  inline bool isReturnTypeBool() const {
    return m_expr->getReturnType() == Expr::EXPR_RETURN_BOOL;
  }
};
