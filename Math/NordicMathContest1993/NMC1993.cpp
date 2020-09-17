#include "targetver.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// See https://www.youtube.com/watch?v=9lgsFhHVegc

constexpr double _1over3 = 1.0 / 3;
constexpr double _2over3 = 2.0 / 3;

UINT maxLevel = 0;
double f(double x,UINT level) {
  if(level > maxLevel) maxLevel = level;
  if(x < _1over3) {
    return (x <= 0) ? 0 : 0.5 * f(3.0 * x,level+1);
  } else if(x > _2over3) {
    return (x >= 1) ? 1 : 1.0 - f(1.0 - x,level+1);
  } else {
    return 0.5;
  }
}

double f(double x) {
  maxLevel = 0;
  return f(x, 0);
}

int main(int argc, char **argv) {
  constexpr int n = 100000;
  constexpr double left = 0;
  constexpr double right = 1;
  constexpr double step = (right - left) / (n - 1);
  double x = left;
  for(int i = 0; i++ < n; x += step) {
    if(i == n) x = right;
    printf("%le %le %5u\n", x, f(x), maxLevel);
  }
  return 0;
}
