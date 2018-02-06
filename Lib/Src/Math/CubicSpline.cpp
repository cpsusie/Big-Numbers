#include "pch.h"
#include <Math/Polynomial.h>

static int dpointcmp(const DataPoint &d1, const DataPoint &d2) {
  return sign(d1.x - d2.x);
}

CubicSpline::CubicSpline(FittingType type, const CompactArray<DataPoint> &data, const Real &dydx1, const Real &dydx2) {
  m_ok = false;
  if(data.size() < 2) {
    return;
  }
  CompactArray<DataPoint> tmp(data);
  tmp.sort(dpointcmp);
  CompactArray<DataPoint> copy;
  copy.add(tmp[0]);
  int c = 1;
  for(size_t i = 1; i < tmp.size(); i++) {
    if(tmp[i].x == copy.last().x) {
      copy.last().y += tmp[i].y;
      c++;
    } else {
      copy.last().y /= c;
      copy.add(tmp[i]);
      c = 1;
    }
  }
  copy.last().y /= c;

  const size_t n = copy.size();
  if(n < 2)
    return;

  a.setDimension(n);
  y.setDimension(n);
  h.setDimension(n);
  for(size_t i = 0; i < n; i++) {
    a[i] = copy[i].x;
    y[i] = copy[i].y;
  }
  for(size_t i = 1; i < n; i++)
    h[i] = a[i] - a[i-1];

  Vector sigma(n);
  Vector lambda(n);
  Vector my(n);
  Vector d(n);

  for(size_t i = 1; i < n; i++) {
    sigma[i] = (y[i] - y[i-1]) / h[i];
  }

  for(size_t i = 1; i < n-1; i++) {
    lambda[i] = h[i+1] /(h[i] + h[i+1]);
    my[i] = 1 - lambda[i];
    d[i] = 6.0 * (sigma[i+1] - sigma[i]) / (h[i] + h[i+1]);
  }
  switch(type) {
  case NATURAL_SPLINE       :
    lambda[0] = d[0] = my[n-1] = d[n-1] = 0;
    break;
  case DYDXMATCH_SPLINE     :
    lambda[0] = my[n-1] = 1;
    d[0]   = 6.0 * ((y[1] - y[0])/h[1] - dydx1) / h[1];
    d[n-1] = 6.0 * (dydx2 - (y[n-1] - y[n-2])/h[n-1]) / h[n-1];
    break;
  case IGNOREENDPOINT_SPLINE:
    lambda[0] = d[0] = my[n-1] = d[n-1] = 0; // TBD
    break;
  }

  Vector diag(n);
  for(size_t i = 0 ; i < n; i++) {
    diag[i] = 2;
  }

  M = solve3diag(lambda,diag,my,d);

  m_lastInterval = -1;
  m_ok = true;
}

AbstractDataFit *CubicSpline::clone() const {
  return new CubicSpline(*this);
}

int CubicSpline::findInterval(const Real &x) const {
  int n = (int)a.getDimension();
  if(m_lastInterval > 1 && m_lastInterval < n && x >= a[m_lastInterval-1] && x <= a[m_lastInterval]) {
    return m_lastInterval;
  }

  if(x < a[0] || x > a[n-1]) {
    throwException(_T("x=%lf is outside interval of interpolation =[%lf;%lf]"),x,a[0],a[n-1]);
  }
  for(int i = 1; i < n; i++) {
    if(a[i] >= x) {
      m_lastInterval = i;
      return i;
    }
  }
  return 0; // error
}

static Real P3(const Real &x) {
  return x * x * x;
}

Real CubicSpline::operator()(const Real &x) {
  int    i    = findInterval(x);
  Real   hi   = h[i];
  Real   hi2  = sqr(hi);
  Real   hx   = a[i]-x;
  Real   hxm1 = x-a[i-1];
  Real   res  = M[i-1] * P3(hx  )/6;
         res += M[i  ] * P3(hxm1)/6;
         res += (y[i-1]-M[i-1]*hi2/6) * hx  ;
         res += (y[i]-M[i]*hi2/6) * hxm1;

  return res / hi;
}

Real CubicSpline::operator()(int d, const Real &x) {
  int    i  = findInterval(x);
  Real hi = h[i];
  Real res;
  switch(d) {
  case 0:
    return (*this)(x);
  case 1:
    res  = -M[i-1]*sqr(a[i]-x)/2/hi;
    res +=  M[i]*sqr(x-a[i-1])/2/hi;
    res += (y[i]-y[i-1])/hi;
    res -= (M[i]-M[i-1])/6*hi;
    return res;
  case 2:
    return (M[i-1]*(a[i]-x) + M[i]*(x-a[i-1]))/hi;
  case 3:
    return (M[i] - M[i-1]) / hi;
  default:
    return 0;
  }
}

NumberInterval<Real> CubicSpline::getDomain() const {
  return NumberInterval<Real>(a[0], a[a.getDimension()-1]);
}


static const TCHAR *ctext_code1 =
_T("class CubicSpline {\n"
   "private:\n"
   "  mutable int m_lastInterval;\n"
   "  int n;\n"
   "  const double *h;\n"
   "  const double *a;\n"
   "  const double *M;\n"
   "  cosnt double *y;\n"
   "  int findInterval(double x) const;\n"
   "public:\n"
   "  CubicSpline(int _n, const double *_h, const double *_a, const double *_M, const double *_y);\n"
   "  double evaluate(double x) const;\n"
   "};\n"
   "\n"
   "CubicSpline::CubicSpline(int _n, const double *_h, const double *_a, const double *_M, const double *_y) {\n"
   "  n = _n;\n"
   "  h = _h;\n"
   "  a = _a;\n"
   "  M = _M;\n"
   "  y = _y;\n"
   "  m_lastInterval = -1;\n"
   "}\n"
   "\n"
   "int CubicSpline::findInterval(double x) const {\n"
   "  if(m_lastInterval > 1 && m_lastInterval < n && x >= a[m_lastInterval-1] && x <= a[m_lastInterval]) \n"
   "    return m_lastInterval;\n"
   "  if(x < a[0] || x > a[n-1])\n"
   "    return 0;\n"
   "  for(int i = 1; i < n; i++) {\n"
   "    if(a[i] >= x) {\n"
   "      m_lastInterval = i;\n"
   "      return i;\n"
   "    }\n"
   "  }\n"
   "  return 0;\n"
   "}\n"
   "\n"
   "double CubicSpline::evaluate(double x) const {\n"
   "  int    i    = findInterval(x);\n"
   "  if(i == 0) return 0;\n"
   "  double hi   = h[i];\n"
   "  double hi2  = hi*hi;\n"
   "  double hx   = a[i]-x;\n"
   "  double hxm1 = x - a[i-1];\n"
   "  double res  = M[i-1]*hx*hx*hx/6;\n"
   "         res += M[i] * hxm1*hxm1*hxm1/6;\n"
   "         res += (y[i-1]-M[i-1]*hi2/6) * hx;\n"
   "         res += (y[i]-M[i]*hi2/6) * hxm1;\n"
   "  return res / hi;\n"
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

String CubicSpline::generateCFunction() const {
  String result = ctext_code1;
  result += declareData("_h",h);
  result += declareData("_a",a);
  result += declareData("_M",M);
  result += declareData("_y",y);
  result += format(_T("CubicSpline spline(%zu,_h,_a,_M,_y);\n"),h.getDimension());
  return result;
}



