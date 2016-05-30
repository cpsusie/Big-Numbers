#pragma once

class MatrixIndex {
public:
  unsigned int r, c;
  inline MatrixIndex() {
  }
  inline MatrixIndex(unsigned int _r, unsigned int _c) : r(_r), c(_c) {
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
  unsigned int rowCount, columnCount;
  inline MatrixDimension() {
  }
  inline MatrixDimension(int _rowCount, int _columnCount) : rowCount(_rowCount), columnCount(_columnCount) {
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
  inline bool isLegalIndex(unsigned int r, unsigned int c) const {
    return (r < rowCount) && (c < columnCount);
  }
  inline unsigned int getElementCount() const {
    return rowCount * columnCount;
  }
  inline String toString() const {
    return format(_T("(%lu,%lu)"), rowCount, columnCount);
  }
};
/*
class CRect {
public:
int left, top, right, bottom;
inline CRect() {
}
// from left, top, right, and bottom
inline CRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {
}
// retrieves the width
inline int Width() const {
return right - left;
}
// returns the height
int Height() const {
return bottom - top;
}
// returns the size
CSize Size() const {
return CSize(Width(), Height());
}
// reference to the top-left point
CPoint TopLeft() const {
return CPoint(left, top);
}
// reference to the bottom-right point
CPoint BottomRight() const {
return CPoint(right, bottom);
}
// the geometric center point of the rectangle
CPoint CenterPoint() const {
return CPoint((left + right) / 2, (top + bottom) / 2);
}
bool PtInRect(POINT point) const {
return (left <= point.x) && (point.x < right) && (top <= point.y) && (point.y < bottom);
}
};
*/
