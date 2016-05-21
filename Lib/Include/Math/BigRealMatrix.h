#pragma once

#include <MatrixTemplate.h>
#include <VectorTemplate.h>
#include "BigReal.h"

class BigRealVector : public VectorTemplate<BigReal> {
private:
  unsigned int m_digits;
  void checkPrecision(unsigned int digits);
protected:
  void init(unsigned int dim, bool initialize, int digits);

public:
  explicit BigRealVector(unsigned int dim = 1, unsigned int digits = 16);
  BigRealVector(const BigRealVector &v);
  BigRealVector(const VectorTemplate<BigReal> &v, unsigned int digits);
  unsigned int setPrecision(unsigned int digits);
  unsigned int getPrecision() const {
    return m_digits;
  }
  BigRealVector &operator*=(const BigReal &d);
  BigRealVector &operator/=(const BigReal &d);
  BigRealVector &operator+=(const BigRealVector &rhs);
  BigRealVector &operator-=(const BigRealVector &rhs);
  BigReal length() const;
  friend class BigRealMatrix;
};

BigRealVector  operator*(const BigReal &d, const BigRealVector &rhs );
BigRealVector  operator*(const BigRealVector &lts, const BigReal &d );
BigRealVector  operator/(const BigRealVector &lts, const BigReal &d );
BigRealVector  operator+(const BigRealVector &lts, const BigRealVector& rhs);
BigRealVector  operator-(const BigRealVector &lts, const BigRealVector& rhs);
BigReal        operator*(const BigRealVector &lts, const BigRealVector& rhs);

class BigRealMatrix : public MatrixTemplate<BigReal> {
protected:
  unsigned int m_digits;
  void checkPrecision(unsigned int digits);
  void init(unsigned int rows, unsigned int cols, bool initialize, unsigned int digits);
public:
  explicit BigRealMatrix(unsigned int rows = 1, unsigned int cols = 1, unsigned int digits = 16);
  BigRealMatrix(const BigRealMatrix &a);
  unsigned int setPrecision(unsigned int digits);
  unsigned int getPrecision() const {
    return m_digits;
  }
  static BigRealMatrix one( unsigned int dim , unsigned int digits = 16);
  static BigRealMatrix zero(unsigned int rows, unsigned int columns, unsigned int digits = 16);
  BigRealVector getRow(   unsigned int row) const;
  BigRealVector getColumn(unsigned int column) const;
};

BigRealVector operator*(const BigRealVector &lts, const BigRealMatrix &rhs);
BigRealVector operator*(const BigRealMatrix &lts, const BigRealVector &rhs);
BigRealMatrix operator+(const BigRealMatrix &lts, const BigRealMatrix &rhs);
BigRealMatrix operator-(const BigRealMatrix &lts, const BigRealMatrix &rhs);
BigRealMatrix operator*(const BigRealMatrix &lts, const BigRealMatrix &rhs);
BigRealMatrix operator*(const BigReal       &d  , const BigRealMatrix &rhs);
BigRealMatrix operator*(const BigRealMatrix &lts, const BigReal       &d  );
BigRealMatrix operator/(const BigRealMatrix &lts, const BigReal       &d  );
BigReal       det(      const BigRealMatrix &a);
BigRealMatrix inverse(  const BigRealMatrix &a);
BigRealMatrix transpose(const BigRealMatrix &a);
BigReal       normf(    const BigRealMatrix &a); // Frobenius norm

class BigRealLUMatrix : private BigRealMatrix {
private:
  unsigned int *m_permut;
  int           m_detsign;

  void allocPermut();
  void initPermut();
  BigRealVector evald() const;
  void pivot(const BigRealVector &d, unsigned int k);
  void lowerUpper();
  BigRealLUMatrix(const BigRealLUMatrix &a);                              /* not defined */
  BigRealLUMatrix &operator=(const BigRealLUMatrix &a);                   /* not defined */
  BigRealLUMatrix &setDimension(unsigned int rows, unsigned int cols);   /* not defined */

public:
  BigRealLUMatrix();
  BigRealLUMatrix(const BigRealMatrix &a);
  ~BigRealLUMatrix();
  BigRealLUMatrix &operator=(const BigRealMatrix &a);
  BigRealMatrix getInverse() const;
  BigRealVector solve(const BigRealVector &y) const;
  BigReal getDeterminant() const;


  friend tostream &operator<<(tostream &out, const BigRealLUMatrix &a);
};
