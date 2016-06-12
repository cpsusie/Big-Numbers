#include "pch.h"
#include <Console.h>

DialogField::DialogField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label) 
: DialogControl(dlg,id,left,top,width,height)
{
  m_label     = label;
  m_cursorPos = 0;
  unSelect();
}

void DialogField::draw() {
  Dialog &dlg = getDialog();
  if(!dlg.isVisible()) {
    return;
  }

  dlg.getWin()->printf(getLeft(), getTop(), dlg.getColor(), _T("%s"), m_label.cstr());

  int fx = getFieldPos();
  int fy = getTop();
  dlg.getWin()->printf(fx, fy, getColor(), _T("%-*.*s"), getWidth(), getWidth(), m_str.cstr());
  if(isSelected() && m_selStart < (int)m_str.length()) {
    int e  = min(m_selEnd,(int)m_str.length());
    int sl = e - m_selStart;
    dlg.getWin()->printf(fx + m_selStart, fy, getDialog().getSelectedTextColor(), _T("%-*.*s"), sl, sl, m_str.cstr() + m_selStart);
  }
  if(isSelected())
    setCursorPos();
}

void DialogField::setSel(int start, int end) {
  if(start < 0) start = 0;
  if(start >= (int)m_str.length()) {
    start = (int)m_str.length() - 1;
  }
  if(end < 0  ) end   = -1;
  if(end > (int)m_str.length()) {
    end   = (int)m_str.length();
  }

  m_selStart = start;
  m_selEnd   = end;
  if(getDialog().isVisible()) draw();
}

void DialogField::getSel(int &start, int &end) {
  start = m_selStart;
  end   = m_selEnd;
}

String DialogField::getSelText() {
  if(isSelected()) {
    return substr(m_str, m_selStart, m_selEnd - m_selStart);
  } else {
    return _T("");
  }
}

void DialogField::setCursorPos() {
  Console::setCursorPos(getDialog().getLeft() + getLeft() + (int)m_label.length() + m_cursorPos, getDialog().getTop() + getTop());
}

void DialogField::setCursorPos(int x) {
  if(x >= 0 && x <= getWidth()) {
    m_cursorPos = x;
    if(getDialog().isVisible()) {
      setCursorPos();
    }
  }
}

void DialogField::deleteChar(int pos) {
  m_str.remove(pos);
  if(getDialog().isVisible()) {
    draw();
  }
}

void DialogField::deleteChar() {
  deleteChar(m_cursorPos);
}

void DialogField::deleteSelected() {
  m_str.remove(m_selStart,m_selEnd - m_selStart);
  if(getDialog().isVisible()) {
    draw();
  }
}

bool DialogField::insertChar(TCHAR ch) {
  if((int)m_str.length() < getWidth()) {
    m_str.insert(m_cursorPos,ch);
    if(getDialog().isVisible()) {
      draw();
    }
    return true;
  } else {
    return false;
  }
}

void DialogField::charRight() {
  if(getCursorPos() < (int)m_str.length()) {
    setCursorPos(getCursorPos()+1);
  }
}

void DialogField::setBuffer(const TCHAR *str) {
  m_str = ::left(str,getWidth());
  if(getDialog().isVisible()) {
    draw();
  }
}

void DialogField::setLabel(const String &label) {
  m_label = label;
  if(getDialog().isVisible()) {
    draw();
  }
}

void DialogField::substSelected(TCHAR ch) {
  int start, end;
  deleteSelected();
  getSel(start,end);
  setCursorPos(start);
  unSelect();
  insertChar(ch);
  charRight();
}

void DialogField::copyToClipboard() {
  if(isSelected()) {
    String s = getSelText();

    if(!OpenClipboard(NULL)) return;
    if(EmptyClipboard()) {
      HLOCAL buf = LocalAlloc(0,s.length() + 1);
      memcpy(buf,s.cstr(),s.length());
      HANDLE v = SetClipboardData(CF_TEXT,buf);
    }
    CloseClipboard();
  }
}

void DialogField::copyFromClipboard() {
  OpenClipboard(NULL);
  HANDLE t = GetClipboardData(CF_TEXT);
  CloseClipboard();
  String cb((TCHAR*)t);

  if(isSelected()) {
    String s = getBuffer();
    s.remove(m_selStart,m_selEnd - m_selStart + 1);
    s.insert(m_selStart,cb);
    setBuffer(s.cstr());
    setCursorPos(m_cursorPos + (int)cb.length());
  } else {
    String s = getBuffer();
    s.insert(getCursorPos(),cb);
    setBuffer(s.cstr());
    setCursorPos(m_cursorPos + (int)cb.length());
  }
  if(getDialog().isVisible()) {
    draw();
  }
}

bool DialogField::navigate(int event) {
  String oldstr = getBuffer();
  bool   ret    = false;

  if(EVENTUP(event)) {
    return false;
  }
  switch(EVENTSCAN(event)) {
  case SCAN_LEFT     :
    if(EVENTSTATE(event) & SHIFT_PRESSED) {
      if(isSelected()) {
        if(getCursorPos() == m_selStart) {
          charLeft();
          setSel(getCursorPos(), m_selEnd);
        } else {
          charLeft();
          setSel(m_selStart, getCursorPos());
        }
      } else {
        m_selEnd = getCursorPos();
        charLeft();
        setSel(getCursorPos(), m_selEnd);
      }
    } else {
      unSelect();
      charLeft();
    }
    ret = true;
    break;

  case SCAN_RIGHT    :
    if(EVENTSTATE(event) & SHIFT_PRESSED) {
      if(isSelected()) {
        if(getCursorPos() == m_selEnd) {
          charRight();
          setSel(m_selStart, getCursorPos());
        } else {
          charRight();
          setSel(getCursorPos(), m_selEnd);
        }
      } else {
        m_selStart = getCursorPos();
        charRight();
        setSel(m_selStart, getCursorPos());
      }
    } else {
      unSelect();
      charRight();
    }
    ret = true;
    break;

  case SCAN_HOME     :
    if(EVENTSTATE(event) & SHIFT_PRESSED) {
      if(isSelected()) {
        home();
        setSel(getCursorPos(), m_selEnd);
      } else {
        m_selEnd = getCursorPos();
        home();
        setSel(getCursorPos(), m_selEnd);
      }
    } else {
      unSelect();
      home();
    }
    ret = true;
    break;

  case SCAN_END      :
    if(EVENTSTATE(event) & SHIFT_PRESSED) {
      if(isSelected()) {
        end();
        setSel(m_selStart,getCursorPos());
      } else {
        m_selStart = getCursorPos();
        end();
        setSel(m_selStart, getCursorPos());
      }
    } else {
      unSelect();
      end();
    }
    ret = true;
    break;

  case SCAN_DELETE   :
    if(isSelected()) {
      deleteSelected();
      setCursorPos(m_selStart);
      unSelect();
    } else {
      deleteChar();
    }
    ret = true; 
    break;

  case SCAN_BACKSPACE:
    if(isSelected()) {
      deleteSelected();
      setCursorPos(m_selStart);
      unSelect();
    } else {
      deleteChar(m_cursorPos-1);
      charLeft();
    }
    ret = true; 
    break;

  default            :
    switch(EVENTASCII(event)) {
    case 3: // ctrl + c
      if(EVENTSTATE(event) & CTRL_PRESSED) {
        copyToClipboard();
        ret = true; 
      }
      break;
    case 22: // ctrl + v
      if(EVENTSTATE(event) & CTRL_PRESSED) {
        copyFromClipboard();
        ret = true; 
      }
      break;
    case 24: // ctrl + x
      if(EVENTSTATE(event) & CTRL_PRESSED && isSelected()) {
        copyToClipboard();
        deleteSelected();
        setCursorPos(m_selStart);
        unSelect();
        ret = true;
      }
      break;
    default:
      return false;
    }
  }
  if(ret && m_str != oldstr) {
    postChangeEvent();
  }
  return ret;
}

void DialogField::postChangeEvent() {
  getDialog().postEvent(MSG_CHANGE);
}
