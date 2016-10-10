#include "pch.h"

#define MINMAX1(x,MIN,MAX) (((x) < (MIN)) ? (MIN) : ((x) > (MAX)) ? (MAX) : (x))
#define MINMAX(x,x1,x2) (((x1) <= (x2)) ? MINMAX1(x,x1,x2) : MINMAX1(x,x2,x1))


short minMax(short x, short x1, short x2) {
  return MINMAX(x, x1, x2);
}

unsigned short minMax(unsigned short x, unsigned short x1, unsigned short x2) {
  return MINMAX(x, x1, x2);
}

int minMax(int x, int x1, int x2) {
  return MINMAX(x, x1, x2);
}

UINT minMax(UINT x, UINT x1, UINT x2) {
  return MINMAX(x, x1, x2);
}

__int64 minMax(__int64 x, __int64 x1, __int64 x2) {
  return MINMAX(x, x1, x2);
}

unsigned __int64 minMax(unsigned __int64 x, unsigned __int64 x1, unsigned __int64 x2) {
  return MINMAX(x, x1, x2);
}

float minMax(float x, float x1, float x2) {
  return MINMAX(x, x1, x2);
}

double minMax(double x, double x1, double x2) {
  return MINMAX(x, x1, x2);
}

