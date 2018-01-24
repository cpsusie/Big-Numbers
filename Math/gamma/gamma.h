#pragma once

#include <Math/Double80.h>

double Lgamma(double x);
double Lfac(  double x);

Double80 Lgamma80(const Double80 &x);
Double80 Lfac80(  const Double80 &x);

int findlimit(double k);
