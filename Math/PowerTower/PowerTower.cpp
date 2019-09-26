#include "stdafx.h"
#include <Math/Double80.h>

using namespace std;

typedef FunctionTemplate<Double80, Double80> D80lFunction;

Double80 findZero(const Double80 &left, const Double80 &right, D80lFunction &f, const Double80 &tolerance) {
  Double80 x0 = left, y0 = f(x0);
  Double80 x2 = right, y2 = f(x2);
  for(int i = 0;; i++) {
    if(sign(y0) == sign(y2)) {
      throwException(_T("(x0,f(x0)):(%s, %s), (x2,f(x2)):(%s, %s). Same sign of y-values")
        , toString(x0).cstr(), toString(y0).cstr(), toString(x2).cstr(), toString(y2).cstr());
    }
    if (x2 == x0) {
      return x0;
    }
    Double80 x1 = (x0 + x2) / 2;
    const Double80 y1 = f(x1);
    if((fabs(y1) <= tolerance) || (i==400)) {
      return x1;
    }
    if(sign(y1) == sign(y0)) {
      x0 = x1;
      y0 = y1;
    } else {
      x2 = x1;
      y2 = y1;
    }
  }
}

class PowerTowrFunctionD80 : public D80lFunction {
private:
  UINT64 m_height;
public:
  PowerTowrFunctionD80() : m_height(0) {
  }
  Double80 operator()(const Double80 &x);
  void setHeight(UINT64 height) {
    m_height = height;
  }
};

#undef max
Double80 PowerTowrFunctionD80::operator()(const Double80 &x) {
  Double80 y = x;
  for(UINT64 i = 0; i < m_height; i++) {
    y = pow(x, y);
    if(!isfinite(y)) {
      return numeric_limits<Double80>::max();
    }
  }
  return y - 8;
}

int mainD80() {
  try {
    Double80 lower0(1.44466);
    Double80 lower(1.44466);
    Double80 upper(1.444667861021);
    Double80 tolerance(1e-10);
    double fheight = 970739;
    INT64  height = (INT64)fheight;
    PowerTowrFunctionD80 f;
    f.setHeight(height);
    cout.precision(19);
    for (;;) {
      const Double80 x = findZero(lower, upper, f, tolerance);
      cout << height << " " << x << endl;
      for (INT64 oldHeight = height;;) {
        fheight = fheight * 3 / 2;
        height = (INT64)fheight;
        if (height > oldHeight) break;
      }
      f.setHeight(height);
      upper = x;
      for(Double80 y = f(x), step = (x - lower0)*0.01, xn = x - step; xn >= lower0; xn -= step) {
        const Double80 yn = f(xn);
        if(sign(yn) != sign(y)) {
          lower = xn;
          break;
        } else {
          upper = xn;
        }
      }
    }
  } catch (Exception e) {
    tcout << e.what() << endl;
    exit(-1);
  }
  return 0;
}



size_t digits = 100;

typedef FunctionTemplate<BigReal,BigReal> BigRealFunction;

BigReal findZero(const BigReal &left, const BigReal &right, BigRealFunction &f, const BigReal &tolerance) {
  BigReal x0 = left , y0 = f(x0);
  BigReal x2 = right, y2 = f(x2);
  for(;;) {
    if(sign(y0) == sign(y2)) {
      throwException(_T("(x0,f(x0)):(%s, %s), (x2,f(x2)):(%s, %s). Same sign of y-values")
                    ,toString(x0).cstr(), toString(y0).cstr(), toString(x2).cstr(), toString(y2).cstr());
    }

//    BigReal x1 = x0 - rQuot(y0 * (x2 - x0), y2 - y0, digits);

//    if((x1 < x0) || (x1 > x2)) {
      BigReal x1 = (x0 + x2) * BIGREAL_HALF;
//    }
    const BigReal y1 = f(x1);
    if(compareAbs(y1, tolerance) <= 0) {
      return x1;
    }
    if(sign(y1) == sign(y0)) {
      x0 = x1;
      y0 = y1;
    } else {
      x2 = x1;
      y2 = y1;
    }
  }
}

class PowerTowrFunction : public BigRealFunction {
private:
  UINT64 m_height;
public:
  PowerTowrFunction() : m_height(0) {
  }
  BigReal operator()(const BigReal &x);
  void setHeight(UINT64 height) {
    m_height = height;
  }
};

const BigReal _8(8);

BigReal PowerTowrFunction::operator()(const BigReal &x) {
  BigReal y = x;
  try {
    for(UINT64 i = 0; i < m_height; i++) {
      y = rPow(x, y, 2 * digits);
    }
    return rDif(y,_8, digits);
  } catch (...) {
    return e(BIGREAL_1, 100);
  }
}

int main(int argc, char **argv) {
  mainD80();

  try {
    BigReal lower0(1);
    BigReal lower(1);
    BigReal upper(3);
    BigReal tolerance(e(BIGREAL_1, -2 * (intptr_t)digits / 3));
    double fheight = 1;
    INT64  height  = (INT64)fheight;
    PowerTowrFunction f;
    f.setHeight(height);
    cout.precision(40);
    for(;;) {
      const BigReal x = findZero(lower, upper, f, tolerance);
      cout << height << " " << x << endl;
      for(INT64 oldHeight = height;;) {
        fheight = fheight * 3 / 2;
        height = (INT64)fheight;
        if(height > oldHeight) break;
      }
      f.setHeight(height);
      upper = x;
      for(BigReal y = f(x), step = (x-lower0)*e(BIGREAL_1,-2), xn = x - step; xn >= lower0; xn -= step) {
        const BigReal yn = f(xn);
        if(sign(yn) != sign(y)) {
          lower = xn;
          break;
        } else {
          upper = xn;
        }
      }
    }
  } catch (Exception e) {
    tcout << e.what() << endl;
    return -1;
  }
  return 0;
}

