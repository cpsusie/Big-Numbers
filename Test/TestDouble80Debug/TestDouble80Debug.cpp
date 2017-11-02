#include "stdafx.h"
#include <Math/Double80.h>

int main() {
  for (double x = 1e-10; x < 1e100; x *= 2.3) {
    Double80 d80 = x;
    printf("%lf\n", x);
  }
  return 0;
}

