#pragma once

#include <Math.h>
#include <Float.h>
#include <Math/Double64.h>
#include <Math/Double80.h>
#include "TestInterval.h"

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

BigReal getRandom(int length, int exponent, RandomGenerator &rnd, DigitPool *pool, bool allowZero = true);
Array<BigReal> generateTestData(const TestInterval &interval, DigitPool *pool, int count = 23);

float    getRelativeError32(float           x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError64(double          x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError80(const Double80 &x, DigitPool *pool , size_t *length = NULL);
double   getRelativeError(  const double   &x, const BigReal &x0);
Double80 getRelativeError(  const Double80 &x, const BigReal &x0);
BigReal  getRelativeError(  const BigReal  &x, const BigReal &x0);

void log(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
void clearLine();

const String &getSourceDir();
const String &getArchitecture();
const String &getCompileMode();
const String getCompileArchitectureSignatureString();
const String getSignatureSubDir();
