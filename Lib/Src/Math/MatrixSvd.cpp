#include "pch.h"
#include <Math/MathException.h>
#include <Math/Matrix.h>

#define Sign(u,v) ( (v)>=0.0 ? fabs(u) : -fabs(u) )

static Real radius(Real u, Real v) {
  u = fabs(u);
  v = fabs(v);

  if(u > v) {
    Real q = v / u;
    return u * sqrt(1.0 + q*q);
  } else if(v != 0) {
    Real q = u / v;
    return v * sqrt(1.0 + q*q);
  } else {
    return 0.0;
  }
}

/*************************** svdDecompose ***********************************
* Given matrix A[m][n], m>=n, using svd decomposition A = U D V' to get     *
* U[m][n], D[n][n] and V[n][n], where U occupies the position of A.         *
* NOTE: if m<n, A should be filled up to square with zero rows.             *
*       A[m][n] has been destroyed by U[m][n] after the decomposition.      *
****************************************************************************/
static void svdDecompose(Matrix &a, Vector &d, Matrix &v) {
  DEFINEMETHODNAME;
  const int m   = (int)a.getRowCount();
  const int n   = (int)a.getColumnCount();
  const int nm1 = n - 1;
  const int mm1 = m - 1;
  int  i, j, k, l, nm;
  Real g     = 0;
  Real scale = 0;
  Real anorm = 0.0;
  Real c, f, h, s, z;

  if(m < n) {
    throwMathException(_T("%s:Matrix a must be augmented with extra zero rows. Dimension=(%d,%d)."), method, m, n);
  }

  Vector rv1(n);

  // Householder reduction to bidigonal form
  for(i = 0; i < n; i++) {
    l = i + 1;
    rv1[i] = scale * g;
    g = s = scale = 0;
    if(i < m) {
      for(k = i; k < m; k++) {
        scale += fabs(a(k,i));
      }
      if(scale != 0) {
        for(k = i; k < m; k++) {
          a(k,i) /= scale;
          s += a(k,i) * a(k,i);
        }
        f = a(i,i);
        g = -Sign(sqrt(s), f);
        h = f * g - s;
        a(i,i) = f - g;
        if(i != nm1) {
          for(j = l; j < n; j++) {
            for(s = 0, k = i; k < m; k++) {
              s += a(k,i) * a(k,j);
            }
            f = s / h;
            for(k = i; k < m; k++) {
              a(k,j) += f * a(k,i);
            }
          }
        }
        for(k = i; k < m; k++) {
          a(k,i) *= scale;
        }
      }
    }
    d[i] = scale * g;
    g = s = scale = 0;
    if(i < m && i != nm1) {
      for(k = l; k < n; k++) {
        scale += fabs(a(i,k));
      }
      if(scale != 0) {
        for(k = l; k < n; k++) {
          a(i,k) /= scale;
          s += a(i,k) * a(i,k);
        }
        f = a(i,l);
        g = -Sign(sqrt(s), f);
        h = f * g - s;
        a(i,l) = f - g;
        for(k = l; k < n; k++) {
          rv1[k] = a(i,k) / h;
        }
        if(i != mm1) {
          for(j = l; j < m; j++) {
            for(s = 0, k = l; k < n; k++) {
              s += a(j,k) * a(i,k);
            }
            for(k = l; k < n; k++) {
              a(j,k) += s * rv1[k];
            }
          }
        }
        for(k = l; k < n; k++) {
          a(i,k) *= scale;
        }
      }
    }
    anorm = dmax(anorm, (fabs(d[i]) + fabs(rv1[i])));
  }

  // Accumulation of right-hand transformations
  for(i = n - 1; i >= 0; i--) {
    if(i < nm1) {
      if(g != 0) { // double division to avoid possible underflow
        for(j = l; j < n; j++) {
          v(j,i) = (a(i,j) / a(i,l)) / g;
        }
        for(j = l; j < n; j++) {
          for(s = 0, k = l; k < n; k++) {
            s += a(i,k) * v(k,j);
          }
          for(k = l; k < n; k++) {
            v(k,j) += s * v(k,i);
          }
        }
      }
      for(j = l; j < n; j++) {
        v(i,j) = v(j,i) = 0;
      }
    }
    v(i,i) = 1;
    g = rv1[i];
    l = i;
  }

  // Accumulation of left-hand transformations
  for(i = n - 1; i >= 0; i--) {
    l = i + 1;
    g = d[i];
    if(i < nm1) {
      for(j = l; j < n; j++) {
        a(i,j) = 0;
      }
    }
    if(g != 0) {
      g = 1.0 / g;
      if(i != nm1) {
        for(j = l; j < n; j++) {
          for(s = 0.0, k = l; k < m; k++) {
            s += a(k,i) * a(k,j);
          }
          f = (s / a(i,i)) * g;
          for(k = i; k < m; k++) {
            a(k,j) += f * a(k,i);
          }
        }
      }
      for(j = i; j < m; j++) {
        a(j,i) *= g;
      }
    } else {
      for(j = i; j < m; j++) {
        a(j,i) = 0;
      }
    }
    a(i,i) += 1;
  }

//  cout << "bidiagonalForm:\n" << a << "\n";

#define MAXITERATION 40
  // diagonalization of the bidigonal form
  for(k = n - 1; k >= 0; k--) { // loop over singlar values
    int iteration;
    for(iteration = 0; iteration < MAXITERATION; iteration++) { // loop over allowed iterations
      bool flag = true;
      for(l = k; l >= 0; l--) { // test for splitting
        nm = l - 1;     // note that rv1[l] is always zero
        if(fabs(rv1[l]) + anorm == anorm) {
          flag = false;
          break;
        }
        if(fabs(d[nm]) + anorm == anorm) {
          break;
        }
      }
      if(flag) {
        c = 0;        // cancellation of rv1[l], if l>1
        s = 1;
        for(i = l; i <= k; i++) {
          f = s * rv1[i];
          if(fabs(f) + anorm != anorm) {
            g = d[i];
            h = radius(f, g);
            d[i] = h;
            h = 1.0 / h;
            c = g * h;
            s = -f * h;
            for(j = 0; j < m; j++) {
              Real y = a(j,nm);
              z = a(j,i);
              a(j,nm) = y * c + z * s;
              a(j,i)  = z * c - y * s;
            }
          }
        }
      }
      z = d[k];
      if(l == k) { // convergence
        if(z < 0.0) {
          d[k] = -z;
          for(j = 0; j < n; j++) {
            v(j,k) = -v(j,k);
          }
        }
        break;
      }
      Real x = d[l];           // shift from bottom 2-by-2 minor
      nm = k - 1;
      Real y = d[nm];
      g = rv1[nm];
      h = rv1[k];
      f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
      g = radius(f, 1.0);

      // next QR transformation
      f = ((x - z) * (x + z) + h * ((y / (f + Sign(g, f))) - h)) / x;
      c = s = 1;
      for(j = l; j <= nm; j++) {
        i = j + 1;
        g = rv1[i];
        y = d[i];
        h = s * g;
        g = c * g;
        z = radius(f, h);
        rv1[j] = z;
        c = f / z;
        s = h / z;
        f = x * c + g * s;
        g = g * c - x * s;
        h = y * s;
        y = y * c;
        for(int jj = 0; jj < n; jj++) {
          x = v(jj,j);
          z = v(jj,i);
          v(jj,j) = x * c + z * s;
          v(jj,i) = z * c - x * s;
        }
        z = radius(f, h);
        d[j] = z;       // rotation can be arbitrary if z=0
        if(z != 0) {
          z = 1.0 / z;
          c = f * z;
          s = h * z;
        }
        f = (c * g) + (s * y);
        x = (c * y) - (s * g);
        for(int jj = 0; jj < m; jj++) {
          y = a(jj,j);
          z = a(jj,i);
          a(jj,j) = y * c + z * s;
          a(jj,i) = z * c - y * s;
        }
      }
      rv1[l] = 0.0;
      rv1[k] = f;
      d[k] = x;
    }

//    cout << "k:" << k << " Iterations:" << iteration << "\n";

    if(iteration == MAXITERATION) {
      throwException(_T("%s:No convergence in %d iterations"), method, MAXITERATION);
    }
  }
}

/******************************* svdSolve ***************************
* Given A[m][n], b[m], solves A x = b in the svd form U D V'x = b   *
* so x = V U'b/D                                                    *
* No input quantities are destroyed, so sequential calling is OK    *
********************************************************************/
static Vector svdSolve(const Matrix &u, const Vector &d, const Matrix &v, const Vector &b) {
  const int m = (int)u.getRowCount();
  const int n = (int)u.getColumnCount();

  Vector tmp(n);
  for(int j = 0; j < n; j++) { // calculate <U,b>
    Real s = 0;
    if(d[j] != 0) {        // nonzero result only if nonzero d[j]
      for(int i = 0; i < m; i++) {
        s += u(i,j) * b[i];
      }
      s /= d[j];            // premultiply by inverse D
    }
    tmp[j] = s;
  }

  Vector result(n);
  for(int j = 0; j < n; j++) { // V(U'b/D)
    Real s = 0;
    for(int jj = 0; jj < n; jj++) {
      s += v(j,jj) * tmp[jj];
    }
    result[j] = s;
  }
  return result;
}

DEFINECLASSNAME(SVDDecomposition);

SVDDecomposition::SVDDecomposition(const Matrix &a) : m_u(a) {
  const int c = (int)a.getColumnCount();
  m_d.setDimension(c);
  m_v.setDimension(c,c);
  svdDecompose(m_u,m_d,m_v);
}

Vector SVDDecomposition::solve(const Vector &b) const {
  DEFINEMETHODNAME;
  const int dim = (int)b.getDimension();
  if(dim != m_u.getRowCount()) {
    throwInvalidArgumentException(method
                                 ,_T("Invalid dimension u.%s. b.%s")
                                 , m_u.getDimensionString().cstr()
                                 , b.getDimensionString().cstr());
  }
  return svdSolve(m_u,m_d,m_v,b);
}
