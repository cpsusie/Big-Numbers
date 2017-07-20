#include "stdafx.h"
#include "EdgeMatrix.h"

static const Direction leftTurn[]    = { E, N, W, S, NODIR };
static const Direction rightTurn[]   = { W, S, E, N, NODIR };
static const Direction oppositeDir[] = { N, W, S, E, NODIR };

Direction turnLeft(  Direction dir) {
  return leftTurn[dir];
}

Direction turnRight( Direction dir) {
  return rightTurn[dir];
}

Direction turnAround(Direction dir) {
  return oppositeDir[dir];
}

const CPoint EdgeMatrix::s_dirStep[4] = {
  CPoint( 0, 1) // S
 ,CPoint( 1, 0) // E
 ,CPoint( 0,-1) // N
 ,CPoint(-1, 0) // W
};

const CPoint EdgeMatrix::s_leftStep[4] = {
  CPoint( 1, 0) // S
 ,CPoint( 0,-1) // E
 ,CPoint(-1, 0) // N
 ,CPoint( 0, 1) // W
};

// Inside attribute bits
#define A00 0x000001
#define A01 0x000004
#define A02 0x000010
#define A10 0x000100
#define A11 0x000400
#define A12 0x001000
#define A20 0x010000
#define A21 0x040000
#define A22 0x100000

// Dirty attribute bits
#define D00 (A00 << 1)
#define D01 (A01 << 1)
#define D02 (A02 << 1)
#define D10 (A10 << 1)
#define D11 (A11 << 1)
#define D12 (A12 << 1)
#define D20 (A20 << 1)
#define D21 (A21 << 1)
#define D22 (A22 << 1)

EdgeMatrix::EdgeMatrix() {
  m_allAttr = D00 | D01 | D02 | D10 | A11 | D12 | D20 | D21 | D22;
}

Direction EdgeMatrix::findStepDirection(Direction dir) const {
  switch(dir) {
  case S:
    if((m_allAttr & (A10|A00)) == A10) return W;
    if((m_allAttr & (A21|A10)) == A21) return S;
    if((m_allAttr & (A12|A21)) == A12) return E;
    return (m_allAttr & A01) ? N : NODIR;
  case E:
    if((m_allAttr & (A21|A20)) == A21) return S;
    if((m_allAttr & (A12|A21)) == A12) return E;
    if((m_allAttr & (A01|A12)) == A01) return N;
    return (m_allAttr & A10) ? W : NODIR;
  case N:
    if((m_allAttr & (A12|A22)) == A12) return E;
    if((m_allAttr & (A01|A12)) == A01) return N;
    if((m_allAttr & (A10|A01)) == A10) return W;
    return (m_allAttr & A21) ? S : NODIR;
  case W:
    if((m_allAttr & (A01|A02)) == A01) return N;
    if((m_allAttr & (A10|A01)) == A10) return W;
    if((m_allAttr & (A21|A10)) == A21) return S;
    return (m_allAttr & A12) ? E : NODIR;
  }
  return NODIR;
}

void EdgeMatrix ::adjustAttributes(Direction dir) {
  switch(dir) {
  case S    :
    m_allAttr =  (m_allAttr >> 8)   /* not and mask */         | (D20|D21|D22);
    break;
  case E :
    m_allAttr = ((m_allAttr >> 2) & (A00|A01|A10|A11|A20|A21)) | (D02|D12|D22);
    break;
  case N    :
    m_allAttr = ((m_allAttr << 8) & (A10|A11|A12|A20|A21|A22)) | (D00|D01|D02);
    break;
  case W    :
    m_allAttr = ((m_allAttr << 2) & (A01|A02|A11|A12|A21|A22)) | (D00|D10|D20);
    break;
  default   :
    throwInvalidArgumentException(__TFUNCTION__,_T("dir=%s"), s_directionName[dir]);
  }
}

#define GETATTR(r,c) ((m_attrRow[r] >> (2*(c))) & 3)

EdgeAttribute EdgeMatrix ::getLeftAttr(Direction dir) const {
  switch(dir) {
  case S    : return GETATTR(1,2);
  case E    : return GETATTR(0,1);
  case N    : return GETATTR(1,0);
  case W    : return GETATTR(2,1);
  default   :
    throwInvalidArgumentException(__TFUNCTION__,_T("dir=%s"), s_directionName[dir]);
    return 0;
  }
}

String EdgeMatrix::toString() const {
  String s;
  for(int r = 0; r < 3; r++) {
    s += format(_T("%d%d%d\n"), GETATTR(r,0), GETATTR(r,1), GETATTR(r,2));
  }
  return s;
}

const TCHAR *EdgeMatrix::s_directionName[5] = {
  _T("S")
 ,_T("E")
 ,_T("N")
 ,_T("W")
 ,_T("NODIR")
};
