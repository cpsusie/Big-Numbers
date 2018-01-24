#include "stdafx.h"
#include <Math/FPU.h>
#include "gamma.h"

int main(int argc, char **argv) {
/*
  for(int i = 0; i < 100; i++) {
    double x = pow(10,-i);
    printf("%le %d\n",x,findlimit(x));
  }

  return(0);
  for(;;) {
    printf("Indtast x:");
    char line[200];
    gets(line);
    double x;
    sscanf(line,"%le",&x);
    int p = findlimit(x);
    printf("findlimit(%le):%d\n",x,p);
  }

  for(;;) {
    printf("Indtast x:");
    char line[200];
    gets(line);
    double x;
    sscanf(line,"%le",&x);
    double g = fac(x);
    printf("fac(x):%20.15le\n",g);
  }

  printf("gamma(1.5):%20.15le\n",gamma(1.5));
  return 0;
*/

  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
  for(double x = 1; x <= 2.00001; x+= 0.01) {
    tcout.precision(3);
    tcout.flags(ios::fixed);
    tcout.width(5);
    tcout << x;
    tcout.flush();

    tcout.precision(17);
    tcout.flags(ios::fixed);
    tcout.width(23);
    tcout << Lgamma(x);
    tcout.flush();

    tcout.precision(19);
    tcout.flags(ios::fixed);
    tcout.width(23);
    tcout << Lgamma80(x);
    tcout << _T("\n");

    tcout.flush();
  }
  return 0;
}

