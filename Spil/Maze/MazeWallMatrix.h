#pragma once

#include <MatrixTemplate.h>
#include "MazeWall.h"

class MazeWallMatrix : public MatrixTemplate<MazeWall> {
private:
  const String     m_name;
  const ColorIndex m_colorIndex;

public:
  MazeWallMatrix(const String &name, int r, int c, ColorIndex colorIndex = Black) 
  : m_name(name), m_colorIndex(colorIndex)
  , MatrixTemplate<MazeWall>(r,c)
  {
  }

  const String &getName() const {
    return m_name;
  }

  void paint(CDC &dc);
};

#define ADDWALL(m,r,c) if(m.getDimension().isLegalIndex(r,c)) wa[wc++] = &m(r,c)

