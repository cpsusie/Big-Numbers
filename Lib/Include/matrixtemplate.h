#pragma once

#include "Exception.h"
#include "MyString.h"
#include "StreamParameters.h"
#include "VectorTemplate.h"
#include "MatrixDimension.h"

template <class T> class MatrixTemplate {
private:
  T              *m_a;
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
                         ,formatSize(r).cstr()
                         ,formatSize(c).cstr());
    }
  }

  inline size_t index(size_t r, size_t c) const {
    return m_dim.columnCount * r + c;
  }
  inline size_t index(const MatrixIndex &i) const {
    return index(i.r, i.c);
  }

  static T *allocate(size_t rows, size_t columns, bool initialize) {
    if(rows == 0) {
      throwMatrixException(_T("allocate:Number of rows=0"));
    }
    if(columns == 0) {
      throwMatrixException(_T("allocate:Number of columns=0"));
    }
    const size_t n = rows * columns;
    T *a = new T[n]; TRACE_NEW(a);
    if(initialize) {
      T *p = a, *last = p + n;
      const T z(0);
      while(p < last) *(p++) = z;
    }
    return a;
  }

  inline void cleanup() {
    SAFEDELETEARRAY(m_a);
  }

protected:
  void init(size_t rows, size_t columns, bool initialize) {
    m_a   = allocate(rows, columns, initialize);
    m_dim = MatrixDimension(rows, columns);
  }

public:
  inline MatrixTemplate() {
    init(1, 1, true);
  }

  explicit MatrixTemplate(const T &coef) {
    init(1, 1, false);
    m_a[0] = coef;
  }

  inline MatrixTemplate(size_t rows, size_t columns) {
    init(rows, columns, true);
  }

  explicit MatrixTemplate(const MatrixDimension &dim) {
    init(dim.rowCount, dim.columnCount, true);
  }

  MatrixTemplate(const MatrixTemplate<T> &src) {
    init(src.getRowCount(), src.getColumnCount(), false);
    T       *dp = m_a;
    const T *sp = src.m_a, *last = sp + m_dim.getElementCount();
    while(sp < last) *(dp++) = *(sp++);
  }

  explicit MatrixTemplate(const VectorTemplate<T> &diagonal) {
    const size_t d = diagonal.getDimension();
    init(d, d, true);
    for(size_t i = 0; i < d; i++) {
      m_a[index(i,i)] = diagonal[i];
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

    T       *dp = m_a;
    const T *sp = src.m_a, *last = sp + m_dim.getElementCount();
    while(sp < last) *(dp++) = *(sp++);
    return *this;
  }

  virtual ~MatrixTemplate() {
    cleanup();
  }

  void clear() {
    T *p = m_a, *last = p + m_dim.getElementCount();
    const T z(0);
    while(p < last) *(p++) = z;
  }

  MatrixTemplate<T> &setDimension(size_t rows, size_t columns) {
    if(rows != getRowCount() || columns != getColumnCount()) {
      T *newa = allocate(rows, columns, true);
      const size_t copyr = __min(rows, getRowCount());
      const size_t copyc = __min(columns, getColumnCount());
      for(size_t r = 0; r < copyr; r++) {
        size_t t = r*columns;
        for(size_t c = 0; c < copyc; c++) {
          newa[t++] = m_a[index(r,c)];
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
        if(m_a[index(r,c)] != m_a[index(c,r)]) {
          return false;
        }
      }
    }
    return true;
  }

  inline T &operator()(size_t r, size_t c) {
    checkIndex(r, c);
    return m_a[index(r,c)];
  }

  inline const T &operator()(size_t r, size_t c) const {
    checkIndex(r, c);
    return m_a[index(r,c)];
  }

  inline T &select() {
    return m_a[randSizet(m_dim.getElementCount())];
  }
  inline const T &select() const {
    return m_a[randSizet(m_dim.getElementCount())];
  }
  // row must be [1..getRowCount()-1]
  T &subDiagonal(size_t row) {
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[index(row,row-1)];
  }

  // row must be [1..getRowCount()-1]
  const T &subDiagonal(size_t row) const {
    if(!isSquare()) {
      throwIndexException(_T("subDiagonal:Matrix not square"));
    }
    checkIndex(row, row-1);
    return m_a[index(row,row-1)];
  }

  VectorTemplate<T> getRow(size_t row) const {
    if(row >= getRowCount()) {
      throwIndexException(_T("getRow:Row %s out of range"), formatSize(row).cstr());
    }
    VectorTemplate<T> result(getColumnCount());
    const T *p = m_a + index(row,0);
    for(size_t c = 0; c < getColumnCount(); c++) {
      result[c] = *(p++);
    }
    return result;
  }

  VectorTemplate<T> getColumn(size_t column) const {
    if(column >= getColumnCount()) {
      throwIndexException(_T("getColumn:Column %s out of range"), formatSize(column).cstr());
    }
    VectorTemplate<T> result(getRowCount());
    const T *p = m_a + index(0, column);
    for(size_t r = 0; r < getRowCount(); r++, p += getColumnCount()) {
      result[r] = *p;
    }
    return result;
  }

  MatrixTemplate<T> &setRow(size_t row, const VectorTemplate<T> &v) {
    if(row >= getRowCount()) {
      throwIndexException(_T("setRow:Row %s out of range"), formatSize(row).cstr());
    }
    if(v.getDimension() != getColumnCount()) {
      throwMatrixException(_T("setRow:Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr(), v.getDimensionString().cstr());
    }

    T *p = m_a + index(row,0);
    for(size_t c = 0; c < getColumnCount(); c++) {
      *(p++) = v(c);
    }
    return *this;
  }

  MatrixTemplate<T> &setColumn(size_t column, const VectorTemplate<T> &v) {
    if(column >= getColumnCount()) {
      throwIndexException(_T("setColumn:Column %s out of range"), formatSize(column).cstr());
    }
    if(v.getDimension() != getRowCount()) {
      throwMatrixException(_T("setColumn:Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr(), v.getDimensionString().cstr());
    }
    T *p = m_a + index(0, column);
    for(size_t r = 0; r < getRowCount(); r++, p += getColumnCount()) {
      *p = v(r);
    }
    return *this;
  }

  MatrixTemplate<T> &swapRows(size_t r1, size_t r2) {
    if(r1 >= getRowCount()) {
      throwIndexException(_T("swapRows:r1=%s out of range"), formatSize(r1).cstr());
    }
    if(r2 >= getRowCount()) {
      throwIndexException(_T("swapRows:r2=%s out of range"), formatSize(r2).cstr());
    }
    if (r1 != r2) {
      T *p1 = m_a + index(r1, 0);
      T *p2 = m_a + index(r2, 0);
      for(size_t c = 0; c < getColumnCount(); c++, p1++, p2++) {
        std::swap(*p1, *p2);
      }
    }
    return *this;
  }

  VectorTemplate<T> getDiagonal() const {
    if(!isSquare()) {
      throwIndexException(_T("getDiagonal:Matrix not square"));
    }
    VectorTemplate<T> result(getColumnCount());
    for(size_t r = 0; r < getColumnCount(); r++) {
      result(r) = m_a[index(r,r)];
    }
    return result;
  }

  MatrixTemplate<T> getSubMatrix(size_t row, size_t column, size_t rowCount, size_t columnCount) const {
    if(row + rowCount > getRowCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                          , formatSize(row).cstr()
                          , formatSize(column).cstr()
                          , formatSize(rowCount).cstr()
                          , formatSize(columnCount).cstr()
                          , formatSize(getRowCount()).cstr());
    }
    if(column + columnCount > getColumnCount()) {
      throwMatrixException(_T("getSubMatrix:Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          , formatSize(row).cstr()
                          , formatSize(column).cstr()
                          , formatSize(rowCount).cstr()
                          , formatSize(columnCount).cstr()
                          , formatSize(getColumnCount()).cstr());
    }

    MatrixTemplate<T> result(rowCount, columnCount);
    for(size_t r = row, rr = 0; rr < rowCount; r++, rr++) {
      for(size_t c = column, rc = 0; rc < columnCount; c++, rc++) {
        result.m_a[result.index(rr,rc)] = m_a[index(r,c)];
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
                           , formatSize(row).cstr()
                           , formatSize(column).cstr()
                           , formatSize(src.getRowCount()).cstr()
                           , formatSize(src.getColumnCount()).cstr()
                           , formatSize(getRowCount()).cstr());
    }
    if(column + src.getColumnCount() > getColumnCount()) {
      throwMatrixException(_T("setSubMatrix:Cannot set subMatrix at offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          , formatSize(row).cstr()
                          , formatSize(column).cstr()
                          , formatSize(src.getRowCount()).cstr()
                          , formatSize(src.getColumnCount()).cstr()
                          , formatSize(getColumnCount()).cstr());
    }

    const size_t rowCount    = src.getRowCount();
    const size_t columnCount = src.getColumnCount();
    for(size_t r = row, sr = 0; sr < rowCount; r++, sr++) {
      for(size_t c = column, sc = 0; sc < columnCount; c++, sc++) {
        m_a[index(r,c)] = src.m_a[src.index(sr,sc)];
      }
    }
    return *this;
  }

  static MatrixTemplate<T> one(size_t dim) {
    MatrixTemplate<T> result(dim, dim);
    for(size_t  i = 0; i < dim; i++) {
      result.m_a[result.index(i,i)] = 1;
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
    const T *spl = lts.m_a, *last = spl + lts.m_dim.getElementCount(), *spr = rhs.m_a;
    T       *dp  = result.m_a;
    while(spl < last) *(dp++) = *(spl++) + *(spr++);
    return result;
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &lts, const MatrixTemplate<T> &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    lts.checkSameDimension(_T("operator-"), rhs);

    MatrixTemplate<T> result(rows, columns);
    const T *spl = lts.m_a, *last = spl + lts.m_dim.getElementCount(), *spr = rhs.m_a;
    T       *dp  = result.m_a;
    while(spl < last) *(dp++) = *(spl++) - *(spr++);
    return result;
  }

  friend MatrixTemplate<T> operator-(const MatrixTemplate<T> &m) {
    const size_t rows    = m.getRowCount();
    const size_t columns = m.getColumnCount();

    MatrixTemplate<T> result(rows, columns);
    const T *sp = m.m_a, *last = sp + m.m_dim.getElementCount();
    T       *dp  = result.m_a;
    while(sp < last) *(dp++) = -*(sp++);
    return result;
  }

  MatrixTemplate<T> &operator+=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator+="), rhs);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = m_a;
    while(sp < last) *(dp++) += *(sp++);
    return *this;
  }

  MatrixTemplate<T> &operator-=(const MatrixTemplate<T> &rhs) {
    checkSameDimension(_T("operator-="), rhs);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = m_a;
    while(sp < last) *(dp++) -= *(sp++);
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
        sum += lts.m_a[lts.index(r,c)] * rhs[c];
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
        sum += lts[r] * rhs.m_a[rhs.index(r,c)];
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

    T *dp = result.m_a;
    for(size_t r = 0; r < rRows; r++) {
      for(size_t c = 0; c < rColumns; c++) {
        T sum = 0;
        const T *lp = lts.m_a + lts.index(r,0);
        const T *rp = rhs.m_a + lts.index(0,c);
        for(size_t k = 0; k < maxK; k++, rp += rColumns) {
          sum += *(lp++) * *rp;
        }
        *(dp++) = sum;
      }
    }
    return result;
  }

  friend MatrixTemplate<T> operator*(const T &d, const MatrixTemplate<T> &rhs) {
    const size_t rows    = rhs.getRowCount();
    const size_t columns = rhs.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) * d;
    return result;
  }

  friend MatrixTemplate<T> operator*(const MatrixTemplate<T> &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    const T *sp = lts.m_a, *last = sp + lts.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) * d;
    return result;
  }

  friend MatrixTemplate<T> operator/(const MatrixTemplate<T> &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate<T> result(rows, columns);

    const T *sp = lts.m_a, *last = sp + lts.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) / d;
    return result;
  }

  MatrixTemplate<T> &operator*=(const T &d) {
    T *p = m_a, *last = p + m_dim.getElementCount();
    while(p < last) *(p++) *= d;
    return *this;
  }

  MatrixTemplate<T> &operator/=(const T &d) {
    T *p = m_a, *last = p + m_dim.getElementCount();
    while(p < last) *(p++) /= d;
    return *this;
  }

  friend MatrixTemplate<T> transpose(const MatrixTemplate<T> &m) {
    MatrixTemplate<T> result(m.getColumnCount(), m.getRowCount());
    for(size_t r = 0; r < m.getRowCount(); r++) {
      for(size_t c = 0; c < m.getColumnCount(); c++) {
        result(c,r) = m(r,c);
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
        const T &aij = A(i,j);
        for(size_t k = 0, alfa = p*i; k < p; k++, alfa++) {
          for(size_t l = 0, beta = q*j; l < q; l++, beta++) {
            result(alfa,beta) = aij * B(k,l);
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
    const T *p1 = m1.m_a, *last = p1 + m1.m_dim.getElementCount();
    const T *p2 = m2.m_a;
    while(p1 < last) {
      if(!((*p1++) == *(p2++))) {
        return false;
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
        out << p << a(r,c) << _T(" ");
      }
      out << _T("\n");
    }
    return out;
  }

  friend tistream &operator>>(tistream &in, MatrixTemplate<T> &a) {
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        in >> a(r,c);
      }
    }
    return in;
  }

  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }
};
