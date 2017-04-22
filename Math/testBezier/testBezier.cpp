#include "stdafx.h"
#include <Math/MathFunctions.h>
#include <Math/Matrix.h>
#include <Math/Rectangle2D.h>

class BinomialTable {
private:
  static Array<CompactDoubleArray> coefMatrix;
public:
  BinomialTable();
  double operator()(UINT n, UINT k) const;
};

Array<CompactDoubleArray> BinomialTable::coefMatrix;

BinomialTable::BinomialTable() {
  coefMatrix.add(CompactDoubleArray(1));
  coefMatrix.last().add(1); // define binmomial(0,0)
}

double BinomialTable::operator()(UINT n, UINT k) const {
  if(k > n) {
    return 0;
  } else if(n >= coefMatrix.size()) {
    for(UINT i = (UINT)coefMatrix.size(); i <= n; i++) {
      coefMatrix.add(CompactDoubleArray(i/2+1));
      CompactDoubleArray &c = coefMatrix.last();
      for(UINT k = 0; k <= i/2; k++) {
        c.add((*this)(i-1,k)+(*this)(i-1,k-1));
      }
    }
  }
  return (k <= n/2) ? coefMatrix[n][k] : coefMatrix[n][n-k];
}

class BezierCurve {
private:
  BinomialTable m_binomial;
public:
  Point2D BezierCurve::getPoint(const Point2DArray &pa, double t) const;
  void printCurve(const Point2DArray &pa, int steps, FILE *out = stdout); // output n-order beziercurve, n = #controlpoins - 1
};

Point2D BezierCurve::getPoint(const Point2DArray &pa, double t) const { // assume 0 <= t <= 1
  if(t <= 0) {
    return pa[0];
  } else if(t >= 1) {
    return pa.last();
  } else {                        // 0 < t < 1 => t1 != 0
    const int    n  = (int)pa.size() - 1;
    const double t1 = 1.0-t;
    const double q  = t / t1;
    Point2D sum(0, 0);
    double tt1Factor = pow(t1, n); // == t^k * (1-t)^(n-k)
    for(int k = 0; k <= n; k++, tt1Factor *= q) {
      sum += m_binomial(n,k) * tt1Factor * pa[k];
    }
    return sum;
  }
}

void BezierCurve::printCurve(const Point2DArray &pa, int steps, FILE *out) { // output n-order beziercurve, n = #controlpoins - 1
  steps--;

#define OUTPUT(p)  _ftprintf(out, _T("%+.3lf %+.3lf\n"), p.x,p.y)
  for(int i = 0; i <= steps; i++) {
    const Point2D p = getPoint(pa, (double)i/steps);
    OUTPUT(p);
  }
}

#define TEST_BINOMIALTABLE

int main(int argc, char **argv) {
#ifdef TEST_BINOMIALTABLE

#define FIELDSIZE 8
  BinomialTable binomial;
  const int maxN = 23;
  for(int n = 0; n <= maxN; n++) {
    _tprintf(_T("n=%3d %*s"), n, FIELDSIZE*(maxN-n)/2,EMPTYSTRING);
    for(int k = 0; k <= n; k++) {
      const String vstr   = format(_T("%.0lf"), binomial(n,k));
      const int    vlen   = (int)vstr.length();
      const int    filler = (FIELDSIZE-1-vlen)/2;
      _tprintf(_T("%-*s%-*s"), filler,EMPTYSTRING , FIELDSIZE-filler, vstr.cstr());
    }
    _tprintf((n<maxN) ? _T("\n") : _T("\n\n"));
  }
  return 0;

#else

  try {
    BezierCurve curve;
    for(;;) {
      Point2DArray p;
      const int n = randInt(20) + 3;
      for(int i = 0; i < n; i++) {
        p.add(Point2D(random(-3,3),random(-3,3)));
      }
      const String pointFileName  = _T("c:\\temp\\bzPoints.txt");
      const String bezierFileName = _T("c:\\temp\\bezierCurve.txt");
      FILE *fp = FOPEN(pointFileName, _T("w"));
      for(i = 0; i < p.size(); i++) {
        _ftprintf(fp, _T("%lf %lf\n"), p[i].x, p[i].y);
      }
      fclose(fp);

  //    FILE *fbz = stdout;
      FILE *fbz = FOPEN(bezierFileName, _T("w"));
      curve.printCurve(p, 1000, fbz);
      fclose(fbz);
      system(format(_T("showgraf %s %s"), pointFileName.cstr(), bezierFileName.cstr()).cstr());
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
#endif

  return 0;
}
