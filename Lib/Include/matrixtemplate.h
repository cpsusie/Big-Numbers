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

  void checkIndex(size_t r, size_t c) const {
    if(!m_dim.isLegalIndex(r,c)) {
      throwIndexException(_T("Index (%s, %s) out of range")
                         ,format1000(r).cstr()
                         ,format1000(c).cstr());
    }
  }

  static T **allocate(size_t rows, size_t columns, bool initialize) {
    if(rows == 0) {
      throwMatrixException(_T("allocate:Number of rows=0"));
    }
    if(columns == 0) {
      throwMatrixException(_T("allocate:Number of columns=0"));
    }
    T **a = new T*[rows];
    for(size_t r = 0; r < rows; r++) {
      a[r] = new T[columns];
      T *v = a[r];
      if(initialize) {
        for(size_t c = 0; c < columns; c++) {
          v[c] = T(0);
        }
      }
    }
    return a;
  }

  void cleanup() {
    for(size_t r = 0; r < getRowCount(); r++) {
      delete[] m_a[r];
    }
    delete[] m_a;
  }

protected:
  void init(size_t rows, size_t columns, bool initialize) {
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

  MatrixTemplate(size_t rows, size_t columns) {
    init(rows, columns, true);
  }

  explicit MatrixTemplate(const MatrixDimension &dim) {
    init(dim.rowCount, dim.columnCount, true);
  }

  MatrixTemplate(const MatrixTemplate<T> &src) {
    init(src.getRowCount(), src.getColumnCount(), false);
    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = src.m_a[r][c];
      }
    }
  }

  explicit MatrixTemplate(const VectorTemplate<T> &diagonal) {
    const size_t d = diagonal.getDimension();
    init(d, d, true);
    for(size_t i = 0; i < d; i++) {
      m_a[i][i] = diagonal[i];
    }
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

  MatrixTemplate<T> &operator=(const MatrixTemplate<T> &src) {
    if(this == &src) {
      return *this;
    }
    if(!hasSameDimension(src)) {
      cleanup();
      init(src.getRowCount(), src.getColumnCount(), false);
    }

    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = src.m_a[r][c];
      }
    }
    return *this;
  }

  virtual ~MatrixTemplate() {
    cleanup();
  }

  void clear() {
    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] = T(0);
      }
    }
  }

  MatrixTemplate<T> &setDimension(size_t rows, size_t columns) {
    if(rows != getRowCount() || columns != getColumnCount()) {
      T **newa = allocate(rows, columns, true);
      const size_t copyr = __min(rows, getRowCount());
      const size_t copyc = __min(columns, getColumnCount());
      for(size_t r = 0; r < copyr; r++) {
        for(size_t c = 0; c < copyc; c++) {
          newa[r][c] = m_a[r][c];
        }
      }
      cleanup();
      m_a   = newa;
      m_dim = MatrixDimension(rows, columns);
    }
    return *this;
  }

  MatrixTemplate<T> &setDimension(size_t dim) {  // make it square
    return setDimension(dim, dim);
  }

  inline size_t getRowCount() const {
    return m_dim.rowCount;
  }

  inline size_t getColumnCount() const {
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
    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = r+1; c < getColumnCount(); c++) {
        if(m_a[r][c] != m_a[c][r]) {
          return false;
        }
      }
    }
    return true;
  }

  T &operator()(size_t r, size_t c) {
    checkIndex(r, c);
    return m_a[r][c];
  }

  const T &operator()(size_t r, size_t c) const {
    checkIndex(r, c);
    return m_a[r][c];
  }

  T &subDiagonal(size_t row) { // row must be [1..getRowCount()-1]
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[row][row-1];
  }

  const T &subDiagonal(size_t row) const { // row must be [1..getRowCount()-1]
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[row][row-1];
  }

  VectorTemplate<T> getRow(size_t row) const {
    if(row >= getRowCount()) {
      throwIndexException(_T("getRow:Row %s out of range"), format1000(row).cstr());
    }
    VectorTemplate<T> result(getColumnCount());
    for(size_t c = 0; c < getColumnCount(); c++) {
      result[c] = m_a[row][c];
    }
    return result;
  }

  VectorTemplate<T> getColumn(size_t column) const {
    if(column >= getColumnCount()) {
      throwIndexException(_T("getColumn:Column %s out of range"), format1000(column).cstr());
    }
    VectorTemplate<T> result(getRowCount());
    for(size_t r = 0; r < getRowCount(); r++) {
      result[r] = m_a[r][column];
    }
    return result;
  }

  MatrixTemplate<T> &setRow(size_t row, const VectorTemplate<T> &v) {
    if(row >= getRowCount()) {
      throwIndexException(_T("setRow:Row %s out of range"), format1000(row).cstr());
    }
    if(v.getDimension() != getColumnCount()) {
      throwMatrixException(_T("setRow:Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr(), v.getDimensionString().cstr());
    }
    for(size_t c = 0; c < getColumnCount(); c++) {
      m_a[row][c] = v(c);
    }
    return *this;
  }

  MatrixTemplate<T> &setColumn(size_t column, const VectorTemplate<T> &v) {
    if(column >= getColumnCount()) {
      throwIndexException(_T("setColumn:Column %s out of range"), format1000(column).cstr());
    }
    if(v.getDimension() != getRowCount()) {
      throwMatrixException(_T("setColumn:Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr(), v.getDimensionString().cstr());
    }
    for(size_t r = 0; r < getRowCount(); r++) {
      m_a[r][column] = v(r);
    }
    return *this;
  }

  MatrixTemplate<T> &swapRows(size_t r1, size_t r2) {
    if(r1 >= getRowCount()) {
      throwIndexException(_T("swapRows:r1=%s out of range"), format1000(r1).cstr());
    }
    if(r2 >= getRowCount()) {
      throwIndexException(_T("swapRows:r2=%s out of range"), format1000(r2).cstr());
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
    for(size_t r = 0; r < getColumnCount(); r++) {
      result(r) = m_a[r][r];
    }
    return result;
  }

  MatrixTemplate<T> getSubMatrix(size_t row, size_t column, size_t rowCount, size_t columnCount) const {
    if(row + rowCount > getRowCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                          , format1000(row).cstr()
                          , format1000(column).cstr()
                          , format1000(rowCount).cstr()
                          , format1000(columnCount).cstr()
                          , format1000(getRowCount()).cstr());
    }
    if(column + columnCount > getColumnCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          , format1000(row).cstr()
                          , format1000(column).cstr()
                          , format1000(rowCount).cstr()
                          , format1000(columnCount).cstr()
                          , format1000(getColumnCount()).cstr());
    }

    MatrixTemplate<T> result(rowCount, columnCount);
    for(size_t r = row, rr = 0; rr < rowCount; r++, rr++) {
      for(size_t  c = column, rc = 0; rc < columnCount; c++, rc++) {
        result.m_a[rr][rc] = m_a[r][c];
      }
    }
    return result;
  }

  MatrixTemplate<T> getSubMatrix(size_t  row, size_t  column) const {
    return getSubMatrix(row, column, getRowCount()-row, getColumnCount()-column);
  }

  MatrixTemplate<T> &setSubMatrix(size_t  row, size_t  column, const MatrixTemplate<T> &src) {
    if(row + src.getRowCount() > getRowCount()) {
      throwMatrixException(_T("setSubMatrix:Cannot set subMatrix at offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                           , format1000(row).cstr()
                           , format1000(column).cstr()
                           , format1000(src.getRowCount()).cstr()
                           , format1000(src.getColumnCount()).cstr()
                           , format1000(getRowCount()).cstr());
    }
    if(column + src.getColumnCount() > getColumnCount()) {
      throwMatrixException(_T("setSubMatrix:Cannot set subMatrix at offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          , format1000(row).cstr()
                          , format1000(column).cstr()
                          , format1000(src.getRowCount()).cstr()
                          , format1000(src.getColumnCount()).cstr()
                          , format1000(getColumnCount()).cstr());
    }

    const size_t rowCount    = src.getRowCount();
    const size_t columnCount = src.getColumnCount();
    for(size_t r = row, sr = 0; sr < rowCount; r++, sr++) {
      for(size_t c = column, sc = 0; sc < columnCount; c++, sc++) {
        m_a[r][c] = src.m_a[sr][sc];
      }
    }
    return *this;
  }

  static MatrixTemplate<T> one(size_t dim) {
    MatrixTemplate<T> result(dim, dim);
    for(size_t  i = 0; i < dim; i++) {
      result.m_a[i][i] = 1;
    }
    return result;
  }

  static MatrixTemplate<T> zero(size_t rows, size_t columns) {
    return MatrixTemplate<T>(rows, columns);
  }

  friend MatrixTemplate<T> operator+(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    lts.checkSameDimension(_T("operator+"), rhs);

    MatrixTemplate<T> result(rows, columns);
    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] + rhs.m_a[r][c];
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    lts.checkSameDimension(_T("operator-"), rhs);

    MatrixTemplate<T> result(rows, columns);
    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] - rhs.m_a[r][c];
      }
    }
    return result;   
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &m) {
    const size_t rows    = m.getRowCount();
    const size_t columns = m.getColumnCount();

    MatrixTemplate<T> result(rows, columns);
    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = -m.m_a[r][c];
      }
    }
    return result;   
  }

  MatrixTemplate<T> &operator+=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator+="), rhs);

    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] += rhs.m_a[r][c];
      }
    }
    return *this;
  }

  MatrixTemplate<T> &operator-=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator-="), rhs);

    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] -= rhs.m_a[r][c];
      }
    }
    return *this;
  }

  friend VectorTemplate<T> operator*(const MatrixTemplate<T> &lts, const VectorTemplate<T> &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    if(columns != rhs.getDimension()) {
      throwMatrixException(_T("operator*(Matrix,Vector):Invalid dimension. Matrix.%s, Vector.%s")
                          ,lts.getDimensionString().cstr()
                          ,rhs.getDimensionString().cstr());
    }

    VectorTemplate<T> result(rows);
    for(size_t r = 0; r < rows; r++) {
	    T sum = 0;
      for(size_t c = 0; c < columns; c++) {
        sum += lts.m_a[r][c] * rhs[c];
      }
	    result[r] = sum;
    }
    return result;
  }

  friend VectorTemplate<T> operator*(const VectorTemplate<T> &lts, const MatrixTemplate &rhs) {
    const size_t rows    = rhs.getRowCount();
    const size_t columns = rhs.getColumnCount();

    if(lts.getDimension() != rows) {
      throwMatrixException(_T("operator*(Vector,Matrix):Invalid dimension. Vector.%s. Matrix.%s")
                          ,lts.getDimensionString().cstr()
                          ,rhs.getDimensionString().cstr());
    }

    VectorTemplate<T> result(columns);
    for(size_t  c = 0; c < columns; c++) {
	    T sum = 0;
      for(size_t r = 0; r < rows; r++) {
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
   
    const size_t rRows    = lts.getRowCount();
    const size_t rColumns = rhs.getColumnCount();

    MatrixTemplate<T> result(rRows, rColumns);

    const size_t maxK = lts.getColumnCount(); // == rhs.getRowCount()

    for(size_t r = 0; r < rRows; r++) {
      for(size_t c = 0; c < rColumns; c++) {
        T sum = 0;
        for(size_t k = 0; k < maxK; k++) {
          sum += lts.m_a[r][k] * rhs.m_a[k][c];
        }
        result.m_a[r][c] = sum;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const T &d, const MatrixTemplate<T> &rhs) {
    const size_t rows    = rhs.getRowCount();
    const size_t columns = rhs.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = rhs.m_a[r][c] * d;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const MatrixTemplate<T> &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] * d;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator/(const MatrixTemplate<T> &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    for(size_t r = 0; r < rows; r++) {
      for(size_t c = 0; c < columns; c++) {
        result.m_a[r][c] = lts.m_a[r][c] / d;
      }
    }
    return result;
  }

  MatrixTemplate<T> &operator*=(const T &d) {
    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] *= d;
      }
    }
    return *this;
  }

  MatrixTemplate<T> &operator/=(const T &d) {
    for(size_t r = 0; r < getRowCount(); r++) {
      for(size_t c = 0; c < getColumnCount(); c++) {
        m_a[r][c] /= d;
      }
    }
    return *this;
  }

  friend MatrixTemplate<T> transpose(const MatrixTemplate<T> &a) {
    MatrixTemplate<T> result(a.getColumnCount(), a.getRowCount());
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
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

    const size_t a = A.getRowCount();
    const size_t b = B.getRowCount();
    return kroneckerProduct(A, one(b)) + kroneckerProduct(one(a), B);
  }

  friend MatrixTemplate<T> kroneckerProduct(const MatrixTemplate<T> &A, const MatrixTemplate<T> &B) {
    const size_t m = A.getRowCount();
    const size_t n = A.getColumnCount();
    const size_t p = B.getRowCount();
    const size_t q = B.getColumnCount();
    MatrixTemplate<T> result(m*p, n*q);

    for(size_t i = 0; i < m; i++) {
      for(size_t j = 0; j < n; j++) {
        const T &aij = A.m_a[i][j];
        for(size_t k = 0, alfa = p*i; k < p; k++, alfa++) {
          for(size_t l = 0, beta = q*j; l < q; l++, beta++) {
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
    for(size_t r = 0; r < m1.getRowCount(); r++) {
      for(size_t c = 0; c < m1.getColumnCount(); c++) {
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
    for(size_t r = 0; r < getRowCount(); r++) {
      result += getRow(r).toString() + _T("\n");
    }
    return result;
  }

  friend tostream &operator<<(tostream &out, const MatrixTemplate<T> &a) {
    StreamParameters p(out);
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        out << p << a.m_a[r][c] << _T(" ");
      }
      out << _T("\n");
    }
    return out;
  }

  friend tistream &operator>>(tistream &in, MatrixTemplate<T> &a) {
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        in >> a.m_a[r][c];
      }
    }
    return in;
  }

  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }
};
