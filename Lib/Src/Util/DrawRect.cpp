#include "pch.h"
#include <Console.h>

void Console::rectangle(int left, int top, int right, int bottom, FrameType type, WORD color) {
  const FrameChars &f = FrameChars::getFrameChars(type);
  String tmp = spaceString(right-left-1,f.m_horz);
  printf(left  , top   , color, _T("%c"), f.m_ul );
  printf(left+1, top   , color, _T("%s"), tmp.cstr());
  printf(right , top   , color, _T("%c"), f.m_ur );

  for(int y = top+1; y < bottom; y++) {
    printf(left , y, color, _T("%c"), f.m_vert);
    printf(right, y, color, _T("%c"), f.m_vert);
  }
  printf(left  , bottom, color, _T("%c"), f.m_ll );
  printf(left+1, bottom, color, _T("%s"), tmp.cstr());
  printf(right , bottom, color, _T("%c"), f.m_lr );
}
