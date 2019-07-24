#pragma once

#include "MatrixDimension.h"
#include "BitSet.h"

class BitMatrix : private BitSet {
  friend class BitMatrixIterator;
  friend class BitMatrixRowIterator;
  friend class BitMatrixColumnIterator;
private:
  MatrixDimension m_dim;
protected:
  inline size_t getIndex(size_t r, size_t c) const {
    return r * m_dim.columnCount + c;
  }
  inline MatrixIndex indexToPoint(size_t index) const {
    return MatrixIndex(index / m_dim.columnCount, index % m_dim.columnCount);
  }
  void checkSameDimension(const TCHAR *method, const BitMatrix &m) const;
  void checkValidRow(const TCHAR *method, size_t r) const;
  void checkValidColumn(const TCHAR *method, size_t c) const;

public:
  BitMatrix(size_t rowCount, size_t columnCount)
    : m_dim(rowCount, columnCount)
    , BitSet(rowCount*columnCount)
  {
  }
  BitMatrix(const MatrixDimension &dim)
    : m_dim(dim)
    , BitSet(dim.getElementCount())
  {
  }
  BitMatrix &operator=(const BitMatrix &m) {
    __super::operator=(m);
    m_dim = m.m_dim;
    return *this;
  }
  void set(size_t r, size_t c, bool v);
  inline void set(const MatrixIndex &i, bool v) {
    set(i.r, i.c, v);
  }
  inline bool get(size_t r, size_t c) const {
    return contains(getIndex(r, c));
  }
  inline bool get(const MatrixIndex &i) const {
    return contains(getIndex(i.r, i.c));
  }
  inline void clear() {
    __super::clear();
  }
  inline void invert() {
    __super::invert();
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
  BitMatrix &setDimension(const MatrixDimension &dim);
  inline size_t size() const {    // Number of true elements.
    return __super::size();
  }
  inline bool isEmpty() const {
    return __super::isEmpty();
  }
  BitSet &getRow(size_t r, BitSet &s) const;
  BitSet &getColumn(size_t c, BitSet &s) const;
  inline BitSet getRow(size_t r) const {
    BitSet result(getColumnCount());
    return getRow(r, result);
  }
  inline BitSet getColumn(size_t c) const {
    BitSet result(getRowCount());
    return getColumn(c, result);
  }
  void    setRow(size_t r, bool v);
  void    setColumn(size_t c, bool v);
  bool operator==(const BitMatrix &m) const;
  inline bool operator!=(const BitMatrix &m) const {
    return !(*this == m);
  }
  BitMatrix &operator&=(const BitMatrix &m);
  BitMatrix &operator|=(const BitMatrix &m);
  BitMatrix &operator^=(const BitMatrix &m);
  BitMatrix &operator-=(const BitMatrix &m);
  BitMatrix operator*  (const BitMatrix &rhs) const; // like normal matrix multiplication, using bool instead of floating points
  Iterator<MatrixIndex> getIterator();
  Iterator<MatrixIndex> getRowIterator(size_t r);
  Iterator<MatrixIndex> getColumnIterator(size_t c);
  String toString() const;
  String getDimensionString() const {
    return format(_T("Dimension=%s"), m_dim.toString().cstr());
  }
};
