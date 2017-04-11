#include "stdafx.h"
#include <Math.h>
#include <Date.h>

#define PI 3.14159265359

double f(double x) {
  return sin((((x+10) / 365.25) - 0.25) * 2 * PI) * 5.3725 + 12.2165;
}

int _tmain(int argc, TCHAR **argv) {
  for(Date d(1,1,2007); d.getYear() == 2007; d+=1) {
    double x = d.getDayOfYear();
    int DATO = 10000 * d.getYear() + 100 * d.getMonth() + d.getDayOfMonth();
    _tprintf(_T("%d %lf\n"),DATO,f(x));
  }
	return 0;
}
