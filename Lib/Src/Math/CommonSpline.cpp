#include "pch.h"
#include <Math/Matrix.h>
#include <Math/Polynomial.h>

static int dpointcmp(const DataPoint &d1, const DataPoint &d2) {
  return sign(d1.x - d2.x);
}

int CommonSpline::preprocessdata(const CompactArray<DataPoint> &data, Vector &x, Vector &y) {
  CompactArray<DataPoint> tmp(data);
  tmp.sort(dpointcmp);
  CompactArray<DataPoint> copy;
  copy.add(tmp[0]);
  int c = 1;
  for(int i = 1; i < (int)tmp.size(); i++) {
    if(tmp[i].x == copy.last().x) {
      copy.last().y += tmp[i].y;
      c++;
    }
    else {
      copy.last().y /= c;
      copy.add(tmp[i]);
      c = 1;
    }
  }
  copy.last().y /= c;

  const int n = (int)copy.size();

  x.setDimension(n);
  y.setDimension(n);

  for(int i = 0; i < n; i++) {
    x[i] = copy[i].x;
    y[i] = copy[i].y;
  }
  return n;
}

int CommonSpline::findInterval(const Real &x) const {
  int n = (int)a.getDimension();
  if(m_lastInterval >= 0 && m_lastInterval < n-1 && x >= a[m_lastInterval] && x < a[m_lastInterval+1]) {
    return m_lastInterval;
  }

  if(x < a[0] || x > a[n-1]) {
    throwException(_T("x=%lf is outside interval of interpolation =[%lf;%lf]"),x,a[0],a[n-1]);
  }
  for(int i = 1; i < n; i++) {
    if(a[i] >= x) {
      m_lastInterval = i-1;
      return m_lastInterval;
    }
  }
  return 0; // error
}

Real CommonSpline::operator()(const Real &x) {
  int i = findInterval(x);
  Real h = x - a[i];
  return yd(i,0)+h*(yd(i,1)+h*(yd(i,2)/2.0+h*yd(i,3)/6.0));
}

Real CommonSpline::operator()(int d, const Real &x) {
  int i = findInterval(x);
  Real h = x - a[i];

  switch(d) {
  case 0: return (*this)(x);
  case 1: return yd(i,1) + h*(yd(i,2) + h*yd(i,3) / 2);
  case 2: return yd(i,2) + h*yd(i,3);
  case 3: return yd(i,3);
  default: return 0;
  }
}

static const TCHAR *ctext_code1 =
_T("class Spline {\n"
   "private:\n"
   "  mutable int m_lastInterval;\n"
   "  int n;\n"
   "  const double **yd;\n"
   "  const double *a;\n"
   "  int findInterval(double x) const;\n"
   "public:\n"
   "  Spline(int _n, const double **_yd, const double *_a);\n"
   "  double evaluate(double x) const;\n"
   "};\n"
   "\n"
   "Spline::Spline(int _n, const double **_yd, const double *_a) {\n"
   "  n = _n;\n"
   "  yd = _yd;\n"
   "  a = _a;\n"
   "  m_lastInterval = -1;\n"
   "}\n"
   "\n"
   "int Spline::findInterval(double x) const {\n"
   "  if(m_lastInterval >= 0 && m_lastInterval < n && x >= a[m_lastInterval] && x < a[m_lastInterval+1]) \n"
   "    return m_lastInterval;\n"
   "  if(x < a[0] || x > a[n-1])\n"
   "    return -1;\n"
   "  for(int i = 1; i < n; i++) {\n"
   "    if(a[i] >= x) {\n"
   "      m_lastInterval = i-1;\n"
   "      return m_lastInterval;\n"
   "    }\n"
   "  }\n"
   "  return -1;\n"
   "}\n"
   "\n"
   "double Spline::evaluate(double x) const {\n"
   "  int i = findInterval(x);\n"
   "  if(i < 0) return 0;\n"
   "  double h = x - a[i];\n"
   "  return yd[0][i]+h*(yd[1][i]+h*(yd[2][i]/2.0+h*yd[3][i]/6.0));\n"
   "}\n"
   "\n");

static String declareData(const String &name, const Vector &v) {
  String result = format(_T("static double %s[] = {\n"),name.cstr());
  for(UINT i = 0; i < v.getDimension(); i++) {
    result += format(_T("  %.16le%s"),v[i],i < v.getDimension()-1?_T(",\n"):_T("\n"));
  }
  result += _T("};\n");
  return result;
}

String CommonSpline::generateCFunction() const {
  String result = ctext_code1;
  for(int c = 0; c < 4; c++) {
    String name = format(_T("_yd%d"),c);
    result += declareData(name,yd.getColumn(c));
  }
  result += "static const double *_yd[] = { _yd0,_yd1,_yd2,_yd3 };\n";
  result += declareData("_a",a);
  result += format(_T("Spline spline(%zu,_yd,_a);\n"),a.getDimension());
  return result;
}
