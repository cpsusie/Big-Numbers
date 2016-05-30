#include "pch.h"
#include <Console.h>

DialogString::DialogString(int left, int top, const String &str) {
  m_left      = left;
  m_top       = top;
  m_str       = str;
}

DialogControl::DialogControl(Dialog *dlg, int id, int left, int top, int width, int height ) : m_dlg(*dlg) {
  m_id        = id;
  m_left      = left;
  m_top       = top;
  m_width     = width;
  m_height    = height;
  m_enabled   = true;
  m_visible   = true;
  m_tabIndex  = -1;
}

void DialogControl::setTabIndex(int tabIndex) {
  if(tabIndex == m_tabIndex)
    return;
  if(tabIndex < 0)
    return;
  if(m_tabIndex != -1) {
    int n = m_dlg.m_controls.size();
    if(m_tabIndex < tabIndex) {
      for(int i = 0; i < n; i++) {
        DialogControl *c = m_dlg.m_controls[i];
        if(c == this) continue;
        if(m_tabIndex <= c->m_tabIndex && c->m_tabIndex <= tabIndex)
          c->m_tabIndex--;
      }
    }
    else {
      for(int i = 0; i < n; i++) {
        DialogControl *c = m_dlg.m_controls[i];
        if(c == this) continue;
        if(tabIndex <= c->m_tabIndex && c->m_tabIndex <= m_tabIndex)
          c->m_tabIndex++;
      }
    }
    m_tabIndex = tabIndex;
  }
  else {
    int n = m_dlg.m_controls.size();
    for(int i = 0; i < n; i++) {
      DialogControl *c = m_dlg.m_controls[i];
      if(c->m_tabIndex >= tabIndex)
        c->m_tabIndex++;
    }
    m_tabIndex = tabIndex;
  }
}

void DialogControl::setEnable(bool v) {
  m_enabled = v;
  if(!m_enabled && m_dlg.getCurrentControl() == this)
    m_dlg.nextField();
  else
    draw();
}

void DialogControl::setVisible(bool v) {
}

WORD DialogControl::getColor() {
  WORD color;
  if(getDialog().getCurrentControl() == this)
    color = getDialog().getSelectedFieldColor();
  else
    color = getDialog().getNormalFieldColor();
  if(!isEnabled())
    color &= ~(FOREGROUND_INTENSITY | BACKGROUND_INTENSITY);
  return color;
}

Dialog::Dialog(const String &title, int left, int top, int width, int height) {
  m_title         =  title;
  m_left          =  left;
  m_top           =  top;
  m_width         =  width;
  m_height        =  height;
  m_currentField  = -1;
  m_menu          = NULL;
  m_borderType    = SINGLE_FRAME;
  m_borderColor   = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  m_color         = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  m_selFieldColor = REVERSEVIDEO | BACKGROUND_INTENSITY;
  m_selTextColor  = NORMALVIDEO  | FOREGROUND_INTENSITY;
  m_fieldColor    = NORMALVIDEO  | FOREGROUND_INTENSITY;
}

Dialog::~Dialog() {
  for(int i = 0; i < m_controls.size(); i++)
    delete m_controls[i];
}

void Dialog::onOk() {
  quit(IDOK);
}

void Dialog::onCancel() {
  quit(IDCANCEL);
}

bool Dialog::eventHandler(int event) {
  if(EVENTUP(event))
    return false;
  switch(EVENTSCAN(event)) {
  case SCAN_ENTER :
    onOk();
    return true;
  case SCAN_ESCAPE:
    onCancel();
    return true;
  }
  return false;
}

void Dialog::addIntField(     int id, int left, int top, int length, const String &label) {
  addControl(new IntField(    this, id, left, top, length, 1, label));
}

void Dialog::addShortField(   int id, int left, int top, int length, const String &label) {
  addControl(new ShortField(  this, id, left, top, length, 1, label));
}

void Dialog::addDoubleField(  int id, int left, int top, int length, const String &label) {
  addControl(new DoubleField( this, id, left, top, length, 1, label));
}

void Dialog::addStringField(  int id, int left, int top, int length, const String &label) {
  addControl(new StringField( this, id, left, top, length, 1, label));
}

void Dialog::addComboBox(     int id, int left, int top, int length, const String &label) {
  addControl(new ComboBox(    this, id, left, top, length, 1, label));
}

void Dialog::addBoolField(    int id, int left, int top, int length, const String &label) {
  addControl(new BoolField(   this, id, left, top, length, 1, label));
}

void Dialog::addButton(       int id, int left, int top, int width, int height, const String &label) {
  addControl(new DialogButton(this, id, left, top, width, height, label));
}

void Dialog::addListBox(      int id, int left, int top, int width, int height, const String &label) {
  addControl(new ListBox(     this, id, left, top, width, height, label));
}

int Dialog::findControlIndex(int id) {
  for(int i = 0; i < m_controls.size(); i++)
    if(m_controls[i]->getid() == id)
      return i;
  return -1;
}

void Dialog::checkIdUnique(int id) {
  for(int i = 0; i < m_controls.size(); i++)
    if(m_controls[i]->getid() == id)
      throwException(_T("field %d already exist"),id);
}

int Dialog::findNextTabIndex() {
  int v = -1;
  for(int i = 0; i < m_controls.size(); i++) {
    int t = m_controls[i]->getTabIndex();
    if(t > v) v = t;
  }
  return v + 1;
}

void Dialog::addControl(DialogControl *control) {
  checkIdUnique(control->getid());
  control->setTabIndex(findNextTabIndex());
  m_controls.add(control);
}

DialogControl *Dialog::getDlgItem(int id) {
  for(int i = 0; i < m_controls.size(); i++)
    if(m_controls[i]->getid() == id)
      return m_controls[i];
  throwException(_T("Control %d doesnt exist"),id);
  return m_controls[0];
}

void Dialog::setVar(int id, int n) {
  IntField *f = (IntField*)getDlgItem(id);
  f->getInt() = n;
  f->dataToField();
  if(isVisible())
    f->draw();
}

void Dialog::setVar(int id, short n) {
  ShortField *f = (ShortField*)getDlgItem(id);
  f->getShort() = n;
  f->dataToField();
  if(isVisible())
    f->draw();
}

void Dialog::setVar(int id, double n) {
  DoubleField *f = (DoubleField*)getDlgItem(id);
  f->getDouble() = n;
  f->dataToField();
  if(isVisible())
    f->draw();
}

void Dialog::setVar(int id, const String &s) { 
  StringField *f = (StringField*)getDlgItem(id);
  f->getString() = s;
  f->dataToField();
  if(isVisible())
    f->draw();
}

void Dialog::setVar(int id, bool n) {
  BoolField *f = (BoolField*)getDlgItem(id);
  f->getBool() = n;
  if(isVisible())
    f->draw();
}

void Dialog::getVar(int id, int    &n)     { n = ((IntField*)getDlgItem(id))->getInt();       }
void Dialog::getVar(int id, short  &n)     { n = ((ShortField*)getDlgItem(id))->getShort();   }
void Dialog::getVar(int id, double &n)     { n = ((DoubleField*)getDlgItem(id))->getDouble(); }
void Dialog::getVar(int id, String &s)     { s = ((StringField*)getDlgItem(id))->getString(); }
void Dialog::getVar(int id, bool   &n)     { n = ((BoolField*)getDlgItem(id))->getBool();     }

void Dialog::printf(int x, int y, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String tmp = vformat(format,argptr);
  va_end(argptr);
  m_strings.add(DialogString(x, y, tmp));
  if(isVisible()) {
    drawStrings();
  }
}

void Dialog::drawBorder() {
  getWin()->rectangle( 0, 0, m_width, m_height, m_borderType, m_borderColor);
  if(m_title.length() > 0) {
    int t = (m_width - m_title.length()) / 2;
    getWin()->printf(t, 0, m_borderColor, _T(" %s "), m_title.cstr());
  }
}

void Dialog::drawStrings() {
  for(int i = 0; i < m_strings.size(); i++)
    getWin()->printf(m_strings[i].m_left, m_strings[i].m_top, m_color, _T("%s"), m_strings[i].m_str.cstr());
}

void Dialog::drawControls() {
  for(int i = 0; i < m_controls.size(); i++) {
    m_controls[i]->draw();
  }
}

void Dialog::drawMenu() {
  if(m_menu) {
    m_menu->draw(m_left+1,m_top+1);
  }
}

void Dialog::draw() {
  int cx, cy;

  if(!isVisible()) {
    create(m_shadowed,m_left,m_top,m_left+m_width-1,m_top+m_height-1);
    ConsoleWindow::draw();
  }
  Console::getCursorPos(cx, cy);
  getWin()->set( 0, 0, m_width, m_height, TR_ALL, ' ', m_color);
  drawBorder();
  drawStrings();
  drawControls();
  drawMenu();
  Console::setCursorPos(cx,cy);
}

static int tabIndexCmp(DialogControl * const &c1, DialogControl * const &c2) {
  return c1->getTabIndex() - c2->getTabIndex();
}

int Dialog::domodal() {
  int i;
  int cx,cy;
  bool cursorWasVisible = Console::isCursorVisible();
  Console::getCursorPos(cx,cy);
  m_controls.sort(tabIndexCmp);

  for(i = 0; i < m_controls.size(); i++) {
    m_controls[i]->enterDialog();
  }

  draw();

  firstField();
  if(m_currentField >= 0) {
    for(;;) {
      int event = getEvent();
      if(m_controls[m_currentField]->key(event)) {
        continue;
      }
      if(translateEvent(event)) {
        continue;
      }
      if(m_menu && m_menu->wantEvent(event)) {
        m_controls[m_currentField]->killFocus();
        m_menu->handleEvent(event);
        m_controls[m_currentField]->gotFocus();
        continue;
      }
      eventHandler(event);
      if(event == MSG_QUIT) {
        break;
      }
    }
  }
  if(m_currentField >= 0) {
    m_controls[m_currentField]->killFocus();
  }
  for(i = 0; i < m_controls.size(); i++) {
    m_controls[i]->exitDialog();
  }

  destroy();

  Console::setCursorPos(cx,cy);
  Console::showCursor(cursorWasVisible);

  return m_return;
}

void Dialog::quit(int ret) { 
  m_return = ret;
  postEvent(MSG_QUIT);
}

void Dialog::firstField() {
  for(int i = 0; i < (int)m_controls.size(); i++) {
    if(m_controls[i]->isEnabled()) {
      gotoField(i);
      break;
    }
  }
}

void Dialog::lastField() {
  for(int i = m_controls.size() - 1; i >= 0; i--) {
    if(m_controls[i]->isEnabled()) {
      gotoField(i);
      break;
    }
  }
}

void Dialog::nextField() {
  if(m_controls.size() == 0) {
    return;
  }
  for(int i = (m_currentField+1) % m_controls.size(); i != m_currentField; i = (i+1) % m_controls.size()) {
    if(m_controls[i]->isEnabled()) {
      gotoField(i);
      break;
    }
  }
}

void Dialog::prevField() {
  if(m_controls.size() == 0) {
    return;
  }
  for(int i = (m_currentField == 0)? (m_controls.size()-1):(m_currentField-1); i != m_currentField; i = (i==0)?(m_controls.size()-1):(i-1)) {
    if(m_controls[i]->isEnabled()) {
      gotoField(i);
      break;
    }
  }
}

bool Dialog::fieldUp() {
  DialogControl &f = *m_controls[m_currentField];
  int newField = -1;
  for(int i = 0; i < (int)m_controls.size(); i++) {
    if(i == m_currentField) {
      continue;
    }
    DialogControl &t = *m_controls[i];
    if(t.isEnabled() && t.getTop() < f.getTop()) {
      if(newField == -1 || t.getTop() >= m_controls[newField]->getTop()) {
        if(newField < 0) {
          newField = i;
        } else {
          if(abs(f.getLeft() - t.getLeft()) <= abs(f.getLeft() - m_controls[newField]->getLeft())) {
            newField = i;
          }
        }
      }
    }
  }
  return (newField < 0) ? false : gotoField(newField);
}

bool Dialog::fieldDown() {
  DialogControl &f = *m_controls[m_currentField];
  int newField = -1;
  for(int i = 0; i < (int)m_controls.size(); i++) {
    if(i == m_currentField) {
      continue;
    }
    DialogControl &t = *m_controls[i];
    if(t.isEnabled() && t.getTop() > f.getTop()) {
      if(newField == -1 || t.getTop() <= m_controls[newField]->getTop()) {
        if(newField < 0) {
          newField = i;
        } else {
          if(abs(f.getLeft() - t.getLeft()) <= abs(f.getLeft() - m_controls[newField]->getLeft())) {
            newField = i;
          }
        }
      }
    }
  }
  return (newField < 0) ? false : gotoField(newField);
}

bool Dialog::fieldRight() {
  DialogControl &f = *m_controls[m_currentField];
  int newField = -1;
  for(int i = 0; i < (int)m_controls.size(); i++) {
    if(i == m_currentField) {
      continue;
    }
    DialogControl &t = *m_controls[i];
    if(t.isEnabled() && t.getLeft() > f.getLeft()) {
      if(newField == -1 || t.getLeft() <= m_controls[newField]->getLeft()) {
        if(newField < 0) {
          newField = i;
        } else {
          if(abs(f.getTop() - t.getTop()) <= abs(f.getTop() - m_controls[newField]->getTop())) {
            newField = i;
          }
        }
      }
    }
  }
  return (newField < 0) ? false : gotoField(newField);
}

bool Dialog::fieldLeft() {
  DialogControl &f = *m_controls[m_currentField];
  int newField = -1;
  for(int i = 0; i < (int)m_controls.size(); i++) {
    if(i == m_currentField) {
      continue;
    }
    DialogControl &t = *m_controls[i];
    if(t.isEnabled() && t.getLeft() < f.getLeft()) {
      if(newField == -1 || t.getLeft() >= m_controls[newField]->getLeft()) {
        if(newField < 0) {
          newField = i;
        } else {
          if(abs(f.getTop() - t.getTop()) <= abs(f.getTop() - m_controls[newField]->getTop())) {
            newField = i;
          }
        }
      }
    }
  }
  return (newField < 0) ? false : gotoField(newField);
}

bool Dialog::gotoField(int index) {
  if(index < 0 || index >= m_controls.size() || !m_controls[index]->isEnabled()) {
    return false;
  }
  DialogControl *ff = NULL;
  if(m_currentField >= 0) {
    ff = m_controls[m_currentField];
    ff->killFocus();
  }
  m_currentField = index;
  if(ff) {
    ff->draw();
  }
  DialogControl *tf = m_controls[m_currentField];
  tf->draw();
  tf->gotFocus();
  return true;
}

bool Dialog::interField(int event) {
  if(EVENTUP(event)) {
    return false;
  }
  switch(EVENTSCAN(event)) {
  case SCAN_TAB  :
    if(EVENTSTATE(event) & SHIFT_PRESSED) {
      prevField();
    } else {
      nextField();
    }
    return true;
  case SCAN_LEFT :
    return fieldLeft();
  case SCAN_RIGHT:
    return fieldRight();
  case SCAN_UP   :
    return fieldUp();
  case SCAN_DOWN :
    return fieldDown();
  default        :
    return false;
  }
  return false;
}

void Dialog::setColor(WORD color) {
  m_color = color;
  if(isVisible()) {
    draw();
  }
}

void Dialog::setBorderColor(WORD color) {
  m_borderColor = color;
  if(isVisible()) {
    drawBorder();
  }
}

void Dialog::setBorderType(FrameType bordertype) {
  m_borderType = bordertype;
  if(isVisible()) {
    drawBorder();
  }
}

void Dialog::setNormalFieldColor(WORD color) {
  m_fieldColor = color;
  if(isVisible()) {
    drawControls();
  }
}

void Dialog::setSelectedFieldColor(WORD color) {
  m_selFieldColor = color;
  if(isVisible()) {
    drawControls();
  }
}

void Dialog::setSelectedTextColor( WORD color) {
  m_selTextColor = color;
  if(isVisible()) {
    drawControls();
  }
}

void Dialog::setLabel(int id, const String &label) {
  ((DialogField*)getDlgItem(id))->setLabel(label);
}

void Dialog::setTitle(const String &title) {
  m_title = title;
  if(isVisible()) {
    drawBorder();
  }
}

void Dialog::setShadow(bool on) {
  m_shadowed = on;
  if(isVisible()) {
    ConsoleWindow::setShadow(m_shadowed);
  }
}

void Dialog::setMenu(Menu *m) {
  if(m_menu && isVisible()) {
    m_menu->destroy();
  }

  m_menu = m;

  if(m_menu) {
    m_menu->setHostWindow(this);
    if(isVisible()) {
      drawMenu();
    }
  }
}

Menu *Dialog::getMenu() {
  return m_menu;
}

void Dialog::getRect(SMALL_RECT &rect) {
  rect.Left   = m_left;
  rect.Top    = m_top;
  rect.Right  = m_left + m_width;
  rect.Bottom = m_top  + m_height;
}

DialogButton::DialogButton(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
: DialogControl(dlg,id,left,top,width,height) {
  m_label   = label;
  m_pressed = false;
}

void DialogButton::enterDialog() {
  m_pressed = false;
}

void DialogButton::gotFocus() {
  m_pressed = false;
  Console::showCursor(false);
}

bool DialogButton::key(int event) {
  if(getDialog().interField(event)) {
    return true;
  }
  if(EVENTTYPE(event) == EVENTTYPE_KEY && EVENTSCAN(event) == SCAN_ENTER) {
    if(EVENTDOWN(event)) {
      m_pressed = true;
      return true;
    } else if(EVENTUP(event) && m_pressed) {
      getDialog().postEvent(COMMANDEVENT(getid()));
      m_pressed = false;
      return true;
    }
  }
  m_pressed = false;
  return false;
}

void DialogButton::exitDialog() {
}

void DialogButton::killFocus() {
  m_pressed = false;
  Console::showCursor(true);
}

void DialogButton::draw() {
  getDialog().getWin()->printf(getLeft(), getTop(), getColor(), _T("%s"), m_label.cstr());
}
