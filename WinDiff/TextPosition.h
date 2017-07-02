#pragma once

class TextPosition { // position in textDocument
public:
  int m_line, m_column;

  inline TextPosition() {
    reset();
  }

  inline TextPosition(int line, int column) {
    set(line,column);
  }

  inline void set(int line, int column) {
    m_line = line; m_column = column;
  }

  inline void reset() {
    set(-1,-1);
  }

  inline bool isEmpty() const {
    return m_line < 0;
  }

  inline int cmp(const TextPosition &a) const {
    const int c = m_line - a.m_line;
    if(c) return c;
    return m_column - a.m_column;
  }

  inline String toString() const {
    return isEmpty() ? _T("Empty") : format(_T("(l,c):(%3d,%2d)"), m_line,m_column);
  }
};

inline bool operator==(const TextPosition &a1, const TextPosition &a2) {
  return a1.cmp(a2) == 0;
}

inline bool operator!=(const TextPosition &a1, const TextPosition &a2) {
  return a1.cmp(a2) != 0;
}

inline bool operator>=(const TextPosition &a1, const TextPosition &a2) {
  return a1.cmp(a2) >= 0;
}

inline bool operator<=(const TextPosition &a1, const TextPosition &a2) {
  return a1.cmp(a2) <= 0;
}

class TextPositionPair {
public:
  TextPosition m_pos1,m_pos2;

  inline TextPositionPair() {
  }

  inline TextPositionPair(const TextPosition &p1, const TextPosition &p2) : m_pos1(p1), m_pos2(p2) {
  }

  inline TextPositionPair(int line1, int column1, int line2, int column2) : m_pos1(line1,column1), m_pos2(line2,column2) {
  }

  inline TextPositionPair(int line, int column, int len) : m_pos1(line,column), m_pos2(line,column+len) {
  }

  inline bool isEmpty() const {
    return m_pos1.isEmpty();
  }

  inline int getLineCount() const {
    return isEmpty() ? 0 : (m_pos2.m_line - m_pos1.m_line + 1);
  }

  inline String toString() const {
    return isEmpty() ? _T("Empty") : format(_T("P1:%-15s - %-15s"), m_pos1.toString().cstr(), m_pos2.toString().cstr());
  }
};

class MousePosition : public CPoint { // Position in window, in characters
public:
  CPoint m_distance;  // Distance to nearest windowposition, in characters
  inline MousePosition() : m_distance(0,0) {
  };
  inline MousePosition(const CPoint &p) : CPoint(p), m_distance(0,0) {
  }
  inline MousePosition(const CPoint &p, CPoint &distance) : CPoint(p), m_distance(distance) {
  }
  inline bool isOutsideWindow() const {
    return (m_distance.x != 0) || (m_distance.y != 0);
  }
};
