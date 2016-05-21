#pragma once

#include <MatrixTemplate.h>
#include "MathException.h"

// Implementation of LUMatrix
template <class T> class LUMatrixTemplate : private MatrixTemplate<T> {
private:

  unsigned int *m_permut;
  int           m_detsign;

  void allocPermut() {
    m_permut = new unsigned int[getRowCount()];
  }

  void initPermut() {
    for(int r = 0; r < getRowCount(); r++) {
      m_permut[r] = r;
    }
    m_detsign = 1;
  }

  VectorTemplate<T> evald() const {
    const LUMatrixTemplate<T> &a = *this;
    const int                  n = getRowCount();
    VectorTemplate<T> d(n);
    for(int i = 0; i < n; i++) {
      T max = 0;
      for(int j = 0; j < n; j++) {
        if(fabs(a(i,j)) > fabs(max)) {
          max = a(i,j);
        }
      }
      d[i] = max;
    }
    return d;
  }

  void pivot(const VectorTemplate<T> &d, unsigned int k) {
    LUMatrixTemplate<T> &a = *this;
    const int            n = getRowCount();
    int current;
    T max = 0;
    for(int i = k; i < n; i++) {
      T tmp = a(m_permut[i],k)/d[m_permut[i]];
      if(fabs(tmp) > fabs(max)) {
        max = tmp;
        current = i;
      }
    }
    if(fabs(max) == 0) {
      throwMathException(_T("LUMatrixTemplate::pivot:The matrix is singular"));
    }
    if(current != (int)k) {
      int itmp          = m_permut[current];
      m_permut[current] = m_permut[k];
      m_permut[k]       = itmp;
      m_detsign         = -m_detsign; // remember how many permutations made
    }
  }

  /* ---------------------------------- crout algorithm ------------------------ */
  /*  Implements Crout LU-decomposition as described in                          */
  /*  "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 421 */
  /* --------------------------------------------------------------------------- */
  void lowerUpper() { // Makes this = L * p * U. where L is lower triangular, U is upper triangular, and p interchange rows
    initPermut();
    LUMatrixTemplate<T> &a = *this;
    const int            n = getRowCount();
    VectorTemplate<T>    d = evald();
    initPermut();

    for(int k = 0; k < n; k++) {
      for(int i = k; i < n; i++) {
        T sum = 0;
        for(int l = 0; l <= k-1; l++) {
          sum += a(m_permut[i],l) * a(m_permut[l],k);
        }
        a(m_permut[i],k) -= sum;
      }
      a.pivot(d,k);
      for(int j = k + 1; j < n; j++) {
        T sum = 0;
        for(int l = 0; l <= k - 1; l++) {
          sum += a(m_permut[k],l) * a(m_permut[l],j);
        }
        a(m_permut[k],j) = (a(m_permut[k],j)-sum)/a(m_permut[k],k);
      }
    }
  }

  LUMatrixTemplate(const LUMatrixTemplate<T> &src);               // not defined
  LUMatrixTemplate<T> &operator=(const LUMatrixTemplate<T> &src); // not defined
  void setDimension(int rows, int columns);                       // not defined
public:
  LUMatrixTemplate() : MatrixTemplate<T>(one(1)) {
    allocPermut();
    initPermut();
  };

  LUMatrixTemplate(const MatrixTemplate<T> &a) : MatrixTemplate<T>(a) {
    if(!a.isSquare()) {
      throwMathException(_T("LUMatrixTemplate::LUMatrixTemplate:Matrix not square. %s"), a.getDimensionString().cstr());
    }
    allocPermut();
    lowerUpper();
  }

  ~LUMatrixTemplate() {
    delete[] m_permut;
  }

  LUMatrixTemplate<T> &operator=(const MatrixTemplate<T> &a) {
    if(this == &a) {
      return *this;
    }
    if(!a.isSquare()) {
      throwMathException(_T("LUMatrixTemplate::operator=:Matrix not square. %s"), a.getDimensionString().cstr());
    }

    delete[] m_permut;
    MatrixTemplate<T>::operator=(a);
    allocPermut();
    lowerUpper();
    return *this;
  }

  // Computes x so that this*x=y. Assumes this has been LU decomposed with lowerupper
  VectorTemplate<T> solve(const VectorTemplate<T> &y) const { 
    const LUMatrixTemplate<T> &a = *this;
    const int                  n = getRowCount();

    if(y.getDimension() != (unsigned int)n) {
      throwMathException(_T("LUMatrixTemplate::solveLU:Invalid dimension. y.dimension=%u, LU.%s"), y.getDimension(), getDimensionString().cstr());
    }

    VectorTemplate<T> z(n);
    for(int i = 0; i < n; i++) {
      T sum = 0;
      for(int j = 0; j <= i-1; j++) {
        sum += a(m_permut[i],j) * z[j];
      }
      z[i] = (y[m_permut[i]]-sum)/a(m_permut[i],i);
    }

    VectorTemplate<T> x(n);
    for(int i = n - 1; i >= 0; i--) {
      T sum = 0;
      for(int j = i+1; j < n; j++) {
        sum += a(m_permut[i],j) * x[j];
      }
      x[i] = z[i] - sum;
    }
    return x;
  }

  MatrixTemplate<T> getInverse() const {
    const int n = getRowCount();
    MatrixTemplate<T> result(n,n);
    VectorTemplate<T> e(n);
    for(int i = 0; i < n; i++) {
      e[i] = T(1);
      result.setColumn(i,solve(e));
      e[i] = T(0);
    }
    return result;
  }

  T getDeterminant() const {
    const LUMatrixTemplate<T> &a = *this;
    const int                  n = getRowCount();
    T d = a.m_detsign;
    for(int i = 0; i < n; i++) {
      d *= a(a.m_permut[i],i);
    }
    return d;
  }

  friend tostream &operator<<(tostream &out, const LUMatrixTemplate<T> &a) {
    out << MatrixTemplate<T>(a);
    out << _T("Permut:(");
    for(unsigned int r = 0; r < a.getRowCount(); r++) {
      out << a.m_permut[r] << _T(" ");
    }
    out << _T("). determinant sign:") << a.m_detsign << endl;
    return out;
  }
};
