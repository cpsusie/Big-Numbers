#include "pch.h"
#include <Console.h>

ConsoleWindow::ConsoleWindow() {
  m_parent  = NULL;
  m_tw      = NULL;
}

ConsoleWindow::~ConsoleWindow() {
  destroy();
}

void ConsoleWindow::drawShadow() {
/*
  int scrw,scrh;
  screensize(scrw,scrh);
  const SMALL_RECT &r = m_ca->getRect();
  if(r.Right < scrw) {
    int bottom = r.Bottom;
    if(bottom >= scrh) bottom = scrh - 1;
    for(int row = r.Top + 1; row <= bottom; row++) {
      WORD attr = consoleGetColor(r.Right,row);
      attr = (FOREGROUNDCOLOR(attr) & ~FOREGROUND_INTENSITY);
      consoleSetColor(r.Right,row,attr);
    }
  }
  if(r.Bottom < scrh) {
    int right = r.Right;
    if(right >= scrw) right = scrw - 1;
    for(int col = r.Left + 1; col < right; col++) {
      WORD attr = consoleGetColor(col,r.Bottom);
      attr = (FOREGROUNDCOLOR(attr) & ~FOREGROUND_INTENSITY);
      consoleSetColor(col,r.Bottom,attr);
    }
  }
*/
}

void ConsoleWindow::hideShadow() {
/*
  int scrw, scrh;
  screensize(scrw,scrh);
  const SMALL_RECT &r = m_ca->getRect();
  if(r.Right < scrw) {
    int bottom = r.Bottom;
    if(bottom >= scrh) bottom = scrh - 1;
    for(int row = r.Top + 1; row <= bottom; row++)
      consoleSetColor(r.Right,row,m_ca->getattr(r.Right,row));
  }
  if(r.Bottom < scrh) {
    int right = r.Right;
    if(right >= scrw) right = scrw - 1;
    for(int col = r.Left + 1; col < right; col++)
      consoleSetColor(col,r.Bottom,m_ca->getattr(col,r.Bottom));
  }
*/
}

void ConsoleWindow::draw() {
  if(m_shadow)
    drawShadow();
}

void ConsoleWindow::setShadow(bool on) {
  if(on != m_shadow)
    if(on) 
      drawShadow();
    else
      hideShadow();
  m_shadow = on;
}

void ConsoleWindow::create(bool shadowed, int left, int top, int right, int bottom, ConsoleWindow *parent) {
  if(m_tw != NULL)
    throwException(_T("window already created"));
  m_parent  = parent;
  if(m_parent)
    m_parent->m_children.add(this);
  m_shadow = shadowed;
  if(shadowed) {
    right++;
    bottom++;
  }
  if(m_tw == NULL) {
    m_tw = new TextWin(left, top, right - left + 1, bottom - top + 1);
  } else {
    SMALL_RECT r;
    r.Left   = left;
    r.Right  = right;
    r.Top    = top;
    r.Bottom = bottom;
    m_tw->setRect(r);
  }
}

void ConsoleWindow::destroy() {
  if(m_parent) {
    for(int i = 0; i < m_parent->m_children.size(); i++) {
      if(m_parent->m_children[i] == this) {
        m_parent->m_children.removeIndex(i);
        break;
      }
    }
    m_parent = NULL;
  }
  if(m_tw) {
    delete m_tw;
    m_tw     = NULL;
  }
}

int ConsoleWindow::getEvent() {
  if(!m_eventQueue.isEmpty()) {
    return m_eventQueue.get();
  } else {
    return Console::getKeyEvent();
  }
}

void ConsoleWindow::postEvent(int event) {
  m_eventQueue.put(event);
}

bool ConsoleWindow::translateEvent(int event) {
  for(int i = 0; i < m_accel.size(); i++) {
    if(EVENTDOWN(event) && m_accel[i].catchevent(event)) {
      postEvent(m_accel[i].m_command);
      return true;
    }
  }
  return false;
}

AcceleratorItem::AcceleratorItem(int key, int command) {
  m_keyevent = key;
  m_command  = command;
}

bool AcceleratorItem::catchevent(int event) {
  int eventstate = EVENTSTATE(event);
  int eventascii = EVENTASCII(event);

  if(EVENTSTATE(m_keyevent) && !(EVENTSTATE(m_keyevent) & eventstate))
    return false;
  if(EVENTSCAN( m_keyevent))
    return (EVENTSCAN(m_keyevent) == EVENTSCAN(event));

  if(eventstate & CTRL_PRESSED) eventascii += 0x40;
  if(tolower(EVENTASCII(m_keyevent)) == tolower(eventascii))
    return true;
  return false;
}

Accelerator::Accelerator(AcceleratorItem *items) {
  for(int i = 0; items[i].m_keyevent != 0; i++)
    add(items[i]);
}
