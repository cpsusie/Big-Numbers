#include "stdafx.h"
#include <MyUtil.h>
#include <Random.h>
#include <BitSet.h>
#include <MatrixTemplate.h>
#include "TestBitSet.h"

class MatrixTester {
private:
  BitMatrix                     m_B;
  MatrixTemplate<unsigned char> m_M;

  void checkVectorBitSetEqual(const VectorTemplate<unsigned char> &v, const BitSet &b) const;
public:
  MatrixTester(const unsigned int rowCount, unsigned int colCount) 
    : m_B(rowCount, colCount)
    , m_M(rowCount, colCount)
  {
  }

  void set(unsigned int r, unsigned int c, bool v);
  bool get(unsigned int r, unsigned int c) const;
  void clear();
  void invert();
  void checkEqual()      const;
  void checkAllRows()    const;
  void checkAllColumns() const;
  const BitMatrix &getBitMatrix() const {
    return m_B;
  }
  void setRandom(int trueCount);
  MatrixTester operator&=(const MatrixTester &m);
  MatrixTester operator|=(const MatrixTester &m);
  MatrixTester operator^=(const MatrixTester &m);
  MatrixTester operator-=(const MatrixTester &m);
};

void MatrixTester::set(unsigned int r, unsigned int c, bool v) {
  m_B.set(r,c,v);
  m_M(r,c) = v?1:0;
}

bool MatrixTester::get(unsigned int r, unsigned int c) const {
  return m_B.get(r,c);
}

void MatrixTester::clear() {
  m_B.clear();
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) = 0;
    }
  }
}

void MatrixTester::invert() {
  m_B.invert();
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) = 1 - m_M(r,c);
    }
  }
}

void MatrixTester::setRandom(int trueCount) {
  clear();
  for(int i = 0; i < trueCount; i++) {
    int r = randInt(m_B.getRowCount());
    int c = randInt(m_B.getColumnCount());
    m_B.set(r,c,true);
    m_M(r,c) = 1;
  }
}

MatrixTester MatrixTester::operator&=(const MatrixTester &m) {
  m_B &= m.m_B;
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) &= m.m_M(r,c);
    }
  }
  return *this;
}

MatrixTester MatrixTester::operator|=(const MatrixTester &m) {
  m_B |= m.m_B;
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) |= m.m_M(r,c);
    }
  }
  return *this;
}

MatrixTester MatrixTester::operator^=(const MatrixTester &m) {
  m_B ^= m.m_B;
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) ^= m.m_M(r,c);
    }
  }
  return *this;

}

MatrixTester MatrixTester::operator-=(const MatrixTester &m) {
  m_B -= m.m_B;
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      m_M(r,c) &= ~m.m_M(r,c);
    }
  }
  return *this;
}

void MatrixTester::checkEqual() const {
  verify((m_B.getRowCount() == m_M.getRowCount()) && (m_B.getColumnCount() == m_M.getColumnCount()));
  const unsigned int rowCount = m_M.getRowCount();
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    for(unsigned int c = 0; c < colCount; c++) {
      const bool m1 = m_M(r,c) ? true : false;
      verify(m1 == m_B.get(r,c));
    }
  }
  checkAllRows();
  checkAllColumns();
}

void MatrixTester::checkAllRows() const {
  const unsigned int rowCount = m_M.getRowCount();
  for(unsigned int r = 0; r < rowCount; r++) {
    const VectorTemplate<unsigned char> V  = m_M.getRow(r);
    const BitSet                        bV = m_B.getRow(r);
    checkVectorBitSetEqual(V, bV);
  }
}

void MatrixTester::checkAllColumns() const {
  const unsigned int colCount = m_M.getColumnCount();
  for(unsigned int c = 0; c < colCount; c++) {
    const VectorTemplate<unsigned char> V  = m_M.getColumn(c);
    const BitSet                        bV = m_B.getColumn(c);
    checkVectorBitSetEqual(V, bV);
  }
}

void MatrixTester::checkVectorBitSetEqual(const VectorTemplate<unsigned char> &v, const BitSet &set) const {
  verify(v.getDimension() == set.getCapacity());
  const unsigned int n = v.getDimension();
  for(unsigned int i = 0; i < n; i++) {
    const bool b1 = v[i] ? true : false;
    const bool b2 = set.contains(i);
    verify(b1 == b2);
  }
}

class IteratorTester {
private:
  BitMatrix            m_matrix;
  CompactArray<CPoint> m_points;
public:
  IteratorTester(const CSize &size) : m_matrix(size) {
  }
  void add(const CPoint &p);
  void checkEqual();
};

static int pointCmp(const CPoint &p1, const CPoint &p2) {
  int c = p1.y - p2.y;
  if(c) return c;
  return p1.x - p2.x;
}

void IteratorTester::add(const CPoint &p) {
  if(m_matrix.get(p)) {
    return;
  }
  m_points.add(p);
  m_matrix.set(p,true);
}

void IteratorTester::checkEqual() {
  m_points.sort(pointCmp);
  int i = 0;
  for(Iterator<CPoint> it = m_matrix.getIterator(); it.hasNext();) {
    verify(i < m_points.size());
    const CPoint ap = m_points[i++];
    const CPoint mp = it.next();
    verify(ap == mp) ;
  }
  verify(i == m_points.size());
}

void testMatrixIterator() {
  for(int i = 0; i < 10; i++) {
    const int rowCount = randInt(1,100);
    const int colCount = randInt(1,100);
    const CSize size(colCount, rowCount);
//    printf("size:(%d,%d)\n", size.cx, size.cy);
    IteratorTester tester(size);
    for(int k = 0; k < 1000; k++) {
      const CPoint p(randInt(colCount), randInt(rowCount));
//      printf("add(%d,%d)\n", p.x, p.y);
      tester.add(p);
    }
    tester.checkEqual();
  }
}

void testBitMatrix() {
  const static TCHAR *objectToTest = _T("BitMatrix");
  _tprintf(_T("Testing %s%s\n"),objectToTest,spaceString(15-_tcslen(objectToTest),_T('.')).cstr());

  for(int i = 0; i < 10; i++) {
    const CSize size(randInt(1,40), randInt(1,70));
    MatrixTester m(size.cy, size.cx);

    for(int k = 0; k < 300; k++) {
      const unsigned int r = randInt(size.cy);
      const unsigned int c = randInt(size.cx);
      m.set(r,c,true);
    }
    m.checkEqual();

    BitMatrix b1 = m.getBitMatrix();

    verify(b1 == m.getBitMatrix());

    m.invert();
    m.checkEqual();

    verify(b1 != m.getBitMatrix());

    b1 = m.getBitMatrix();
    verify(b1 == m.getBitMatrix());

    m.clear();
    m.checkEqual();

    m.setRandom(300);
    MatrixTester m1(m);
    MatrixTester m2(size.cy, size.cx);
    m2.setRandom(300);

    m1 &= m2;
    m1.checkEqual();
    m1 = m;
    m1 |= m2;
    m1.checkEqual();

    m1 = m;
    m1 ^= m2;
    m1.checkEqual();

    m1 = m;
    m1 -= m2;
    m1.checkEqual();

  }
  
  testMatrixIterator();

  _tprintf(_T("%s ok!\n"),objectToTest);
}


