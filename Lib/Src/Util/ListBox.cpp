#include "pch.h"
#include <Console.h>

ListBox::ListBox(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
: DialogControl(dlg,id,left,top,width,height) {
  m_currentSel   = 0;
  m_scrollOffset = 0;
}

ListBox::~ListBox() {
}

bool ListBox::isItemVisible(int i) const {
  return (i >= m_scrollOffset && i < m_scrollOffset + getVisibleItemCount());
}

void ListBox::drawItem(int i, bool selected) {
  if(!getDialog().isVisible() || !isItemVisible(i))
    return;

  TextWin *tw = getDialog().getWin();
  WORD attr;
  if(selected) {
    attr = NORMALVIDEO | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN;
  } else {
    attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  }

  TCHAR *s = (i >= (int)m_strings.size()) ? _T("") : m_strings[i].cstr();

  tw->printf(getLeft() + 1, getTop() + i + 1 - m_scrollOffset, attr, _T("%-*.*s"), getWidth()-2,getWidth()-2,s);
}

void ListBox::drawItem(int i) {
  drawItem(i,i == m_currentSel);
}

void ListBox::drawScrollBar() {
  TextWin *tw = getDialog().getWin();

  int Left = getLeft() + getWidth() - 1;
  int Top  = getTop();
  int attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  ScrollBar::drawvScrollBar(tw,Left,Top,getHeight(), getVisibleItemCount(), m_strings.size(), getMaxScrollOffset(), m_scrollOffset, attr, attr, SINGLE_FRAME);
}

void ListBox::drawBorder() {
  TextWin *tw = getDialog().getWin();
  WORD attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  tw->rectangle(getLeft(), getTop(), getLeft() + getWidth() - 1, getTop() + getHeight() - 1, SINGLE_FRAME, attr);
  if(getMaxScrollOffset() > 0) {
    drawScrollBar();
  }
}

void ListBox::setSel(int i) {
  if(i >= (int)m_strings.size()) {
    i = m_strings.size() - 1;
  }
  if(i < 0) {
    i = 0;
  }
  if(i != m_currentSel) {
    if(!getDialog().isVisible()) {
      m_currentSel = i;
    } else {
      if(m_currentSel >= 0) {
        drawItem(m_currentSel,false);
      }
      m_currentSel = i;
      drawItem(m_currentSel,true);
    }
    getDialog().postEvent(MSG_SELCHANGE);
  }
}

int ListBox::getMaxScrollOffset() {
  const int m = m_strings.size() - getVisibleItemCount();
  return m < 0 ? 0 : m;
}

void ListBox::setScrollOffset(int i) {
  const int m = getMaxScrollOffset();
  if(i > m) {
    m_scrollOffset = m;
  } else if(i < 0) {
    m_scrollOffset = 0;
  } else {
    m_scrollOffset = i;
  }
  draw();
}

void ListBox::addString(const String &str) {
  m_strings.add(str);
  if(getDialog().isVisible()) {
    draw();
  }
}

void ListBox::insertString(int i, const String &str) {
  if(i < 0 || i > m_strings.size()) {
    return;
  }
  m_strings.add(i,str);
  if(getDialog().isVisible()) {
    draw();
  }
}

void ListBox::removeString(int i) {
  if(i < 0 || i >= m_strings.size()) {
    return;
  }
  m_strings.removeIndex(i);
  if(m_currentSel >= m_strings.size()) {
    m_currentSel = m_strings.size() - 1;
  }
  if(m_scrollOffset > getMaxScrollOffset()) {
    m_scrollOffset = getMaxScrollOffset();
  }
  if(getDialog().isVisible()) {
    draw();
  }
}

void ListBox::draw() {
  if(!getDialog().isVisible()) {
    return;
  }
  drawBorder();
  const int n = getVisibleItemCount();
  for(int i = m_scrollOffset; i < m_scrollOffset + n; i++) {
    drawItem(i);
  }
}

void ListBox::enterDialog( ) {
  m_currentSel   = 0;
  m_scrollOffset = 0;
}

void ListBox::gotFocus(  ) {
  Console::showCursor(false);
}

void ListBox::killFocus(   ) {
  Console::showCursor(true);
}

bool ListBox::key(int event) {
  if(EVENTUP(event)) {
    return false;
  }
  switch(EVENTSCAN(event)) {
  case SCAN_DOWN  :
    if(m_currentSel >= m_scrollOffset + getVisibleItemCount() - 1) {
      setScrollOffset(m_scrollOffset + 1);
    }
    setSel(m_currentSel + 1);
    return true;
    
  case SCAN_UP    :
    if(m_currentSel == m_scrollOffset) {
      setScrollOffset(m_scrollOffset - 1);
    }
    setSel(m_currentSel - 1);
    return true;

  case SCAN_END   :
    setScrollOffset(getMaxScrollOffset());
    setSel(m_strings.size() - 1);
    return true;

  case SCAN_HOME  :
    setScrollOffset(0);
    setSel(0);
    break;

  case SCAN_PGDOWN:
    setScrollOffset(m_scrollOffset + getVisibleItemCount());
    setSel(m_currentSel + getVisibleItemCount());
    return true;

  case SCAN_PGUP  :
    setScrollOffset(m_scrollOffset - getVisibleItemCount());
    setSel(m_currentSel - getVisibleItemCount());
    return true;

  default:
    if(getDialog().interField(event)) {
      return true;
    }
  }

  return false;
}

void ListBox::exitDialog() {
}
