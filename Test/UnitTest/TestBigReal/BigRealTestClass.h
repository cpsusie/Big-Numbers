#pragma once

#include <TimeMeasure.h>
#include "FunctionTest.h"

// -----------------------------------------------------------------------------------------------------------------
// This class is a friend of BigReal, so all functions that want special access to private members of BigReal
// should be added here.
// WARNING: Should NOT be run multithreaded, as they all use DEFAULT_DIGITPOOL

class BigRealTestClass {
private:
  DigitPool      *m_digitPool;
  RandomGenerator &m_rnd;

  DigitPool *getDigitPool();
  BigReal getRandomDividend(int length);
public:
  BigRealTestClass() : m_rnd(*RandomGenerator::s_stdGenerator) {
    m_digitPool = nullptr;
  }
  ~BigRealTestClass();
  RandomGenerator &getRandomGenerator() {
    return m_rnd;
  }
  void measureProduct(bool measureSplitProd, bool measureSplitProdRealTime, bool measureShortProd, int splitLength = -1);
  void measureSplitLength();
  void measureQuot();
  void measureQuotRemainder();
  void testQuotRemainder();
  void testMultiThreadedProduct();
  void testLnEstimate();
  void testFullFormat();
  void testGetDouble();
  void testBigRealStream();
  void testCopy();
  void testCopyAllDigits();
  void testTruncRound();
  void testCopyrTrunc();

  void testFractionate();
  void testGetDecimalDigitCount64();
#if defined(HAS_LOOP_DIGITCOUNT)
  void measureGetDecimalDigitCount();
#endif // HAS_LOOP_DIGITCOUNT
};

typedef enum {
  SUM
 ,DIF
 ,PROD
 ,SHORTPROD
 ,QUOTNEWTON
 ,QUOTLINEAR32
 ,QUOTLINEAR64
 ,QUOTREMAINDER
 ,QUOTREMAINDER64
 ,OPERATOR_MOD
 ,OPERATOR_MOD64
#if defined(IS64BIT)
 ,QUOTLINEAR128
 ,QUOTREMAINDER128
 ,OPERATOR_MOD128
#endif // IS64BIT
} BinaryOperator;

class MeasureBinaryOperator : public MeasurableFunction {
private:
  DigitPool             *m_pool;
  const BinaryOperator   m_op;
  const Array<BigReal>  &m_x, &m_y;
  const BigReal          m_f;
  BigReal                m_result, m_remainder;
  BigInt                 m_intResult;
  int i,j;
public:
  void f();
  MeasureBinaryOperator(BinaryOperator op, const Array<BigReal> &x, const Array<BigReal> &y, const BigReal &f);
  ~MeasureBinaryOperator();

  const BigReal &getResult() const {
    return m_result;
  }
};

