#pragma once

typedef enum {
  S
 ,E
 ,N
 ,W
 ,NODIR
} Direction;

typedef BYTE EdgeAttribute;
#define ATTR_INSIDE 0x1
#define ATTR_DIRTY  0x2

Direction turnLeft(  Direction dir);
Direction turnRight( Direction dir);
Direction turnAround(Direction dir);

class EdgeMatrix {
private:
  union {
    ULONG m_allAttr;
    BYTE  m_attrRow[3];
  };
  static const TCHAR *s_directionName[5];
public:
  EdgeMatrix();
  inline bool isDirty(UINT i, UINT j) {
    return ((m_attrRow[i] >> (2*j)) & 3) == ATTR_DIRTY;
  }
  inline void setOutside(UINT i, UINT j) {
    m_attrRow[i] &= ~(3 << (2*j));
  }
  inline void setRowOutside(UINT i) {
    m_attrRow[i] = 0;
  }
  inline void setInside(UINT i, UINT j) {
    m_attrRow[i] ^= (3 << (2*j));
  }
  Direction findStepDirection(Direction dir) const;
  void      adjustAttributes( Direction dir);
  EdgeAttribute getLeftAttr(  Direction dir) const;
  String toString() const;
  static const CPoint s_dirStep[4];
  static const CPoint s_leftStep[4];
};
