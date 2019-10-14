#include "pch.h"
#include <Math/MathException.h>
#include <Math/Polynomial.h>

using namespace std;

DataPoint::DataPoint() {
  x = y = w = 0;
}

DataPoint::DataPoint(const Real &x, const Real &y, const Real &w) {
  this->x = x;
  this->y = y;
  this->w = w;
}

DataPoint::DataPoint(const Point2D &p) {
  x = p.x;
  y = p.y;
  w = 1;
}

String DataPoint::toString(StreamSize precision) const {
  return format(_T("(%s,%s,%s)")
               ,::toString(x, precision).cstr()
               ,::toString(y, precision).cstr()
               ,::toString(w, precision).cstr()
               );
}

Polynomial::Polynomial(int degree) {
  m_coef.setDimension(degree+1);
}

Polynomial::Polynomial(const Vector &coef) {
  m_coef.setDimension(coef.getDimension());
  for(UINT i = 0; i < coef.getDimension(); i++) {
    m_coef[i] = coef[i];
  }
  trimLeadingZeroes();
}

Polynomial::Polynomial(const ComplexVector &coef) {
  m_coef = coef;
  trimLeadingZeroes();
}

Complex Polynomial::operator()(const Complex &x) {
  Complex result(0);
  for(int i = getDegree(); i >= 0; i--) {
    result = result * x + m_coef[i];
  }
  return result;
}

Polynomial Polynomial::operator+(const Polynomial &rhs) const {
  int newDegree = max(getDegree(),rhs.getDegree());
  Polynomial result(newDegree);
  for(int i = 0; i <= newDegree; i++) {
    if(i > getDegree()) {
      result.m_coef[i] = rhs.m_coef[i];
    } else if(i > rhs.getDegree()) {
      result.m_coef[i] = m_coef[i];
    } else {
      result.m_coef[i] = m_coef[i] + rhs.m_coef[i];
    }
  }
  return result.trimLeadingZeroes();
}

Polynomial Polynomial::operator-(const Polynomial &rhs) const {
  int newDegree = max(getDegree(),rhs.getDegree());
  Polynomial result(newDegree);
  for(int i = 0; i <= newDegree; i++) {
    if(i > getDegree()) {
      result.m_coef[i] = -rhs.m_coef[i];
    } else if(i > rhs.getDegree()) {
      result.m_coef[i] = m_coef[i];
    } else {
      result.m_coef[i] = m_coef[i] - rhs.m_coef[i];
    }
  }
  return result.trimLeadingZeroes();
}

Polynomial Polynomial::operator*(const Polynomial &rhs) const {
  Polynomial result(getDegree() + rhs.getDegree());
  for(int i = 0; i <= getDegree(); i++) {
    for(int j = 0; j <= rhs.getDegree(); j++) {
      result.m_coef[i+j] += m_coef[i] * rhs.m_coef[j];
    }
  }
  return result.trimLeadingZeroes();
}

Polynomial Polynomial::operator/(const Polynomial &rhs) const {
  Polynomial rem(*this);
  return div(*this,rhs,rem);
}

Polynomial &Polynomial::trimLeadingZeroes() {
  int i;
  for(i = getDegree(); i >= 0; i--) {
    if(arg(m_coef[i]) != 0) {
      break;
    }
  }
  if(i < 0) {
    m_coef.setDimension(1);
    m_coef[0] = 0;
  } else if(i < getDegree()) {
    m_coef.setDimension(i+1);
  }
  return *this;
}

Polynomial div(const Polynomial &p, const Polynomial &d, Polynomial &rem) {
  rem = p;
  int remDegree = rem.getDegree();
  int dDegree   = d.getDegree();
  Polynomial result(remDegree - dDegree);
  while(remDegree >= dDegree) {
    Complex l = rem.getCoef(remDegree) / d.getCoef(dDegree);
    int lp = remDegree - dDegree;
    result.setCoef(lp,l);
    Polynomial q(remDegree);
    for(int i = 0; i <= dDegree; i++) {
      q.setCoef(i+lp, l * d.getCoef(i));
    }
    rem = rem - q;
    rem.setCoef(remDegree,Complex(0.0));
    rem.trimLeadingZeroes();
    remDegree = rem.getDegree();
  }
  return result;
}

Polynomial Polynomial::operator*(const Complex &rhs) const {
  Polynomial result(*this);
  for(int i = 0; i <= getDegree(); i++) {
    result.m_coef[i] *= rhs;
  }
  return result.trimLeadingZeroes();
}

Polynomial Polynomial::operator/(const Complex &rhs) const {
  Polynomial result(*this);
  for(int i = 0; i <= getDegree(); i++) {
    result.m_coef[i] /= rhs;
  }
  return result;
}

Polynomial Polynomial::dfdx() const {
  Polynomial result(getDegree()-1);
  for(int i = 1; i <= getDegree(); i++) {
    result.setCoef(i-1,getCoef(i) * Complex(i));
  }
  return result.trimLeadingZeroes();
}

// Divides the Polynomial p(x) by (x-root). That is Polynomial-division
Polynomial& Polynomial::operator/=(const Complex &root) {
  Complex q = m_coef[getDegree()];
  Polynomial newPoly(getDegree()-1);
  for(int i = getDegree(); i > 0; i-- ) {
    newPoly.setCoef(i-1,q);
    q = m_coef[i-1] + q * root;
  }
  return *this = newPoly;
}

static int  maxit        = 60;
static bool purification = true;

Complex Polynomial::findRoot(const Complex &start, bool verbose, const Real &rootCriterium) const {
  Polynomial F = *this;
  Polynomial f = dfdx();

  Complex x = start;
  int i;
  for(i = 0; i < maxit; i++) {
    const Complex y = F(x);
    const Real l = arg(y);
    if(l <= rootCriterium) {

      if(verbose) {
        tcout << _T("Iter(") << i << _T("): x:") << x << _T(" arg(fx):") << l << endl;
      }

      break;
    }
    const Complex dy = f(x);

    if(arg(dy) == 0) {
      throwException(_T("findRoot:dfdx=0"));
    }

    if(verbose) {
      tcout << _T("Iter(") << i << _T("): x:") << x << _T(" arg(fx):") << l << endl;
    }

    x -= y / dy;
  }

  if (i == maxit && verbose) {
    throwException(_T("findRoot:bad convergence"));
  }

  return x;
}

ComplexVector Polynomial::findRoots(bool verbose, const Real &rootCriterium) const {
  ComplexVector roots(getDegree());
  Polynomial p(*this);
  for(int k = 0, d = getDegree(); d > 0; d--) {
    Complex x = p.findRoot(Complex(randReal(-1, 1),randReal(-1, 1)), verbose, rootCriterium);
    roots[k++] = x;
    p /= x;
  }

  if(purification) {
    if(verbose) {
      tcout << _T("Purification:") << endl;
    }
    for(UINT i = 0; i < roots.getDimension(); i++) {
      roots[i] = findRoot(roots[i], verbose, rootCriterium);
    }
  }
  return roots;
}

String Polynomial::toString(StreamSize precision) const {
  if(precision < 0) {
    precision = numeric_limits<Real>::digits10;
  }
  const int g = getDegree();
  String rp = (g > 1) ? _T(")"): EMPTYSTRING;
  wstringstream result;

  result.precision(precision);
  result << spaceString(g-1,'(').cstr();

  for(int i = g; i >= 0; i--) {
    const Complex &coef = getCoef(i);
    if(i == g) {
      result << coef;
      if(g > 0) {
        result << _T("*x");
      }
    } else {
      if(i < g-1) {
        result << rp;
        result << _T("*x");
      }
      if(coef.im == 0) {
        if(coef.re > 0) {
          result << _T(" + ");
          result << coef.re;
        } else if(coef.re < 0) {
          result << _T(" - ");
          result << -coef.re;
        }
      } else if(coef != Complex::_0) {
        result << _T(" + ");
        result << coef;
      }
    }
  }
  return result.str().c_str();
}

tostream &operator<<(tostream &out, const Polynomial& poly) {
  out << poly.toString();
  return out;
}
