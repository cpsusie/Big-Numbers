#include "pch.h"
#include <Math/MathLib.h>
#include <Math/Rectangle2D.h>

// Regula falsi. assume sign(f(l)) == -sign(f(r)) => f(l) != 0 && f(r) != 0
static Real converge(Function &f, const Real &l, const Real &r) {
  Real       x1       = l, y1 = f(x1);
  Real       x2       = r, y2 = f(x2);
  Real       bestX, bestE;
  if(fabs(y1) < fabs(y2)) {
    bestX = x1;
    bestE = fabs(y1);
  } else {
    bestX = x2;
    bestE = fabs(y2);
  }
  const int s1 = sign(y1);

  for(int i = 0; i < 40; i++) {
    const Real x0 = (x1 * y2 - x2 * y1) / (y2 - y1);
    const Real y0 = f(x0);
//    debugLog(_T("x1=% .15le y1=% .15le x2=% .15le y2=% .15le x0=% .5le y0=% .15le best(X,E):(% .15le,%.15le)\n")
//            ,x1,y1,x2,y2,x0,y0,bestX,bestE);
    if(y0 == 0) {
      return x0;
    } else {
      const Real e = fabs(y0);
      if(e < bestE) {
        bestX = x0;
        bestE = e;
      }
    }
    if(sign(y0) == s1) {
      x1 = x0;
    } else {
      x2 = x0;
    }
  }
  return bestX;
}

#define SEARCHCOUNT 1000
CompactRealArray findZeroes(Function &f, const RealInterval &i) {
  CompactRealArray result;
  RealPoint2DArray pointArray(SEARCHCOUNT);
  Real             step = i.getLength() / (SEARCHCOUNT-1);
  Real             x    = i.getFrom();
  for(int t = 0; t < SEARCHCOUNT-1; t++, x += step) {
    try {
      pointArray.add(RealPoint2D(x,f(x)));
    } catch (...) {
      // ignore
    }
  }
  try {
    pointArray.add(RealPoint2D(i.getTo(), f(i.getTo())));
  } catch (...) {
    // ignore
  }
  const size_t       n     = pointArray.size();
  const RealPoint2D *lastp = &pointArray[0];
  for(size_t t = 1; t < n; t++) {
    const RealPoint2D &p = pointArray[t];
    if(sign(lastp->y) * sign(p.y) != 1) {
      if(lastp->y == 0) {
        result.add(lastp->x);
      } else if(p.y != 0) { // lastp->y != 0 && p.y != 0 => opposite sign
        result.add(converge(f,lastp->x, p.x));
      }
    }
    lastp = &p;
  }
  if(lastp->y == 0) {
    result.add(lastp->x);
  }
  return result;
}
