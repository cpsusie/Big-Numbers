#include "pch.h"
#include <Math/RungekuttaFehlberg.h>

class RKFVector : public Vector {
public:
  RKFVector(Real t, const Vector &v) : Vector(v) {
    (*this)[0] = t;
  }
};

RungeKuttaFehlberg::RungeKuttaFehlberg(VectorFunction &diff,  RungeKuttaFehlbergHandler &handler)
: m_diff(diff)
, m_handler(handler)
{
}

void RungeKuttaFehlberg::setValue(const Vector &v) {
  m_y = v;
  m_handler.handleData(*this);
}

// rungekuttafehlberg method of order 4 and 5
// see http://math.fullerton.edu/mathews/n2003/RungeKuttaFehlbergMod.html
const Vector &RungeKuttaFehlberg::calculate(const Vector &start, Real v0end, Real maxError) {
  const Real a2 = 0.25,     b2 = 0.25;
  const Real a3 = 3.0/8.0,  b3 = 3.0/32,        c3 = 9.0/32;
  const Real a4 = 12.0/13,  b4 = 1932.0/2197,   c4 = -7200.0/2197,  d4 = 7296.0/2197;
  const Real a5 = 1,        b5 = 439.0/216,     c5 = -8,            d5 = 3680.0/513,    e5 = -845.0/4104;
  const Real a6 = 0.5,      b6 = -8.0/27,       c6 = 2,             d6 = 3544.0/2565,   e6 = 1859.0/4104,   f6 = -11.0/40;
  const Real r1 = 1.0/360,  r3 = -128.0/4275,   r4 = -2197.0/75240, r5 = 1.0/50,        r6 = 2.0/55;
  const Real n1 = 25.0/216, n3 = 1408.0/2565,   n4 = 2197.0/4104,   n5 = -1.0/5;

  const Real hmin = 0.001;
  const Real hmax = 0.250;
  const Real br   = v0end - 0.00001 * fabs(v0end);
  Real       h    = 0.25;
  Real       s;

  setValue(start);
  Vector k1,k2,k3,k4,k5,k6,ynew;
  while(m_y[0] < v0end) {
    if(m_y[0] + h > br) {
      h = v0end - m_y[0];
    }

    k1 = h * m_diff( m_y );
    k2 = h * m_diff( RKFVector(m_y[0] + a2*h, m_y + b2*k1) );
    k3 = h * m_diff( RKFVector(m_y[0] + a3*h, m_y + b3*k1 + c3*k2) );
    k4 = h * m_diff( RKFVector(m_y[0] + a4*h, m_y + b4*k1 + c4*k2 + d4*k3) );
    k5 = h * m_diff( RKFVector(m_y[0] + a5*h, m_y + b5*k1 + c5*k2 + d5*k3 + e5*k4) );
    k6 = h * m_diff( RKFVector(m_y[0] + a6*h, m_y + b6*k1 + c6*k2 + d6*k4 + e6*k4 + f6*k5) );

    const Real error = Vector(r1*k1 + r3*k3 + r4*k4 + r5*k5 + r6*k6).length();
    ynew = m_y; ynew += RKFVector(h, n1*k1 + n3*k3 + n4*k4 + n5*k5);
    if((error < maxError) || (h < 2 * hmin)) {
      setValue(ynew);
    }
    if(error == 0) {
      s = 0;
    } else {
      s = pow(maxError * h / (2*error), 0.25);
    }
    s = minMax(s    , Real(0.1) , Real(4.0));
    h = minMax(s * h, hmin      , hmax);
  }
  return m_y;
}

#ifdef TEST_MODULE

class expdiff : public VectorFunction {
public:
  Vector operator()(const Vector &x);
};

Vector expdiff::operator()(const Vector &v) {
/*
  Vector dv(v.dim());
  dv[0] = 0.0;
  dv[1] = 0.01 - (0.01 + v[1] + v[2]) * (1.0 + (v[1]+1000.0)*(v[1]+1.0));
  dv[2] = 0.01 - (0.01 + v[1] + v[2]) * (1.0 + v[2]*v[2]);
  return dv;
*/
  Vector dv(v.getDimension());
  dv[0] = 0.0;
  dv[1] = v[1];
  return dv;

}

int main(int argc, char **argv) {
  expdiff f;
  RungeKuttaFehlberg s(f);
  Vector v(2);
  v[0] = 0;
  v[1] = 1;
//  v[2] = 0;

  s.calculate(v,5,0.01);

  return 0;
}

#endif
