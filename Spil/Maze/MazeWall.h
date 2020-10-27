#pragma once

#include <MatrixTemplate.h>
#include <MFCUtil/WinTools.h>

class MazeCell;

#define _BLACK RGB(0,0,0)
#define _WHITE RGB(255,255,255)
#define _RED   RGB(255,0,0)
#define _GREEN RGB(0,255,0)
#define _BLUE  RGB(0,0,255)
#define _GRAY  RGB(192,192,192)
#define _CYAN  RGB(255,0,255)

typedef enum {
  Black
 ,White
 ,Red
 ,Green
 ,Blue
 ,Gray
 ,Cyan
} ColorIndex;

inline CPen *getSolidpen(ColorIndex ci) {
  extern CPen solidPen[];
  return &solidPen[ci];
}

class MazeWall {
public:
  CPoint    m_p1, m_p2;
  MazeCell *m_c1, *m_c2;
  bool      m_visible;

  MazeWall(int dummy=0) {
    m_c1 = m_c2 = nullptr;
    m_visible = true;
  }

  MazeWall(CPoint p1, CPoint p2) {
    m_p1 = p1;
    m_p2 = p2;
    m_c1 = m_c2 = nullptr;
    m_visible   = true;
  }

  inline int getMaxX() const {
    return max(m_p1.x, m_p2.x);
  }

  inline int getMaxY() const {
    return max(m_p1.y, m_p2.y);
  }

  inline bool isVisible() const {
    return m_visible;
  }

  void paint(CDC &dc, ColorIndex ci) const {
    dc.SelectObject(getSolidpen(ci));
    dc.MoveTo(m_p1);
    dc.LineTo(m_p2);
  }

  String toString() const {
    return format(_T("(%d,%d)-(%d,%d)"), m_p1.x, m_p1.y, m_p2.x, m_p2.y);
  }
};

inline String toString(const MazeWall &w) {
  return w.toString();
}

typedef CompactArray<MazeWall*> WallArray;
