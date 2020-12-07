#pragma once

#include "StreamParameters.h"
#include "VectorTemplate.h"
#include "MatrixDimension.h"

template<typename T> class MatrixTemplate {
private:
  AbstractVectorAllocator<T> *m_va;
  T                          *m_a;
  MatrixDimension             m_dim;

  static void throwMatrixException(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    throwException(_T("%s:%s"), method, msg.cstr());
  }

  void throwIndexException(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    throwMatrixException(method, _T("%s. %s"), msg.cstr(), getDimensionString().cstr());
  }

  void checkIndex(const TCHAR *method, size_t r, size_t c) const {
    if(!m_dim.isLegalIndex(r,c)) {
      throwIndexException(method
                         ,_T("Index (%s, %s) out of range")
                         ,formatSize(r).cstr()
                         ,formatSize(c).cstr()
                         );
    }
  }

#define _VALIDATEISSQUAREMATRIX(m) \
{ if(!(m).isSquare()) (m).throwNotSquareMatrixException(__TFUNCTION__, _T(#m)); }

  inline size_t index(size_t r, size_t c) const {
    return m_dim.columnCount * r + c;
  }
  inline size_t index(const MatrixIndex &i) const {
    return index(i.r, i.c);
  }

  T *allocate(size_t rows, size_t columns, bool initialize) const {
    const size_t n = rows * columns;
    if(n == 0) return nullptr;
    T *a = m_va ? m_va->allocVector(n) : new T[n]; TRACE_NEW(a);
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

  inline AbstractVectorAllocator<T> *getVectorAllocator() const {
    return m_va;
  }

  void checkRowIndex(const TCHAR *method, size_t row) const {
    if(row >= getRowCount()) {
      throwIndexException(method
                         ,_T("Row %s out of range")
                         ,formatSize(row).cstr());
    }
  }

  void checkColumnIndex(const TCHAR *method, size_t column) const {
    if(column >= getColumnCount()) {
      throwIndexException(method
                         ,_T("Column %s out of range")
                         ,formatSize(column).cstr());
    }
  }

public:
  inline MatrixTemplate(AbstractVectorAllocator<T> *va = nullptr) : m_va(va), m_a(nullptr), m_dim(0,0) {
  }

  explicit MatrixTemplate(const T &coef, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(1, 1, false);
    m_a[0] = coef;
  }

  inline MatrixTemplate(size_t rows, size_t columns, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(rows, columns, true);
  }

  explicit MatrixTemplate(const MatrixDimension &dim, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(dim.rowCount, dim.columnCount, true);
  }

  MatrixTemplate(const MatrixTemplate &src, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    init(src.getRowCount(), src.getColumnCount(), false);
    T       *dp = m_a;
    const T *sp = src.m_a, *last = sp + m_dim.getElementCount();
    while(sp < last) *(dp++) = *(sp++);
  }

  explicit MatrixTemplate(const VectorTemplate<T> &diagonal, AbstractVectorAllocator<T> *va = nullptr) : m_va(va) {
    const size_t d = diagonal.getDimension();
    init(d, d, true);
    for(size_t i = 0; i < d; i++) {
      m_a[index(i,i)] = diagonal[i];
    }
  }

  void checkSameDimension(const TCHAR *method, const MatrixTemplate &m) const {
    if(!hasSameDimension(m)) {
      throwMatrixException(method
                          ,_T("Invalid dimension. Left.%s. Right.%s")
                          ,getDimensionString().cstr()
                          ,m.getDimensionString().cstr()
                          );
    }
  }

  MatrixTemplate &operator=(const MatrixTemplate &src) {
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

  MatrixTemplate &setDimension(const MatrixDimension &dim) {
    if(dim != m_dim) {
      T *newa = allocate(dim.rowCount, dim.columnCount, true);
      const size_t copyr = __min(dim.rowCount   , getRowCount()   );
      const size_t copyc = __min(dim.columnCount, getColumnCount());
      for(size_t r = 0, t = 0; r < copyr; r++, t += dim.columnCount) {
        for(T *dp = newa + t, *sp = m_a + index(r, 0), *last = sp + copyc; sp < last;) {
          *(dp++) = *(sp++);
        }
      }
      cleanup();
      m_a   = newa;
      m_dim = dim;
    }
    return *this;
  }

  MatrixTemplate &setDimension(size_t rowCount, size_t columnCount) {
    return setDimension(MatrixDimension(rowCount, columnCount));
  }

  // make it square
  MatrixTemplate &setDimension(size_t dim) {
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

  inline bool hasSameDimension(const MatrixTemplate &m) const {
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
#if defined(_DEBUG)
    checkIndex(__TFUNCTION__, r, c);
#endif
    return m_a[index(r,c)];
  }

  inline const T &operator()(size_t r, size_t c) const {
#if defined(_DEBUG)
    checkIndex(__TFUNCTION__, r, c);
#endif
    return m_a[index(r,c)];
  }

  inline T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    return m_a[randSizet(m_dim.getElementCount(), rnd)];
  }
  inline const T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    return m_a[randSizet(m_dim.getElementCount(), rnd)];
  }
  // row must be [1..getRowCount()-1]
  T &subDiagonal(size_t row) {
    _VALIDATEISSQUAREMATRIX(*this);
    checkIndex(__TFUNCTION__, row, row-1);
    return m_a[index(row,row-1)];
  }

  // row must be [1..getRowCount()-1]
  const T &subDiagonal(size_t row) const {
    _VALIDATEISSQUAREMATRIX(*this);
    checkIndex(__TFUNCTION__, row, row-1);
    return m_a[index(row,row-1)];
  }

  VectorTemplate<T> getRow(size_t row) const {
    checkRowIndex(__TFUNCTION__, row);
    const size_t cn = getColumnCount();
    VectorTemplate<T> result(cn);
    const T *p = m_a + index(row,0);
    for(size_t c = 0; c < cn;) {
      result[c++] = *(p++);
    }
    return result;
  }

  VectorTemplate<T> getColumn(size_t column) const {
    checkColumnIndex(__TFUNCTION__, column);
    const size_t rn = getRowCount();
    const size_t cn = getColumnCount();
    VectorTemplate<T> result(rn);
    const T *p = m_a + index(0, column);
    for(size_t r = 0; r < rn; p += cn) {
      result[r++] = *p;
    }
    return result;
  }

  MatrixTemplate &setRow(size_t row, const VectorTemplate<T> &v) {
    checkRowIndex(__TFUNCTION__, row);
    const size_t cn = getColumnCount();
    if(v.getDimension() != cn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr()
                          ,v.getDimensionString().cstr());
    }

    T *p = m_a + index(row,0);
    for(size_t c = 0; c < cn;) {
      *(p++) = v(c++);
    }
    return *this;
  }

  MatrixTemplate &setColumn(size_t column, const VectorTemplate<T> &v) {
    checkColumnIndex(__TFUNCTION__, column);
    const size_t rn = getRowCount();
    const size_t cn = getColumnCount();
    if(v.getDimension() != rn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Invalid dimension. %s. Vector.%s")
                          ,getDimensionString().cstr()
                          ,v.getDimensionString().cstr()
                          );
    }
    T *p = m_a + index(0, column);
    for(size_t r = 0; r < rn; p += cn) {
      *p = v(r++);
    }
    return *this;
  }

  MatrixTemplate &setRow(size_t row, const T &v) {
    if(row >= getRowCount()) {
      throwIndexException(__TFUNCTION__
                         ,_T("Row %s out of range")
                         ,formatSize(row).cstr()
                         );
    }
    T *p = m_a + index(row,0);
    const T *endp = p + getColumnCount();
    while(p < endp) {
      *(p++) = v;
    }
    return *this;
  }

  MatrixTemplate &setColumn(size_t column, const T &v) {
    const size_t cn  = getColumnCount();
    if(column >= cn) {
      throwIndexException(__TFUNCTION__
                         ,_T("Column %s out of range")
                         ,formatSize(column).cstr()
                         );
    }
    T *p = m_a + index(0, column);
    const T     *end = m_a + index(getRowCount(), column);
    for(;p < end; p += cn) {
      *p = v;
    }
    return *this;
  }

  MatrixTemplate &setValue(size_t row, size_t column, size_t rowCount, size_t columnCount, const T &v) {
    const size_t cn = getColumnCount();
    if(row + rowCount > getRowCount()) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot set value from offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(rowCount).cstr()
                          ,formatSize(columnCount).cstr()
                          ,formatSize(getRowCount()).cstr());
    }
    if(column + columnCount > cn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot set value from offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(rowCount).cstr()
                          ,formatSize(columnCount).cstr()
                          ,formatSize(cn).cstr());
    }


    const T *rightp  = m_a + index(row         , column+columnCount);
    const T *bottomp = m_a + index(row+rowCount, column            );
    for(T *leftp = m_a + index(row, column); leftp < bottomp; leftp += cn, rightp += cn) {
      for(T *vp = leftp; vp < rightp;) {
        *(vp++) = v;
      }
    }
    return *this;
  }

  MatrixTemplate &swapRows(size_t r1, size_t r2) {
    if(r1 >= getRowCount()) {
      throwIndexException(__TFUNCTION__
                         ,_T("r1=%s out of range")
                         ,formatSize(r1).cstr()
                         );
    }
    if(r2 >= getRowCount()) {
      throwIndexException(__TFUNCTION__
                         ,_T("r2=%s out of range")
                         ,formatSize(r2).cstr()
                         );
    }
    if(r1 != r2) {
      T *p1 = m_a + index(r1, 0);
      T *p2 = m_a + index(r2, 0);
      const T *lastp1 = p1 + getColumnCount();
      while(p1 < lastp1) {
        std::swap(*(p1++), *(p2++));
      }
    }
    return *this;
  }

  VectorTemplate<T> getDiagonal() const {
    _VALIDATEISSQUAREMATRIX(*this);
    const size_t cn = getColumnCount();
    VectorTemplate<T> result(cn);
    for(size_t r = 0; r < cn; r++) {
      result(r) = m_a[index(r,r)];
    }
    return result;
  }

  MatrixTemplate getSubMatrix(size_t row, size_t column, size_t rowCount, size_t columnCount) const {
    const size_t rn = getRowCount();
    const size_t cn = getColumnCount();

    if(row + rowCount > rn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(rowCount).cstr()
                          ,formatSize(columnCount).cstr()
                          ,formatSize(rn).cstr());
    }
    if(column + columnCount > cn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot get subMatrix from offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(rowCount).cstr()
                          ,formatSize(columnCount).cstr()
                          ,formatSize(cn).cstr());
    }

    MatrixTemplate result(rowCount, columnCount);
    T *dstLeftp = result.m_a, *dstLastp = result.m_a + result.index(rowCount,0);
    const T *srcLeftp = m_a + index(row,column), *srcRightp = srcLeftp + columnCount;
    for(;dstLeftp < dstLastp; dstLeftp += columnCount, srcLeftp += cn, srcRightp += cn) {
      T *dstp = dstLeftp;
      for(const T *srcp = srcLeftp; srcp < srcRightp;) {
        *(dstp++) = *(srcp++);
      }
    }
    return result;
  }

  MatrixTemplate getSubMatrix(size_t  row, size_t  column) const {
    return getSubMatrix(row, column, getRowCount()-row, getColumnCount()-column);
  }

  // Return *this
  MatrixTemplate &setSubMatrix(size_t  row, size_t  column, const MatrixTemplate &src) {
    const size_t rn          = getRowCount();
    const size_t cn          = getColumnCount();
    const size_t srcRowCount = src.getRowCount();
    const size_t srcColCount = src.getColumnCount();
    if(row + srcRowCount > rn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot set subMatrix at offset (%s,%s) with dimension(%s,%s). RowCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(srcRowCount).cstr()
                          ,formatSize(srcColCount).cstr()
                          ,formatSize(rn).cstr());
    }
    if(column + srcColCount > cn) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Cannot set subMatrix at offset (%s,%s) with dimension(%s,%s). ColumnCount=%s")
                          ,formatSize(row).cstr()
                          ,formatSize(column).cstr()
                          ,formatSize(srcRowCount).cstr()
                          ,formatSize(srcColCount).cstr()
                          ,formatSize(cn).cstr());
    }

    T       *dstLeftp = m_a + index(row,column), *dstRightp = dstLeftp + srcColCount;
    const T *dstEndp  = m_a + index(row+srcRowCount,column);
    for(const T *srcp = src.m_a; dstLeftp < dstEndp; dstLeftp += cn, dstRightp += cn) {
      for(T *dstp = dstLeftp; dstp < dstRightp;) {
        *(dstp++) = *(srcp++);
      }
    }
    return *this;
  }

  // Return dst
  static MatrixTemplate _1(size_t dim) {
    MatrixTemplate result(dim, dim);
    for(size_t  i = 0; i < dim; i++) {
      result.m_a[result.index(i,i)] = 1;
    }
    return result;
  }

  static MatrixTemplate _0(size_t rows, size_t columns) {
    return MatrixTemplate(rows, columns);
  }

  friend MatrixTemplate operator+(const MatrixTemplate &lts, const MatrixTemplate &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    lts.checkSameDimension(__TFUNCTION__, rhs);

    MatrixTemplate result(rows, columns);
    const T *spl = lts.m_a, *last = spl + lts.m_dim.getElementCount(), *spr = rhs.m_a;
    T       *dp  = result.m_a;
    while(spl < last) *(dp++) = *(spl++) + *(spr++);
    return result;
  }

  friend MatrixTemplate operator-(const MatrixTemplate &lts, const MatrixTemplate &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    lts.checkSameDimension(__TFUNCTION__, rhs);

    MatrixTemplate result(rows, columns);
    const T *spl = lts.m_a, *last = spl + lts.m_dim.getElementCount(), *spr = rhs.m_a;
    T       *dp  = result.m_a;
    while(spl < last) *(dp++) = *(spl++) - *(spr++);
    return result;
  }

  friend MatrixTemplate operator-(const MatrixTemplate &m) {
    const size_t rows    = m.getRowCount();
    const size_t columns = m.getColumnCount();

    MatrixTemplate result(rows, columns);
    const T *sp = m.m_a, *last = sp + m.m_dim.getElementCount();
    T       *dp  = result.m_a;
    while(sp < last) *(dp++) = -*(sp++);
    return result;
  }

  MatrixTemplate &operator+=(const MatrixTemplate &rhs) {
    checkSameDimension(__TFUNCTION__, rhs);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = m_a;
    while(sp < last) *(dp++) += *(sp++);
    return *this;
  }

  MatrixTemplate &operator-=(const MatrixTemplate &rhs) {
    checkSameDimension(__TFUNCTION__, rhs);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = m_a;
    while(sp < last) *(dp++) -= *(sp++);
    return *this;
  }

  friend VectorTemplate<T> operator*(const MatrixTemplate &lts, const VectorTemplate<T> &rhs) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();

    if(columns != rhs.getDimension()) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Invalid dimension. Matrix.%s, Vector.%s")
                          ,lts.getDimensionString().cstr()
                          ,rhs.getDimensionString().cstr());
    }

    VectorTemplate<T> result(rows);
    const T *leftp = lts.m_a;
    for(size_t r = 0; r < rows; r++, leftp += columns) {
      T sum = 0;
      const T *ltsp = leftp;
      for(size_t c = 0; c < columns;) {
        sum += *(ltsp++) * rhs[c++];
      }
      result[r] = sum;
    }
    return result;
  }

  friend VectorTemplate<T> operator*(const VectorTemplate<T> &lts, const MatrixTemplate &rhs) {
    const size_t rows    = rhs.getRowCount();
    const size_t columns = rhs.getColumnCount();

    if(lts.getDimension() != rows) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Invalid dimension. Vector.%s. Matrix.%s")
                          ,lts.getDimensionString().cstr()
                          ,rhs.getDimensionString().cstr());
    }

    VectorTemplate<T> result(columns);
    const T *topp = rhs.m_a;
    for(size_t  c = 0; c < columns; c++, topp++) {
      T sum = 0;
      const T *ltsp = topp;
      for(size_t r = 0; r < rows; ltsp += columns) {
        sum += lts[r++] * *ltsp;
      }
      result[c] = sum;
    }
    return result;
  }

  friend MatrixTemplate operator*(const MatrixTemplate &lts, const MatrixTemplate &rhs) {
    if(lts.getColumnCount() != rhs.getRowCount()) {
      throwMatrixException(__TFUNCTION__
                          ,_T("Invalid dimension. Left.%s. Right.%s")
                          ,lts.getDimensionString().cstr()
                          ,rhs.getDimensionString().cstr());
    }

    const size_t rRows    = lts.getRowCount();
    const size_t rColumns = rhs.getColumnCount();

    MatrixTemplate result(rRows, rColumns);

    const size_t ltscn = lts.getColumnCount(); // == rhs.getRowCount()

    T *dp = result.m_a;
    for(size_t r = 0; r < rRows; r++) {
      const T *ltsr0p = lts.m_a + lts.index(r,0), *ltsrlastp = ltsr0p + ltscn;
      for(size_t c = 0; c < rColumns; c++) {
        T sum = 0;
        const T *rp = rhs.m_a + rhs.index(0,c);
        for(const T *lp = ltsr0p; lp < ltsrlastp; rp += rColumns) {
          sum += *(lp++) * *rp;
        }
        *(dp++) = sum;
      }
    }
    return result;
  }

  MatrixTemplate &operator*=(const MatrixTemplate &rhs) {
    *this = *this * rhs;
    return *this;
  }

  friend MatrixTemplate operator*(const T &d, const MatrixTemplate &rhs) {
    const size_t rows    = rhs.getRowCount();
    const size_t columns = rhs.getColumnCount();
    MatrixTemplate result(rows, columns);

    const T *sp = rhs.m_a, *last = sp + rhs.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) * d;
    return result;
  }

  friend MatrixTemplate operator*(const MatrixTemplate &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate result(rows, columns);

    const T *sp = lts.m_a, *last = sp + lts.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) * d;
    return result;
  }

  friend MatrixTemplate operator/(const MatrixTemplate &lts, const T &d) {
    const size_t rows    = lts.getRowCount();
    const size_t columns = lts.getColumnCount();
    MatrixTemplate result(rows, columns);

    const T *sp = lts.m_a, *last = sp + lts.m_dim.getElementCount();
    T       *dp = result.m_a;
    while(sp < last) *(dp++) = *(sp++) / d;
    return result;
  }

  MatrixTemplate &operator*=(const T &d) {
    T *p = m_a, *last = p + m_dim.getElementCount();
    while(p < last) *(p++) *= d;
    return *this;
  }

  MatrixTemplate &operator/=(const T &d) {
    T *p = m_a, *last = p + m_dim.getElementCount();
    while(p < last) *(p++) /= d;
    return *this;
  }

  friend MatrixTemplate kroneckerSum(const MatrixTemplate &A, const MatrixTemplate &B) {
    _VALIDATEISSQUAREMATRIX(A);
    _VALIDATEISSQUAREMATRIX(B);
    const size_t a = A.getRowCount();
    const size_t b = B.getRowCount();
    return kroneckerProduct(A, _1(b)) + kroneckerProduct(_1(a), B);
  }

  friend MatrixTemplate kroneckerProduct(const MatrixTemplate &A, const MatrixTemplate &B) {
    const size_t m = A.getRowCount();
    const size_t n = A.getColumnCount();
    const size_t p = B.getRowCount();
    const size_t q = B.getColumnCount();
    MatrixTemplate result(m*p, n*q);

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

  friend bool operator==(const MatrixTemplate &m1, const MatrixTemplate &m2) {
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

  friend bool operator!=(const MatrixTemplate &m1, const MatrixTemplate &m2) {
    return !(m1 == m2);
  }

  virtual String toString() const {
    String result;
    for(size_t r = 0; r < getRowCount(); r++) {
      result += getRow(r).toString() + _T("\n");
    }
    return result;
  }

  template<typename OSTREAMTYPE> friend OSTREAMTYPE &operator<<(OSTREAMTYPE &out, const MatrixTemplate &a) {
    const StreamSize w = out.width();
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        out.width(w);
        out << a(r,c) << " ";
      }
      out << std::endl;
    }
    return out;
  }

  template<typename ISTREAMTYPE> friend ISTREAMTYPE &operator>>(ISTREAMTYPE &in, MatrixTemplate &a) {
    const FormatFlags flg = in.flags();
    in.flags(flg | std::ios::skipws);
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        in >> a(r,c);
      }
    }
    in.flags(flg);
    return in;
  }

  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }

  void throwNotSquareMatrixException(const TCHAR *method, const TCHAR *name) const {
    throwInvalidArgumentException(method
                                 ,_T("Matrix %s not square. %s")
                                 ,name
                                 ,getDimensionString().cstr()
                                 );
  }

};

// assume m is type derived from MatrixTemplate
template<typename MatrixType> MatrixType transpose(const MatrixType &m) {
  const size_t rc = m.getRowCount(), cc = m.getColumnCount();
  MatrixType result(cc, rc);
  for(size_t r = 0; r < rc; r++) {
    for(size_t c = 0; c < cc; c++) {
      result(c, r) = m(r, c);
    }
  }
  return result;
}
