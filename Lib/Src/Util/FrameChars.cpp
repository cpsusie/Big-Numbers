#include "pch.h"
#include <Console.h>

FrameChars::FrameChars(int ul, int ur, int ll, int lr, int horz, int vert) {
  m_ul = ul;     // upper left corner
  m_ur = ur;     // upper right corner
  m_ll = ll;     // lower left corner
  m_lr = lr;     // lower right corner
  m_horz = horz; // horizontal line
  m_vert = vert; // vertical line
}

static FrameChars frames[] = {
  FrameChars(' ', ' ', ' ', ' ', ' ', ' ' ),   // NOFRAME
  FrameChars(218, 191, 192, 217, 196, 179 ),   // SINGLE_FRAME
  FrameChars(201, 187, 200, 188, 205, 186 ),   // DOUBLE_FRAME
  FrameChars(213, 184, 212, 190, 205, 179 ),   // DOUBLE_HORZ
  FrameChars(214, 183, 211, 189, 196, 186 ),   // DOUBLE_VERT
};

const FrameChars &FrameChars::getFrameChars(FrameType type) {
  return frames[type];
}
