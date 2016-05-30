#pragma once

#include "Exception.h"
#include "MyString.h"
#include "StreamParameters.h"
#include "VectorTemplate.h"
#include "MatrixDimension.h"

template <class T> class MatrixTemplate {
private:
  T             **m_a;
  MatrixDimension m_dim;

  static void vthrowMatrixException(const TCHAR *format, va_list argptr) {
    throwException(_T("MatrixTemplate:%s."), vformat(format, argptr).cstr());
  }

  static void throwMatrixException(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    vthrowMatrixException(format, argptr);
    va_end(argptr);
  }

  void throwIndexException(const TCHAR *format, ...) const {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    throwMatrixException(_T("%s. %s"), msg.cstr(), getDimensionString().cstr());
  }

  void checkSameDimension(const TCHAR *function, const MatrixTemplate<T> &m) const {
    if(!hasSameDimension(m)) {
      throwMatrixException(_T("%s:Invalid dimension. Left.%s. Right.%s")
                          ,function
                          ,getDimensionString().cstr()
                          ,m.getDimensionString().cstr()
                          );
    }
  }

  void checkIndex(unsigned int r, unsigned int c) const {
    if(!m_dim.isLegalIndex(r,c)) {
      throwIndexException(_T("Index (%u, %u) out of range"), r, c);
    }
  }

  static T **allocate(unsigned int rows, unsigned int columns, bool initialize) {
    if(rows == 0) {
      throwMatrixException(_T("allocate:Number of rows=0"));
    }
    if(columns == 0) {
      throwMatrixException(_T("allocate:Number of columns=0"));
    }
    T **a = new T*[rows];
    for(unsigned int r = 0; r < rows; r++) {
      a[r] = new T[columns];
      T *v = a[r];
      if(initialize) {
        for(unsigned int c = 0; c < columns; c++) {
          v[c] = T(0);
        }
      }
    }
    return a;
  }

  void cleanup() {
    for(int r = 0; r < getRowCount(); r++) {
      delete[] m_a[r];
    }
    delete[] m_a;
  }

protected:
  void init(unsigned int rows, unsigned int columns, bool initialize) {
    m_a   = allocate(rows, columns, initialize);
    m_dim = MatrixDimension(rows, columns);
  }

public:
  MatrixTemplate() {
    init(1, 1, true);
  }

  explicit MatrixTemplate(const T &coef) {
    init(1, 1, false);
    m_a[0][0] = coef;
  }

  MatrixTemplate(unsigned int rows, unsigned int columns) {
    init(rows, columns, true);
  }

  explicit MatrixTemplate(const MatrixDimension &dim) {
    init(dim.rowCount, dim.columnCount, true);
  }

  MatrixTemplate(const MatrixTemplate<T> &src) {
    init(src.getRowCount(), src.getColumnCount(), false);
    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = src.m_a[r][c];
      }
    }
  }

  explicit MatrixTemplate(const VectorTemplate<T> &diagonal) {
    const int d = diagonal.getDimension();
    init(d, d, true);
    for(int i = 0; i < d; i++) {
      m_a[i][i] = diagonal[i];
    }
  }

  MatrixTemplate<T> &operator=(const MatrixTemplate<T> &src) {
    if(this == &src) {
      return *this;
    }
    if(!hasSameDimension(src)) {
      cleanup();
      init(src.getRowCount(), src.getColumnCount(), false);
    }

    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = src.m_a[r][c];
      }
    }
    return *this;
  }

  virtual ~MatrixTemplate() {
    cleanup();
  }

  void clear() {
    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = T(0);
      }
    }
  }

  MatrixTemplate<T> &setDimension(unsigned int rows, unsigned int columns) {
    if(rows != getRowCount() || columns != getColumnCount()) {
      T **newa = allocate(rows, columns, true);
      const unsigned int copyr = __min(rows, (unsigned int)getRowCount());
      const unsigned int copyc = __min(columns, (unsigned int)getColumnCount());
      for(unsigned int r = 0; r < copyr; r++) {
        for(unsigned int c = 0; c < copyc; c++) {
          newa[r][c] = m_a[r][c];
        }
      }
      cleanup();
      m_a   = newa;
      m_dim = MatrixDimension(rows, columns);
    }
    return *this;
  }

  MatrixTemplate<T> &setDimension(unsigned int dim) {  // make it square
    return setDimension(dim, dim);
  }

  inline int getRowCount() const {
    return m_dim.rowCount;
  }

  inline int getColumnCount() const {
    return m_dim.columnCount;
  }

  inline const MatrixDimension &getDimension() const {
    return m_dim;
  }

  inline bool hasSameDimension(const MatrixTemplate<T> &m) const {
    return m_dim == m.m_dim;
  }

  inline bool isSquare() const {
    return m_dim.rowCount == m_dim.columnCount;
  }

  bool isSymmetric() const {
    if(!isSquare()) {
      return false;
    }
    for(int r = 0; r < getRowCount(); r++) {
      for(int c = r+1; c < getColumnCount(); c++) {
        if(m_a[r][c] != m_a[c][r]) {
          return false;
        }
      }
    }
    return true;
  }

  T &operator()(unsigned int r, unsigned int c) {
    checkIndex(r, c);
    return m_a[r][c];
  }

  const T &operator()(unsigned int r, unsigned int c) const {
    checkIndex(r, c);
    return m_a[r][c];
  }

  T &subDiagonal(unsigned int row) { // row must be [1..getRowCount()-1]
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[row][row-1];
  }

  const T &subDiagonal(unsigned int row) const { // row must be [1..getRowCount()-1]
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[row][row-1];
  }

  VectorTemplate<T> getRow(unsigned int row) const {
    if(row >= (unsigned int)getRowCount()) {
      throwIndexException(_T("getRow:Row %u out of range"), row);
    }
    VectorTemplate<T> result(getColumnCount());
    for(int c = 0; c < getColumnCount(); c++) {
      result[c] = m_a[row][c];
    }
    return result;
  }

  VectorTemplate<T> getColumn(unsigned int column) const {
    if(column >= (unsigned int)getColumnCount()) {
      throwIndexException(_T("getColumn:Column %u out of range"), column);
    }
    VectorTemplate<T> result(getRowCount());
    for(int r = 0; r < getRowCount(); r++) {
      result[r] = m_a[r][column];
    }
    return result;
  }

  MatrixTemplate<T> &setRow(unsigned int row, const VectorTemplate<T> &v) {
    if(row >= (unsigned int)getRowCount()) {
      throwIndexException(_T("setRow:Row %u out of range"), row);
    }
    if(v.getDimension() != getColumnCount()) {
      throwMatrixException(_T("setRow:Invalid dimension. %s. Vector.Dimension=%u"), getDimensionString().cstr(), v.getDimension());
    }
    for(int c = 0; c < getColumnCount(); c++) {
      m_a[row][c] = v(c);
    }
    return *this;
  }

  MatrixTemplate<T> &setColumn(unsigned int column, const VectorTemplate<T> &v) {
    if(column >= (unsigned int)getColumnCount()) {
      throwIndexException(_T("setColumn:Column %u out of range"), column);
    }
    if(v.getDimension() != getRowCount()) {
      throwMatrixException(_T("setColumn:Invalid dimension. %s. Vector.Dimension=%u"), getDimensionString().cstr(), v.getDimension());
    }
    for(int r = 0; r < getRowCount(); r++) {
      m_a[r][column] = v(r);
    }
    return *this;
  }

  MatrixTemplate<T> &swapRows(unsigned int r1, unsigned int r2) {
    if(r1 >= (unsigned int)getRowCount()) {
      throwIndexException(_T("swapRows:r1=%u out of range"), r1);
    }
    if(r2 >= (unsigned int)getRowCount()) {
      throwIndexException(_T("swapRows:r2=%u out of range"), r2);
    }
    T *tmp  = m_a[r1];
    m_a[r1] = m_a[r2];
    m_a[r2] = tmp;
    return *this;
  }

  VectorTemplate<T> getDiagonal() const {
    if(!isSquare()) {
      throwIndexException(_T("getDiagonal:Matrix not square"));
    }
    VectorTemplate<T> result(getColumnCount());
    for(int r = 0; r < getColumnCount(); r++) {
      result(r) = m_a[r][r];
    }
    return result;
  }

  MatrixTemplate<T> getSubMatrix(unsigned int row, unsigned int column, unsigned int rowCount, unsigned int columnCount) const {
    if(row + rowCount > (unsigned int)getRowCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%u,%u) with dimension(%u,%u). RowCount=%u"), row, column, rowCount, columnCount, getRowCount());
    }
    if(column + columnCount > (unsigned int)getColumnCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%u,%u) with dimension(%u,%u). ColumnCount=%u"), row, column, rowCount, columnCount, getColumnCount());
    }

    MatrixTemplate<T> result(rowCount, columnCount);
    for(unsigned int r = row, rr = 0; rr < rowCount; r++, rr++) {
      for(unsigned int c = column, rc = 0; rc < columnCount; c++, rc++) {
        result.m_a[rr][rc] = m_a[r][c];
      }
    }
    return result;
  }

  MatrixTemplate<T> getSubMatrix(unsigned int row, unsigned int column) const {
    return getSubMatrix(row, column, getRowCount()-row, getColumnCount()-column);
  }

  MatrixTemplate<T> &setSubMatrix(unsigned int row, unsigned int column, const MatrixTemplate<T> &src) {
    if(row + src.getRowCount() > (unsigned int)getRowCount()) {
      throwMatrixException(_T("setSubMatrix:Cannot set subMatrix at offset (%u,%u) with dimension(%u,%u). RowCount=%u"), row, column, src.getRowCount(), src.getColumnCount(), getRowCount());
    }
    if(column + src.getColumnCount() > (unsigned int)getColumnCount()) {
      throwMatrixException(_T("setSubMatrix:Cannot set subMatrix at offset (%u,%u) with dimension(%u,%u). ColumnCount=%u"), row, column, src.getRowCount(), src.getColumnCount(), getColumnCount());
    }

    const int rowCount    = src.getRowCount();
    const int columnCount = src.getColumnCount();
    for(int r = row, sr = 0; sr < rowCount; r++, sr++) {
      for(int c = column, sc = 0; sc < columnCount; c++, sc++) {
        m_a[r][c] = src.m_a[sr][sc];
      }
    }
    return *this;
  }

  static MatrixTemplate<T> one(unsigned int dim) {
    MatrixTemplate<T> result(dim, dim);
    for(unsigned int i = 0; i < dim; i++) {
      result.m_a[i][i] = 1;
    }
    return result;
  }

  static MatrixTemplate<T> zero(unsigned int rows, unsigned int columns) {
    return MatrixTemplate<T>(rows, columns);
  }

  friend MatrixTemplate<T> operator+(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    const int rows    = lts.getRowCount();
    const int columns = lts.getColumnCount();

    lts.checkSameDimension(_T("operator+"), rhs);

    MatrixTemplate<T> result(rows, columns);
    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] + rhs.m_a[r][c];
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    const int rows    = lts.getRowCount();
    const int columns = lts.getColumnCount();

    lts.checkSameDimension(_T("operator-"), rhs);

    MatrixTemplate<T> result(rows, columns);
    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] - rhs.m_a[r][c];
      }
    }
    return result;   
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &m) {
    const int rows    = m.getRowCount();
    const int columns = m.getColumnCount();

    MatrixTemplate<T> result(rows, columns);
    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = -m.m_a[r][c];
      }
    }
    return result;   
  }

  MatrixTemplate<T> &operator+=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator+="), rhs);

    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] += rhs.m_a[r][c];
      }
    }
    return *this;
  }

  MatrixTemplate<T> &operator-=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator-="), rhs);

    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] -= rhs.m_a[r][c];
      }
    }
    return *this;
  }

  friend VectorTemplate<T> operator*(const MatrixTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    const int rows    = lts.getRowCount();
    const int columns = lts.getColumnCount();

    if(columns != (int)rhs.getDimension()) {
      throwMatrixException(_T("operator*(Matrix,Vector):Invalid dimension. Matrix.%s, Vector.Dimension=%u"), lts.getDimensionString().cstr(), rhs.getDimension());
    }

    VectorTemplate<T> result(rows);
    for(int r = 0; r < rows; r++) {
	  T sum = 0;
      for(int c = 0; c < columns; c++) {
        sum += lts.m_a[r][c] * rhs[c];
      }
	  result[r] = sum;
    }
    return result;
  }

  friend VectorTemplate<T> operator*(const VectorTemplate<T> &lts, const MatrixTemplate &rhs) {
    const int rows    = rhs.getRowCount();
    const int columns = rhs.getColumnCount();

    if(lts.getDimension() != rows) {
      throwMatrixException(_T("operator*(Vector,Matrix):Invalid dimension. Vector.Dimension=%u. Matrix.%s"), lts.getDimension(), rhs.getDimensionString().cstr());
    }

    VectorTemplate<T> result(columns);
    for(int c = 0; c < columns; c++) {
	  T sum = 0;
      for(int r = 0; r < rows; r++) {
        sum += lts[r] * rhs.m_a[r][c];
      }
	  result[c] = sum;
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    if(lts.getColumnCount() != rhs.getRowCount()) {
      throwMatrixException(_T("operator*(Matrix,Matrix):Invalid dimension. Left.%s. Right.%s"), lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
    }
   
    const int rRows    = lts.getRowCount();
    const int rColumns = rhs.getColumnCount();

    MatrixTemplate<T> result(rRows, rColumns);

    const int maxK = lts.getColumnCount(); // == rhs.getRowCount()

    for(int r = 0; r < rRows; r++) {
      for(int c = 0; c < rColumns; c++) {
        T sum = 0;
        for(int k = 0; k < maxK; k++) {
          sum += lts.m_a[r][k] * rhs.m_a[k][c];
        }
        result.m_a[r][c] = sum;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const T &d, const MatrixTemplate<T> &rhs) {
    const int rows    = rhs.getRowCount();
    const int columns = rhs.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = rhs.m_a[r][c] * d;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const MatrixTemplate<T> &lts, const T &d) {
    const int rows    = lts.getRowCount();
    const int columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] * d;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator/(const MatrixTemplate<T> &lts, const T &d) {
    const int rows    = lts.getRowCount();
    const int columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] / d;
      }
    }
    return result;
  }

  MatrixTemplate<T> &operator*=(const T &d) {
    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] *= d;
      }
    }
    return *this;
  }

  MatrixTemplate<T> &operator/=(const T &d) {
    for(int r = 0; r < getRowCount(); r++) {
      for(int c = 0; c < getColumnCount(); c++) {
        m_a[r][c] /= d;
      }
    }
    return *this;
  }

  friend MatrixTemplate<T> transpose(const MatrixTemplate<T> &a) {
    MatrixTemplate<T> result(a.getColumnCount(), a.getRowCount());
    for(int r = 0; r < a.getRowCount(); r++) {
      for(int c = 0; c < a.getColumnCount(); c++) {
        result.m_a[c][r] = a.m_a[r][c];
      }
    }
    return result;
  }

  friend MatrixTemplate<T> kroneckerSum(const MatrixTemplate<T> &A, const MatrixTemplate<T> &B) {
    if(!A.isSquare()) {
      throwMatrixException(_T("kroneckerSum:Matrix A not square. %s."), A.getDimensionString().cstr());
    }
    if(!B.isSquare()) {
      throwMatrixException(_T("kroneckerSum:Matrix B not square. %s."), B.getDimensionString().cstr());
    }

    const int a = A.getRowCount();
    const int b = B.getRowCount();
    return kroneckerProduct(A, one(b)) + kroneckerProduct(one(a), B);
  }

  friend MatrixTemplate<T> kroneckerProduct(const MatrixTemplate<T> &A, const MatrixTemplate<T> &B) {
    const int m = A.getRowCount();
    const int n = A.getColumnCount();
    const int p = B.getRowCount();
    const int q = B.getColumnCount();
    MatrixTemplate<T> result(m*p, n*q);

    for(int i = 0; i < m; i++) {
      for(int j = 0; j < n; j++) {
        const T &aij = A.m_a[i][j];
        for(int k = 0, alfa = p*i; k < p; k++, alfa++) {
          for(int l = 0, beta = q*j; l < q; l++, beta++) {
            result.m_a[alfa][beta] = aij * B.m_a[k][l];
          }
        }
      }
    }
    return result;     
  }

  friend bool operator==(const MatrixTemplate<T> &m1, const MatrixTemplate<T> &m2) {
    if(!m1.hasSameDimension(m2)) {
      return false;
    }
    for(int r = 0; r < m1.getRowCount(); r++) {
      for(int c = 0; c < m1.getColumnCount(); c++) {
        if(!(m1.m_a[r][c] == m2.m_a[r][c])) {
          return false;
        }
      }
    }
    return true;
  }

  friend bool operator!=(const MatrixTemplate<T> &m1, const MatrixTemplate<T> &m2) {
    return !(m1 == m2);
  }

  virtual String toString() const {
    String result;
    for(int r = 0; r < getRowCount(); r++) {
      result += getRow(r).toString() + _T("\n");
    }
    return result;
  }

  friend tostream &operator<<(tostream &out, const MatrixTemplate<T> &a) {
    StreamParameters p(out);
    for(int r = 0; r < a.getRowCount(); r++) {
      for(int c = 0; c < a.getColumnCount(); c++) {
        out << p << a.m_a[r][c] << _T(" ");
      }
      out << _T("\n");
    }
    return out;
  }

  friend tistream &operator>>(tistream &in, MatrixTemplate<T> &a) {
    for(int r = 0; r < a.getRowCount(); r++) {
      for(int c = 0; c < a.getColumnCount(); c++) {
        in >> a.m_a[r][c];
      }
    }
    return in;
  }

  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }
};
