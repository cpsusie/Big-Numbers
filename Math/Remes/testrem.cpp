#include <stdio.h>
#include <math.h>

double approximation(double x);

main() {
  double x;
  for(x = -0.5; x <= 0.5; x += 1.0/200)
    _tprintf(_T("%le %le\n"),x, approximation(x) - log(x+1));
}
