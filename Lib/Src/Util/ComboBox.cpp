#include "pch.h"
#include <Console.h>

ComboBox::ComboBox(Dialog *dlg, int id, int left, int top, int width, int height, const String &label) : 
DialogField(dlg,id,left,top,width,height,label)
{
  m_tw = NULL;
  m_dropDownHeight = 7;
}

ComboBox::~ComboBox() {
  hideDropDown();
}

void ComboBox::dataToField() {
  setBuffer(getString().cstr());
}

void ComboBox::fieldToData() {
  getString() = getBuffer();
}

void ComboBox::enterDialog() {
  dataToField();
  setCursorPos(0);
  m_currentSel   = 0;
  m_scrollOffset = 0;
}

void ComboBox::gotFocus( ) {
  enterDialog();
  setCursorPos();
}

bool ComboBox::key(int event) {
  if(EVENTUP(event) || (EVENTSTATE(event) & ALT_PRESSED)) return false;

  if(EVENTSTATE(event) & CTRL_PRESSED) {
    switch(EVENTSCAN(event)) {
    case SCAN_HOME    :
      if(isDropDownVisible()) {
        setScrollOffset(0);
        setSel(0);
        return true;
      }
      break;
    case SCAN_END     :
      if(isDropDownVisible()) {
        setScrollOffset(getMaxScrollOffset());
        setSel((int)m_list.size()-1);
        return true;
      }
      break;
    default:
      break;
    }
  }
  else
    if(EVENTTYPE(event) == EVENTTYPE_KEY) {
      switch(EVENTSCAN(event)) {
      case SCAN_DOWN  :
        if(m_currentSel >= m_scrollOffset + getNoOfVisibleItems() - 1)
          setScrollOffset(m_scrollOffset + 1);
        setSel(m_currentSel + 1);
        return true;
    
      case SCAN_UP    :
        if(m_currentSel == m_scrollOffset)
          setScrollOffset(m_scrollOffset - 1);
        setSel(m_currentSel - 1);
        return true;

      case SCAN_PGDOWN:
        setScrollOffset(m_scrollOffset + getNoOfVisibleItems());
        setSel(m_currentSel + getNoOfVisibleItems());
        return true;

      case SCAN_PGUP  :
        setScrollOffset(m_scrollOffset - getNoOfVisibleItems());
        setSel(m_currentSel - getNoOfVisibleItems());
        return true;

      case SCAN_ESCAPE:
        if(isDropDownVisible()) {
          hideDropDown();
          return true;
        }
        else
          return false;

      case SCAN_ENTER :
        if(isDropDownVisible()) {
          setBuffer(m_list[m_currentSel].cstr());
          hideDropDown();
          return true;
        }
        else
          return false;

      case SCAN_F4    :
        if(isDropDownVisible())
          hideDropDown();
        else
          beginDropDown();
        break;
      }
    }
  if(navigate(event)) return true;
  if(getDialog().interField(event)) return true;
  String oldstr = getBuffer();
  unsigned char ch = EVENTASCII(event);
  if(ch >= ' ') {
    if(isSelected())
      substSelected(ch);
    else {
      if(insertChar(ch))
        charRight();
    }
    if(getBuffer() != oldstr)
      postChangeEvent();
    return true;
  }
  return false;
}

void ComboBox::exitDialog() {
  fieldToData();
}

void ComboBox::killFocus() {
  hideDropDown();
  exitDialog();
}

void ComboBox::addString(const String &str) {
  m_list.add(str);
}

void ComboBox::draw() {
  DialogField::draw();
  Dialog &dlg = getDialog();
  int x = getFieldPos() + getWidth();
  int y = getTop();
//  WORD attr = consoleGetColor(x,y);
//  attr = BACKGROUNDCOLOR(attr) | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  WORD attr = BACKGROUND_BLUE | BACKGROUND_GREEN | NORMALVIDEO | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
  dlg.getWin()->printf( x, y, attr, _T("%c"), 31 );
}

int ComboBox::getRealDropDownHeight() {
  if((int)m_list.size() < m_dropDownHeight) {
    return (int)m_list.size() + 1;
  } else {
    return m_dropDownHeight;
  }
}

int ComboBox::getDropDownHeight() const {
  return m_dropDownHeight;
}

void ComboBox::setDropDownHeight(int newvalue) {
  if(newvalue >= 2 && newvalue != m_dropDownHeight) {
    m_dropDownHeight = newvalue;
    if(isDropDownVisible()) {
      hideDropDown();
      beginDropDown();
    }
  }
}

void ComboBox::beginDropDown() {
  int w = getWidth() - 2;
  for(size_t i = 0; i < m_list.size(); i++) {
    int l = (int)m_list[i].length();
    if(l > w) w = l;
  }

  m_dropDownWidth  = w + 2;

  Dialog &dlg = getDialog();
  int dl = dlg.getLeft() + getFieldPos();
  int dt = dlg.getTop() + getTop() + 1;
  m_tw = new TextWin( dl, dt, m_dropDownWidth, getRealDropDownHeight(),TW_BEFORE,dlg.getWin());
  drawDropDown();
}

bool ComboBox::isItemVisible(int i) {
  return (i >= m_scrollOffset && i < m_scrollOffset + getNoOfVisibleItems());
}

void ComboBox::drawItem(int i, bool selected) {
  if(!getDialog().isVisible() || !isItemVisible(i))
    return;

  WORD attr;
  if(selected) {
    attr = NORMALVIDEO | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN;
  } else {
    attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  }

  const TCHAR *s = (i >= (int)m_list.size()) ? _T("") : m_list[i].cstr();

  int dl = 1;
  int dt = 1;

  m_tw->printf(dl, dt + i - m_scrollOffset, attr, _T("%-*.*s"), m_dropDownWidth-2,m_dropDownWidth-2, s);
}

void ComboBox::drawItem(int i) {
  drawItem(i,i == m_currentSel);
}

void ComboBox::setSel(int i) {
  if (i < 0) {
    i = 0;
  } else if (i >= (int)m_list.size()) {
    i = (int)m_list.size() - 1;
  }
  if(i != m_currentSel) {
    if (!getDialog().isVisible()) {
      m_currentSel = i;
    } else {
      if(!isDropDownVisible()) {
        m_currentSel = i;
        setBuffer(m_list[i].cstr());
      } else {
        if (m_currentSel >= 0) {
          drawItem(m_currentSel, false);
        }
        m_currentSel = i;
        drawItem(m_currentSel,true);
        setBuffer(m_list[i].cstr());
      }
      getDialog().postEvent(MSG_SELCHANGE);
    }
  }
}

int ComboBox::getMaxScrollOffset() {
  int m = (int)m_list.size() - getNoOfVisibleItems();
  return m < 0 ? 0 : m;
}

void ComboBox::setScrollOffset(int i) {
  int m = getMaxScrollOffset();
  if (i > m) {
    m_scrollOffset = m;
  } else if (i < 0) {
    m_scrollOffset = 0;
  } else {
    m_scrollOffset = i;
  }
  if (isDropDownVisible()) {
    drawDropDown();
  }
}


void ComboBox::drawScrollBar() {
  int left     = m_dropDownWidth - 1;
  int top      = 0;
  WORD attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  ScrollBar::drawvScrollBar( m_tw, left, top, getRealDropDownHeight(), getNoOfVisibleItems(), (int)m_list.size(), getMaxScrollOffset(), m_scrollOffset, attr, attr, SINGLE_FRAME);
}

void ComboBox::drawBorder() {
  WORD attr = REVERSEVIDEO | BACKGROUND_INTENSITY;
  m_tw->rectangle(0, 0, m_dropDownWidth - 1, getRealDropDownHeight() - 1,SINGLE_FRAME,attr);
  if (getMaxScrollOffset() > 0) {
    drawScrollBar();
  }
}

void ComboBox::drawDropDown() {
  drawBorder();
  int n = getNoOfVisibleItems();
  for (int i = m_scrollOffset; i < m_scrollOffset + n; i++) {
    drawItem(i);
  }
}

void ComboBox::hideDropDown() {
  if(m_tw) {
    delete m_tw;
    m_tw = NULL;
  }
}
