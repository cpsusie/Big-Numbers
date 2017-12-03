#include "stdafx.h"
#include <Math/Rational.h>
#include <Math/BigReal.h>
#include <Math/Double80.h>
#include <Math/Int128.h>

void doInt128Math() {
  _int128 x, y;

  x = 12345678901234567;
  y = 1243322323;

  _int128 z = x * y;

  char s[200];
  _i128toa(z, s, 10);

  printf("z:%s\n", s);
}

void doD80Math() {
  Double80 x = 1;
  for(; x < 10; x += 0.1234) {
    TCHAR s[100];
    d80tot(s,x);
  }
}

int main(int argc, TCHAR **argv) {


  Rational r(35798345634222, 235489763444);

  doD80Math();
  int brSize = sizeof(BigReal);
  doInt128Math();
  int sz = sizeof(BigReal);

  BigReal xb, yb;
  yb = BigReal("65742343535235935e3444");
  for (;;) {
    yb = rProd(yb, yb, 10000);
  }
  int fisk = 1;

  return 0;
}

