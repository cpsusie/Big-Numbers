#include "pch.h"
#include <Math/Polynomial.h>

/*
Notes:
The computed cubic spline coefficients are as follows:
yd[i,0] = y(x[i])    (the value of y at x = x[i])
yd[i,1] = y'(x[i])   (the 1st derivative of y at x = x[i])
yd[i,2] = y''(x[i])  (the 2nd derivative of y at x = x[i])
yd[i,3] = y'''(x[i]) (the 3rd derivative of y at x = x[i])

To evaluate y(x) for x between x[i] and x[i+1] and h = x-x[i],
use the computed coefficients as follows:
y(x) = yd[i,0]+h*(yd[i,1]+h*(yd[i,2]/2.0+h*yd[i,3]/6.0))

Akima's method provides continuous 1st derivatives, b
ut 2nd and
3rd derivatives are discontinuous.  Akima's method is not linear,
in that the interpolation of the sum of two functions is not the
same as the sum of the interpolations.

The Fritsch-Carlson method yields continuous 1st derivatives, but 2nd
and 3rd derivatives are discontinuous.  The method will yield a
monotonic interpolant for monotonic data.  1st derivatives are set
to zero wherever first divided differences change sign.

The method used by "csplin" yields continuous 1st and 2nd derivatives.

******************************************************************************
References:
See Akima, H., 1970, A new method for
interpolation and smooth curve fitting based on local procedures,
Journal of the ACM, v. 17, n. 4, p. 589-602.

For more information, see Fritsch, F. N., and Carlson, R. E., 1980,
Monotone piecewise cubic interpolation:  SIAM J. Numer. Anal., v. 17,
n. 2, p. 238-246.
Also, see the book by Kahaner, D., Moler, C., and Nash, S., 1989,
Numerical Methods and Software, Prentice Hall.  This function was
derived from SUBROUTINE PCHEZ contained on the diskette that comes
with the book.

For more general information on spline functions of all types see the book by:
Greville, T.N.E, 1969, Theory and Applications of Spline Functions,
Academic Press.
*/
AkimaSpline::AkimaSpline(const CompactArray<DataPoint> &data) {
  m_ok = false;
  if(data.size() == 0) {
    return;
  }
  Vector y;
  int n = preprocessdata(data,a,y);

  yd.setDimension(n,4);
  Real sumw,yd1fx,yd1lx,dx,divdf3;

  /* copy ordinates into output Array */
  for(int i = 0; i < n; i++)
    yd(i,0) = y[i];

  m_ok = true;
  /* if n=1, then use constant interpolation */
  if (n==1) {
    yd(0,1) = 0.0;
    yd(0,2) = 0.0;
    yd(0,3) = 0.0;
    return;

  /* else, if n=2, then use linear interpolation */
  }
  else if (n==2) {
    yd(0,1) = yd(1,1) = (y[1]-y[0])/(a[1]-a[0]);
    yd(0,2) = yd(1,2) = 0.0;
    yd(0,3) = yd(1,3) = 0.0;
    return;
  }

  /* compute 1st divided differences and store in yd[.,2] */
  for(int i = 1; i < n; i++)
    yd(i,2) = (y[i]-y[i-1])/(a[i]-a[i-1]);

  /* compute weights and store in yd[.,3] */
  for(int i = 1; i < n-1; i++)
    yd(i,3) = fabs(yd(i+1,2)-yd(i,2));
  yd(0,3) = yd(1,3);
  yd(n-1,3) = yd(n-2,3);

  /* compute 1st derivative at first x */
  sumw = yd(1,3)+yd(0,3);
  yd1fx = 2.0*yd(1,2)-yd(2,2);
  if(sumw!=0.0)
    yd(0,1) = (yd(1,3)*yd1fx+yd(0,3)*yd(1,2))/sumw;
  else
    yd(0,1) = 0.5*(yd1fx+yd(1,2));

  /* compute 1st derivatives in interior as weighted 1st differences */
  for(int i = 1; i < n-1; i++) {
    sumw = yd(i+1,3)+yd(i-1,3);
    if (sumw!=0.0)
      yd(i,1) = (yd(i+1,3)*yd(i,2)+yd(i-1,3)*yd(i+1,2))/sumw;
    else
      yd(i,1) = 0.5*(yd(i,2)+yd(i+1,2));
  }

  /* compute 1st derivative at last x */
  sumw = yd(n-2,3)+yd(n-1,3);
  yd1lx = 2.0*yd(n-1,2)-yd(n-2,2);
  if(sumw != 0.0)
    yd(n-1,1) = (yd(n-2,3)*yd1lx+yd(n-1,3)*yd(n-1,2))/sumw;
  else
    yd(n-1,1) = 0.5*(yd(n-1,2)+yd1lx);

  /* compute 2nd and 3rd derivatives of cubic polynomials */
  for(int i = 1; i < n; i++) {
    dx = a[i]-a[i-1];
    divdf3 = yd(i-1,1)+yd(i,1)-2.0*yd(i,2);
    yd(i-1,2) = 2.0*(yd(i,2)-yd(i-1,1)-divdf3)/dx;
    yd(i-1,3) = (divdf3/dx)*(6.0/dx);
  }
  yd(n-1,2) = yd(n-2,2)+(a[n-1]-a[n-2])*yd(n-2,3);
  yd(n-1,3) = yd(n-2,3);
}

AbstractDataFit *AkimaSpline::clone() const {
  return new AkimaSpline(*this);
}
