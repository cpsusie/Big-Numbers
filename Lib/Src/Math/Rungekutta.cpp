#include "pch.h"
#include "Math/Matrix.h"

/*
  Implements Runge-Kutta algorithm of 4. degree as described
  "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 217

 v_new = v_old + dv
     dv = 1/6 * ( k1 + 2*k2 + 2*k3 + k4 )
     k1 = step * f( x_old           , y_old          )
     k2 = step * f( x_old + 0.5*step, y_old + 0.5*k1 )
     k3 = step * f( x_old + 0.5*step, y_old + 0.5*k2 )
     k4 = step * f( x_old +   1*step, y_old +   1*k3 )

     x = v[0] og y = v[1..dim-1]
     That is k = diff(x,y) is k = diff(v)
*/

static void rungeKutta(const Vector &v, const Real &step, Vector &dv, VectorFunction &diff) {
  Vector k1(v.getDimension()), k2(v.getDimension()), k3(v.getDimension()), k4(v.getDimension());

  k1 = diff( v );
  k1[0] = 0.0;
  k1 *= step;

  k2 = k1;
  k2[0] = step;
  k2 = diff( v + 0.5 * k2 );
  k2[0] = 0.5;
  k2 *= step;

  k3 = k2;
  k3[0] = step;
  k3 = diff( v + 0.5 * k3 );
  k3[0] = 0.5;
  k3 *= step;

  k4 = k3;
  k4[0] = step;
  k4 = diff( v + k4 );
  k4[0] = 1.0;
  k4 *= step;

  dv = (k1 + 2.0 * k2 + 2.0 * k3 + k4 ) / 6.0;
  dv[0] = step;
}


void rungeKuttaStep(Vector &v, Real step, VectorFunction &diff) {
  Vector dv(v.getDimension());
  rungeKutta( v, step, dv, diff);
  v += dv;
}

