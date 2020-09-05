#include <iostream>
#include <limits>
#include <math.h>
#include <fenv.h>

using namespace std;

bool isUint64( double v) {
  return isfinite(v) && (v == floor(v)) && (v>=0) && (v<0x1.0p+64);
}

int main(int argc, char **argv) {
  double x = 0, y = 12;
  double z = fmod(x, y);

  cout << "x=" << x << ", y=" << y << ", z=" << z << endl;

  unsigned __int64 ul64_0 = ULLONG_MAX;
  double           d64    = (double)ul64_0;
  unsigned __int64 ul64_1 = (unsigned __int64)d64;

  cout << "ul64_0=" << hex      << ul64_0 << endl;
  cout << "d64   =";
  cout.precision(14);
  cout << hexfloat << d64 << endl;
  cout << "ul64_1=" << hex      << ul64_1 << endl;

  bool b = isUint64(d64);
  cout << boolalpha << "isUint return " << b << endl;

  return 0;
}
