#include "pch.h"
#include <Math/Polynomial.h>

static int dataPointCompare(const DataPoint &p1, const DataPoint &p2) {
  return sign(p1.x - p2.x);
}

int LssdPolynomial::countDistinct(const CompactArray<DataPoint> &data) {
  CompactArray<DataPoint> tmp = data;
  tmp.sort(dataPointCompare);
  int count = 1;
  for(size_t i = 1; i < tmp.size(); i++) {
    if(tmp[i].x != tmp[i-1].x) {
      count++;
    }
  }
  return count;
}

LssdPolynomial::LssdPolynomial(const CompactArray<DataPoint> &data) {
  m_degree = 0;
  m        = 0;
  m_ok     = false;

  memset(alfa,0,sizeof(alfa));
  memset(beta,0,sizeof(beta));
  memset(b,0,sizeof(b));

  int n = (int)data.size();
  if(n == 0) {
    return;
  }

  int distinctXValues = countDistinct(data);

  Real sum,sigma[MAXM],omega[MAXM],gamma[MAXM];
  bool exitflag = false;
  Real *p[3];

  for(int i = 0; i < 3; i++) {
    p[i] = new Real[n];
  }

  int j1 = 0, j2 = 1, j3 = 2;
  for(int i = 0; i < n; i++) {
    p[j1][i] = 1;
    p[j3][i] = 0;
  }
  for(int j = 0; j < MAXM; j++) {
    alfa[j] = 0;
    beta[j] = 0;
    b[j]    = 0;
  }

  sum = 0;
  for(int i = 0; i < n; i++) {
    sum += data[i].w;
  }
  gamma[0] = sum;

  sum = 0;
  for(int i = 0; i < n; i++) {
    sum += sqr(data[i].y);
  }
  sigma[0] = sum;

  beta[0] = 0;
  int j = 0;
  int js = 1;
  for(;;) {
    if(js >= MAXM) {
      break;
    }
    sum = 0;
    for(int i = 0; i < n; i++) {
      sum += data[i].w * data[i].y * p[j1][i];
    }
    omega[j] = sum;
    b[j] = omega[j] / gamma[j];
    sigma[js] = sigma[js-1] - omega[j] * omega[j] / gamma[j];
    if(exitflag) {
      break;
    }

    sum = 0;
    for(int i = 0; i < n; i++) {
      sum += data[i].w * data[i].x * sqr(p[j1][i]);
    }
    alfa[j+1] = sum/gamma[j];
    for(int i = 0; i < n; i++) {
      p[j2][i] = (data[i].x-alfa[j+1]) * p[j1][i]-beta[j] * p[j3][i];
    }

    sum = 0;
    for(int i = 0; i < n; i++) {
      sum += data[i].w * sqr(p[j2][i]);
    }
    gamma[j+1] = sum;
    if(gamma[j+1] == 0) {
      break;
    }

    beta[j+1] = gamma[j+1] / gamma[j];

    exitflag = ((js>=3) &&
               (sigma[js]<=0) ||
               ((sigma[js  ]/sigma[js-1]>=0.99) &&
                (sigma[js-1]/sigma[js-2]>=0.99) &&
                (sigma[js-2]/sigma[js-3]>=0.99))
               );
    if(m == distinctXValues-1) {
      break;
    }
    j++; js++; j1 = (j1+1)%3; j2 = (j2+1)%3; j3 = (j3+1)%3;
    m++;
  }

  for(int i = 0; i < 3; i++) {
	  delete[] p[i];
  }
  m_ok = true;
}

AbstractDataFit *LssdPolynomial::clone() const {
  return new LssdPolynomial(*this);
}

Real LssdPolynomial::operator()(const Real &x) {
  Real q[MAXM+2];
  const int degree = getActualDegree();

  q[degree+1] = 0; q[degree+2] = 0;
  for(int k = degree; k >= 0; k--) {
    q[k] = b[k] + (x - alfa[k+1]) * q[k+1] - beta[k+1]*q[k+2];
  }
  return q[0];
}

Real LssdPolynomial::operator()(int d, const Real &x) {
  Real q[MAXM+2][MAXM+2]; //(q[i][j] = i'te derivate of q[j]
  int degree = getActualDegree();
  switch(d) {
  case 0:
    return (*this)(x);
  default:
    { for(int i = 0; i < MAXM + 2; i++) {
        q[i][degree + 1] = q[i][degree + 2] = 0;
      }
      for(int k = degree; k >= 0; k--) {
        q[0][k] = b[k] + (x - alfa[k+1]) * q[0][k+1] - beta[k+1]*q[0][k+2];
        for(int i = 1; i <= d; i++) {
          q[i][k] = i*q[i - 1][k + 1] + (x - alfa[k + 1]) * q[i][k + 1] - beta[k + 1] * q[i][k + 2];
        }
      }
      return q[d][0];
    }
  }
  return 0;
}

LssdPolynomial::operator Polynomial() const {
  Real q[MAXM][MAXM];
  int degree = getActualDegree();

  q[0][0] = 0;
  q[1][0] = 1;
  q[2][0] = -alfa[1];
  q[2][1] = 1;
  for(int j = 1; j < degree; j++) {
    q[j+2][0] =              -alfa[j+1]*q[j+1][0]-beta[j]*q[j][0];
    for(int i = 1; i < j; i++)
      q[j+2][i] = q[j+1][i-1]-alfa[j+1]*q[j+1][i]-beta[j]*q[j][i];
    q[j+2][j]   = q[j+1][j-1]-alfa[j+1]*q[j+1][j];
    q[j+2][j+1] = q[j+1][j];
  }
  Vector coef(degree+1);
  for(int j = 0; j <= degree; j++) {
    coef[j] = 0;
  }
  for(int j = 0; j <= degree; j++) {
    for(int i = 0; i <= j; i++) {
      coef[i] += q[j + 1][i] * b[j];
    }
  }
  return Polynomial(coef);
}

static const TCHAR *ctext_code1 =
  _T("double _poly(int degree, double *p, double x) {\n"
     "  double result = p[degree--];\n"
     "  while(degree >= 0)\n"
     "    result = result * x + p[degree--];\n"
     "  return result;\n"
     "}\n\n"
     "static double _c[] = {\n");


String LssdPolynomial::generateCFunction() const {
  Polynomial ptmp(*this);

  const int degree = ptmp.getDegree();
  String result = ctext_code1;
  for(int i = 0; i <= degree; i++) {
    result += format(_T("  %.16le%s"),ptmp.getCoef(i).re,(i < degree) ? _T(",\n") : _T("\n"));
  }
  const String declareStrFormat = _T("};\n\n"
                                     "double f(double x) {\n"
                                     "  return _poly(%d,_c,x);\n"
                                     "}\n");
  result += format(declareStrFormat.cstr(), degree);
  return result;
}
