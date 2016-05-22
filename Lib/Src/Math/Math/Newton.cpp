#include "pch.h"
#include <iostream>
#include <Math/MathException.h>

//#define DEBUGNEWTON


/* ------------------ Newton raphson iteration --------- */

/*  Implements Newton-Raphson iteration as described in
    "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 361
*/

/* computes x so that f(x) = 0 */
Vector newton(VectorFunction &f, const Vector &x0, int maxit, Real tolerance) {
  const int scaletimes = 5;
  int n = x0.getDimension();
  Vector result(x0);

  Vector y  = f(result);
  Real v1 = y.length();
  for(int it = 0; it < maxit; it++) {

#ifdef DEBUGNEWTON
cout << "IT:" << it << endl;
cout << "X :" << result << endl;
cout << "Y :" << y << "(length=" << v1 << ")\n";
#endif

    Matrix jacobi = getJacobi(f,result,y);

#ifdef DEBUGNEWTON
cout << "Jac:\n" << jacobi ;
#endif

    LUMatrix lu(jacobi);

    Vector help = lu.solve(y);

#ifdef DEBUGNEWTON
cout << "Help:" << help << endl;
#endif

    Vector oldx    = result;
    Vector oldhelp = help;

    int i;
    for(i = 0; i < 2 * scaletimes; i++) {
      result = oldx - help;
      y = f(result);
      Real v2 = y.length();
      if(v2 < v1) {

#ifdef DEBUGNEWTON
cout << "New estimate:" << result << endl;
#endif

        if(v2 < tolerance) {
#ifdef DEBUGNEWTON
cout << "IT:" << it << endl;
cout << "X:" << result << endl;
cout << "Y:" << y << "(length=" << v2 << ")\n";
#endif
          return result;
        }
        v1 = v2;
        break;
      }
      else {
        if(i==scaletimes) {
	      help = oldhelp;
	      help = help * -1.0;
#ifdef DEBUGNEWTON
cout << "change direction\n";
#endif
        }
#ifdef DEBUGNEWTON
cout << "scale down " << i << endl;
#endif
	      help = help * 0.5;
      }
    }
    if(i==2*scaletimes)
      throwMathException(_T("no convergens"));
  }
  throwMathException(_T("number of iterations exceed %d"),maxit);
  return result;
}

