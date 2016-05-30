#include "pch.h"
#include <Console.h>

ProgressBar::ProgressBar(Dialog *dlg, int type, int id, int left, int top, int width, int height)
: DialogControl(dlg,id,left,top,width,height) {
  m_type     = type;
  m_max      = 100;
  m_pos      = 0;
  m_color    = NORMALVIDEO;
  m_barColor = NORMALVIDEO;
  setEnable(false);
}

bool ProgressBar::setMax(int value) {
  if(value <= 0)
    return false;
  if(value == m_max)
    return true;
  m_max = value;
  if(m_pos > m_max) {
    m_pos = m_max;
  }
  if(getDialog().isVisible()) {
    draw();
  }
  return true;
}

void ProgressBar::setPos(int value) {
  if(value < 0)
    value = 0;
  if(value > m_max)
    value = m_max;
  if(value != m_pos) {
    m_pos = value;
    if(getDialog().isVisible()) {
      draw();
    }
  }
}

void ProgressBar::setColor(WORD color , WORD barcolor) {
  m_color    = color;
  m_barColor = barcolor;
  if(getDialog().isVisible())
    draw();
}

void ProgressBar::getColor(WORD &color, WORD &barcolor) {
  color    = m_color;
  barcolor = m_barColor;
}

#define PROGRESSBAR_BACKGROUND 177
#define PROGRESSBAR_BAR        219

void ProgressBar::drawvProgressBar(TextWin *tw, int left, int top, int height, int maxpos, int pos, WORD color, WORD barcolor) {
  int i;
  for(i = 0; i < height; i++) {
    tw->printf(left, top + i, color, _T("%c"), PROGRESSBAR_BACKGROUND);
  }
  const int es = pos * height / maxpos;
  for(i = 0; i < es; i++) {
    tw->printf(left, top + i, barcolor, _T("%c"), PROGRESSBAR_BAR);
  }
}

void ProgressBar::drawhProgressBar(TextWin *tw, int left, int top, int width, int maxpos, int pos, WORD color, WORD barcolor) {
  int i;
  for(i = 0; i < width; i++) {
    tw->printf(left + i, top, color, _T("%c"), PROGRESSBAR_BACKGROUND);
  }
  const int es = pos * width / maxpos;
  for(i = 0; i < es; i++) {
    tw->printf(left + i, top, barcolor, _T("%c"), PROGRESSBAR_BAR);
  }
}

void ProgressBar::draw() {
  if(!getDialog().isVisible())
    return;
  switch(m_type) {
  case PROGRESSBAR_VERT:
    drawvProgressBar(getDialog().getWin(), getLeft(), getTop(), getHeight(), m_max, m_pos, m_color, m_barColor);
    break;
  case PROGRESSBAR_HORZ:
    drawhProgressBar(getDialog().getWin(), getLeft(), getTop(), getWidth() , m_max, m_pos, m_color, m_barColor);
    break;
  }
}
