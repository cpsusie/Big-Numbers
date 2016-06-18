#pragma once

#include <Math.h>
#include <Float.h>
#include <Math/MathLib.h>
#include <Math/BigReal.h>

typedef double    (* DoubleFunction1      )(double x);
typedef double    (* DoubleFunction2      )(double x, double y);
typedef Double80  (* Double80Function1    )(const Double80 &x);
typedef Double80  (* Double80Function2    )(const Double80 &x, const Double80 &y);
typedef BigReal   (* BigRealFunction1     )(const BigReal  &x, const BigReal &f);
typedef BigReal   (*rBigRealFunction1     )(const BigReal  &x, size_t digits);
typedef BigReal   (* BigRealBinaryOperator)(const BigReal  &x, const BigReal &y);
typedef BigReal   (* BigRealFunction2     )(const BigReal  &x, const BigReal &y, const BigReal &f);
typedef BigReal   (* BigRealFunction2Pool )(const BigReal  &x, const BigReal &y, const BigReal &f, DigitPool *pool);
typedef BigReal   (*rBigRealFunction2     )(const BigReal  &x, const BigReal &y, size_t digits);
typedef BigReal   (*rBigRealFunction2Pool )(const BigReal  &x, const BigReal &y, size_t digits, DigitPool *pool);

BigReal getRandom(int length, int exponent, Random &rnd, DigitPool *pool, bool allowZero = true);
Array<BigReal> generateTestData(const BigReal &from, const BigReal &to, bool exponentialStep, int count = 23);

float    getRelativeError32(float           x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError64(double          x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError80(const Double80 &x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError(  const double   &x, const BigReal &x0);
Double80 getRelativeError(  const Double80 &x, const BigReal &x0);
BigReal  getRelativeError(  const BigReal  &x, const BigReal &x0);

#define NEWLINE _T("\n")

void log(const TCHAR *form,...);
void clearLine();

