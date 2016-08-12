#pragma once

class MatrixIndex {
public:
  size_t r, c;
  inline MatrixIndex() {
  }
  inline MatrixIndex(size_t _r, size_t _c) : r(_r), c(_c) {
  }
  inline bool operator==(const MatrixIndex &i) const {
    return (r == i.r) && (c == i.c);
  }
  inline bool operator!=(const MatrixIndex &i) const {
    return (r != i.r) || (c != i.c);
  }
};

class MatrixDimension {
public:
  size_t rowCount, columnCount;
  inline MatrixDimension() {
  }
  inline MatrixDimension(size_t _rowCount, size_t _columnCount) : rowCount(_rowCount), columnCount(_columnCount) {
  }
  inline bool operator==(const MatrixDimension &d) const {
    return (rowCount == d.rowCount) && (columnCount == d.columnCount);
  }
  inline bool operator!=(const MatrixDimension &d) const {
    return (rowCount != d.rowCount) || (columnCount != d.columnCount);
  }
  inline bool isLegalIndex(const MatrixIndex &i) const {
    return (i.r < rowCount) && (i.c < columnCount);
  }
  inline bool isLegalIndex(size_t r, size_t c) const {
    return (r < rowCount) && (c < columnCount);
  }
  inline size_t getElementCount() const {
    return rowCount * columnCount;
  }
  inline String toString() const {
    return format(_T("(%s,%s)"), format1000(rowCount).cstr(), format1000(columnCount).cstr());
  }
};
