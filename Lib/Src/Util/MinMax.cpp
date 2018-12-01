#include "pch.h"

#define MINMAX1(x,MIN,MAX) (((x) < (MIN)) ? (MIN) : ((x) > (MAX)) ? (MAX) : (x))
#define MINMAX(x,x1,x2) (((x1) <= (x2)) ? MINMAX1(x,x1,x2) : MINMAX1(x,x2,x1))

short   minMax(short  x, short  x1, short  x2) { return MINMAX(x, x1, x2); }
USHORT  minMax(USHORT x, USHORT x1, USHORT x2) { return MINMAX(x, x1, x2); }
int     minMax(int    x, int    x1, int    x2) { return MINMAX(x, x1, x2); }
UINT    minMax(UINT   x, UINT   x1, UINT   x2) { return MINMAX(x, x1, x2); }
INT64   minMax(INT64  x, INT64  x1, INT64  x2) { return MINMAX(x, x1, x2); }
UINT64  minMax(UINT64 x, UINT64 x1, UINT64 x2) { return MINMAX(x, x1, x2); }
float   minMax(float  x, float  x1, float  x2) { return MINMAX(x, x1, x2); }
double  minMax(double x, double x1, double x2) { return MINMAX(x, x1, x2); }

