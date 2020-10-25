#include "pch.h"
#include <Math/MathLib.h>
#include <Math/Rectangle2D.h>

typedef PointComparator<Real, 2, 1> PointYComparator;

class ExtremumFinder {
private:
  Function           &m_f;
  const RealInterval &m_interval;
  PointYComparator    m_comparator;
  RealPoint2D findExtremum(const Real &l, const Real &r, int depth);

public:
  ExtremumFinder(Function &f, const RealInterval &interval, bool maximum)
    : m_f(f)
    , m_interval(interval)
    , m_comparator(maximum) {
  }
  RealPoint2D find() {
    return findExtremum(m_interval.getMin(), m_interval.getMax(), 0);
  }
};

static Real inverseInterpol(const RealPoint2D &p1, const RealPoint2D &p2, const RealPoint2D &p3) {
  const Real t = (p1.y()-p2.y())*(p2.x()*p2.x()-p3.x()*p3.x()) - (p2.y()-p3.y())*(p1.x()*p1.x()-p2.x()*p2.x());
  const Real d = (p1.y()-p2.y())*(p2.x()-p3.x())               - (p2.y()-p3.y())*(p1.x()-p2.x());
  return t / d / 2.0;
}

RealPoint2D ExtremumFinder::findExtremum(const Real &l, const Real &r, int depth) {

#define STEPCOUNT 20

  RealPoint2DArray plot;

  assert(l <= r);

  if(l == r) {
    return RealPoint2D(l, m_f(l));
  }
  Real x       = l;
  Real step    = (r - l) / STEPCOUNT;
  for(int count = 0; x < r && count < STEPCOUNT; x += step, count++) {
    assert(l <= x && x <= r );
    plot.add(RealPoint2D(x, m_f(x)));
  }
  plot.add(RealPoint2D(r, m_f(r)));

  plot.sort(m_comparator);

  try {
    x = inverseInterpol(plot[0], plot[1], plot[2]);
  } catch(Exception e) {
    x = l - 1;
  }

  if(isnan(x)) {
    return plot[0];
  }
  RealPoint2D p;
  if(x < l || x > r) {
    p = plot[0];
  } else {
    assert(l <= x && x <= r);
    p = RealPoint2D(x,m_f(x));
    if(m_comparator.compare(p, plot[0]) > 0) {
      p = plot[0];
    }
  }
  if(depth >= 9) {
    return p;
  } else {
    return findExtremum(p.x() - step/10,p.x() + step/10, depth+1);
  }
}

RealPoint2D findExtremum(Function &f, const RealInterval &interval, bool maximum) {
  return ExtremumFinder(f, interval, maximum).find();
}
