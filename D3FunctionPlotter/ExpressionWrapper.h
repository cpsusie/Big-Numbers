#pragma once

// this class is only here to avoid nameclashes between MFC and mytools
class ExpressionWrapper : public Function2D {
private:
  void *m_expr; // real expression-evaluater
  Real *m_xp, *m_yp, *m_zp, *m_tp;
public:
  ExpressionWrapper();
  ExpressionWrapper(const ExpressionWrapper &src);            // not defined
  ExpressionWrapper &operator=(const ExpressionWrapper &src); // not defined
  virtual ~ExpressionWrapper();
  void compile(const String &text, bool machineCode);
  bool ok();
  Real *getVariableByName(const String &name);
  Real evaluate();
  String getErrorMessage();
  double operator()(const Point2D &p);
  double operator()(const Point3D &p);
  void setT(double t) {
    *m_tp = t;
  }
  bool isReturnTypeReal() const;
  bool isReturnTypeBool() const;
};
