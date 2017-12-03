#include "stdafx.h"
#include <Math/MathLib.h>

double norm1(double x) {
  double sum = 0;
  double x2  = -x * x;
  double p   = x;
  double n   = 1;
  double n1  = 1;
#define _norm_factor  0.3989422804014327 /* 1/sqrt(2*PI) */
  for(;;) {
    double oldsum = sum;
    sum += p / n1;
    if(sum == oldsum) break;
    p *= x2 / n / 2.0;
    n++;
    n1 += 2;
  }
  return 0.5 + sum * _norm_factor;
}

int main(int argc, char **argv) {
  for(double x = -6; x < 6; x+= 0.01) {
    printf("%lg %lg\n",x,norm(x) - norm1(x));
  }
  return 0;
  for(;;) {
    printf("Indtast x:");
    char line[100];
    fgets(line, sizeof(line), stdin);
    double x;
    if(sscanf(line,"%le",&x) != 1) continue;
    printf("norm(%lg):%lg\n",x,norm(x));
  }
  return 0;
}
