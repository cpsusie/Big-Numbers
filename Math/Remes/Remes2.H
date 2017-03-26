#pragma once

#include <Math/Matrix.h>

typedef enum {
  CPP
 ,CPP80
 ,JAVA
} Language;

class Remes {
private:
  Real      m_E;
  int       m_M, m_K, m_N;   // N = m + k
  Real      m_maxError;
  bool      m_verbose;
  bool      m_relative;
  int       m_mainIteration;
  Function &m_inputFunction; // function to be approximated
  Real      m_left, m_right;
  TCHAR    *m_name;
  Vector    m_coef;          // coef[0 .. N + 1] = { a0..am,b1..bk,E } b0 = 1
  Vector    m_extr;          // extr[0 .. N + 1]
  Vector    m_funcValue;     // funcval[0 .. N + 1]
  Vector    m_errorValue;    // value of errorFunction [0 .. N + 1]

  void initCoef();
  void initExtrema();
  void plotError(int i);
  Real errorFunction(const Real &x);
  Real approximation(const Real &x);
  Real findExtremum(const Real &l, const Real &m, const Real &r, int depth);
  void setExtremum(int index, const Real &x);
  void findExtrema(Real &minExtr, Real &maxExtr);
  void findCoef();
  Real sfunc(const Real &x);  /* approx = s(x) * Pa(x) / Pb(x)  */
  Real targetFunction(const Real &x);
  void genCppFunction(FILE *f);
  void genCpp80BitFunction(FILE *f);
  void genJavaFunction(FILE *f);
  void genHeader(FILE *f);
public:
  Remes(const Real &left, const Real &right, Function &inputFunction, bool relativeError, TCHAR *name, bool verbose = false);
  void solve(int M, int K);
  void genOutput( tostream &s );
  void genPlot(   tostream &s );
  void genFunction(FILE *f, Language language);
};
