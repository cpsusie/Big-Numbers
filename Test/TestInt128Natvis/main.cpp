#include "stdafx.h"
#include <Math/Int128.h>
#include <Math/Rational.h>

int main(int argc, TCHAR **argv) {

  _int128 x, y;

  Rational r(35798345634222, 235489763444);

  x = 12345678901234567;
  y = 1243322323;

  _int128 z = x * y;

  char s[200];
  _i128toa(z, s, 10);

  int fisk = 1;

  return 0;
}

