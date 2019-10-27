#pragma once

#include <Math/Expression/Expression.h>

using namespace Expr;

#define EPS 1e-8

class ExpressionTest {
private:
  static bool           s_evaluateTestStarted;
  static bool           s_deriveTestStarted;
  static int            s_instanceCount;
  static int            s_maxInstanceCount;
  static int            s_evaluatedCount;
  static int            s_maxDerivableCount;
  static int            s_derivableCount;
  static int            s_derivedCount;
  const bool            m_derivable : 1;
  bool                  m_evaluated : 1;
  bool                  m_derived   : 1;
protected:
  static printfFunction s_printf;
public:
  ExpressionTest(bool derivable);
  virtual ~ExpressionTest();
  void setEvaluated() {
    m_evaluated = true;
    s_evaluatedCount++;
  }
  void setDerived() {
    verify(getReturnType() == EXPR_RETURN_REAL);
    verify(m_derivable);
    m_derived = true;
    s_derivedCount++;
  }
  bool isDerivable() const {
    return m_derivable;
  }
  virtual String getExpr() const = 0;
  virtual ExpressionReturnType getReturnType() const = 0;
  virtual Real fr(const Real &x) const = 0;
  virtual bool fb(const Real &x) const = 0;
  virtual TrigonometricMode getTrigonometricMode() const {
    return RADIANS;
  }
  static void startEvaluateTest(printfFunction prf) {
    s_evaluateTestStarted = true;
    s_printf = prf;
  }
  static void startDeriveTest(printfFunction prf) {
    s_deriveTestStarted = true;
    s_printf = prf;
  }
  Real numDFDX(const Real &x) const;

  static const CompactArray<ExpressionTest*> &getAllSamples();
};

class RealExpressionTest : public ExpressionTest {
public:
  RealExpressionTest(bool derivable = true) : ExpressionTest(derivable) {
  }
  bool fb(const Real &x) const {
    throwException(_T("RealExpressionTest(\"%s\"):fb should not be called"), getExpr());
    return 0;
  }
  ExpressionReturnType getReturnType() const {
    return EXPR_RETURN_REAL;
  }
};

class BoolExpressionTest : public ExpressionTest {
public:
  BoolExpressionTest() : ExpressionTest(false) {
  }
  Real fr(const Real &x) const {
    throwException(_T("BoolExpressionTest(\"%s\"):fr should not be called"), getExpr());
    return 0;
  }
  ExpressionReturnType getReturnType() const {
    return EXPR_RETURN_BOOL;
  }
};
