#include "pch.h"
#include <Console.h>

ScrollBar::ScrollBar(Dialog *dlg, int type, int id, int left, int top, int width, int height)
: DialogControl(dlg,id,left,top,width,height)
{
  m_type      = type;
  m_min       = 0;
  m_max       = 100;
  m_pos       = 0;
  m_pageSize  = 1;
  m_color     = NORMALVIDEO;
  m_barColor  = NORMALVIDEO;
}

void ScrollBar::setRange(int Min, int Max, int pageSize) {
  if(Min <= Max) {
    m_min = Min;
    m_max = Max;
    m_pageSize = pageSize;
    if(m_pageSize < 1) {
      m_pageSize = 1;
    }
    if(m_pos < m_min) {
      m_pos = m_min;
      getDialog().postEvent((m_type==SCROLLBAR_VERT)?MSG_VSCROLL:MSG_HSCROLL);
    } else if(m_pos > m_max) {
      m_pos = m_max;
      getDialog().postEvent((m_type==SCROLLBAR_VERT)?MSG_VSCROLL:MSG_HSCROLL);
    }
    if(getDialog().isVisible()) {
      draw();
    }
  }
}

void ScrollBar::getRange(int &Min, int &Max, int &pageSize) {
  Min = m_min;
  Max = m_max;
  pageSize = m_pageSize;
}

void ScrollBar::setPos(int pos) {
  if(pos < m_min) {
    m_pos = m_min;
  } else if(pos > m_max) {
    m_pos = m_max;
  } else {
    m_pos = pos;
  }
  getDialog().postEvent((m_type==SCROLLBAR_VERT)?MSG_VSCROLL:MSG_HSCROLL);
  if(getDialog().isVisible()) {
    draw();
  }
}

void ScrollBar::setColor(WORD color , WORD barColor) {
  m_color    = color;
  m_barColor = barColor;
  if(getDialog().isVisible()) {
    draw();
  }
}

void ScrollBar::getColor(WORD &color, WORD &barColor) {
  color    = m_color;
  barColor = m_barColor;
}

static void findBarSize(int size, int pageSize, int itemCount, int maxPos, int pos, int &barPos, int &barSize) {
  if(itemCount == 0) {
    barSize = size - 2;
  } else {
    double es = (double)pageSize / itemCount * (size - 2);
    if(es < 1) {
      barSize = 1;
    } else {
      if(es > size - 2) {
        barSize = size - 2;
      } else {
        barSize = (int)es;
      }
    }
  }
  barPos = (int)(((double)pos / maxPos) * (size - 2 - barSize) + 0.5) + 1;
}

#define UPARROW    30
#define DOWNARROW  31
#define LEFTARROW  17
#define RIGHTARROW 16

#define SCROLLBAR_BACKGROUND 177
#define SCROLLBAR_BAR        219

void ScrollBar::drawvScrollBar(TextWin *tw, int left, int top, int height, int pageSize, int itemCount, int maxPos, int pos, WORD color, WORD barColor, FrameType borderType) {
  int i;
  for(i = 1; i < height-1; i++) {
    tw->printf(left, top + i, color, _T("%c"), SCROLLBAR_BACKGROUND);
  }
  const FrameChars &fc = FrameChars::getFrameChars(borderType);
  tw->printf(left, top, color, _T("%c"), pos > 0 ? UPARROW : fc.m_ur);
  tw->printf(left, top + height - 1, color, _T("%c"), pos < maxPos ? DOWNARROW : fc.m_lr);
  int barPos, barSize;
  findBarSize(height, pageSize, itemCount, maxPos, pos, barPos, barSize);
  for(i = 0; i < barSize; i++) {
    tw->printf(left, top + barPos + i, barColor, _T("%c"), SCROLLBAR_BAR);
  }
}

void ScrollBar::drawhScrollBar(TextWin *tw, int left, int top, int width, int pageSize, int itemCount, int maxPos, int pos, WORD color, WORD barColor, FrameType borderType) {
  int i;
  for(i = 1; i < width-1; i++) {
    tw->printf(left + i, top, color, _T("%c"), SCROLLBAR_BACKGROUND);
  }
  const FrameChars &fc = FrameChars::getFrameChars(borderType);
  tw->printf(left, top, color, _T("%c"), pos > 0 ? LEFTARROW : fc.m_ll);
  tw->printf(left + width - 1,top , color, _T("%c"), pos < maxPos ? RIGHTARROW : fc.m_lr);
  int barPos, barSize;
  findBarSize(width, pageSize, itemCount, maxPos, pos, barPos, barSize);
  for(i = 0; i < barSize; i++) {
    tw->printf(left + barPos + i, top, barColor, _T("%c"), SCROLLBAR_BAR);
  }
}

void ScrollBar::draw() {
  TextWin *tw = getDialog().getWin();
  
  switch(m_type) {
  case SCROLLBAR_VERT:
    drawvScrollBar(tw,getLeft(), getTop(), getHeight(), m_pageSize, m_max - m_min + 1, m_max - m_min, m_pos - m_min, m_color, m_barColor);
    break;
  case SCROLLBAR_HORZ:
    drawhScrollBar(tw,getLeft(), getTop(), getWidth() , m_pageSize, m_max - m_min + 1, m_max - m_min, m_pos - m_min, m_color, m_barColor);
    break;
  }
}

void ScrollBar::enterDialog() {
}

void ScrollBar::gotFocus() {
  Console::showCursor(false);
}

void ScrollBar::killFocus() {
  Console::showCursor(true);
}

bool ScrollBar::key(int event) {
  if(!EVENTDOWN(event)) {
    return false;
  }
  switch(m_type) {
  case SCROLLBAR_VERT:
    switch(EVENTSCAN(event)) {
    case SCAN_UP    :
      setPos(m_pos-1);
      return true;
    case SCAN_DOWN  :
      setPos(m_pos+1);
      return true;
    case SCAN_HOME  :
      setPos(m_min);
      return true;
    case SCAN_END   :
      setPos(m_max);
      return true;
    case SCAN_PGDOWN:
      setPos(m_pos + m_pageSize);
      return true;
    case SCAN_PGUP  :
      setPos(m_pos - m_pageSize);
      return true;
    }
    break;;

  case SCROLLBAR_HORZ:
    switch(EVENTSCAN(event)) {
    case SCAN_LEFT  :
      setPos(m_pos-1);
      return true;
    case SCAN_RIGHT :
      setPos(m_pos+1);
      return true;
    case SCAN_HOME  :
      setPos(m_min);
      return true;
    case SCAN_END   :
      setPos(m_max);
      return true;
    case SCAN_PGDOWN:
      setPos(m_pos + m_pageSize);
      return true;
    case SCAN_PGUP  :
      setPos(m_pos - m_pageSize);
      return true;
    }
    break;
  }
  if(getDialog().interField(event))  {
    return true;
  }

  return false;
}

void ScrollBar::exitDialog() {
}
