#pragma once

#include <TimeMeasure.h>
#include "FunctionTest.h"

// -----------------------------------------------------------------------------------------------------------------
// This class is a friend of BigReal, so all functions that want special access to private members of BigReal 
// should be added here.
// WARNING: Should NOT be run multithreaded, as they all use DEFAULT_DIGITPOOL

class BigRealTestClass {
private:
  DigitPool  *m_digitPool;
  Random     &m_rnd;

  DigitPool *getDigitPool();
  BigReal getRandomDividend(int length);
public:
  BigRealTestClass() : m_rnd(_standardRandomGenerator) {
    m_digitPool = NULL;
  }
  ~BigRealTestClass();
  Random &getRandomGenerator() {
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
 ,QUOTREMAINDER1
 ,OPERATOR_MOD
 ,NEW_OPERATOR_MOD
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

