#include "pch.h"
#include <Console.h>

#define NORMALFIELDEVENT(event) (EVENTDOWN(event) && !(EVENTSTATE(event) & (CTRL_PRESSED | ALT_PRESSED)))
#define FILTER(event) if(!NORMALFIELDEVENT(event)) return false

StringField::StringField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
: DialogField(dlg,id,left,top,width,height,label)
{
}

void StringField::dataToField() {
  setBuffer(getString().cstr());
}

void StringField::fieldToData() {
  getString() = getBuffer();
}

void StringField::enterDialog() {
  dataToField();
  setCursorPos(0);
}

void StringField::gotFocus( ) {
  enterDialog();
  setCursorPos();
}

bool StringField::key(int event) {
  if(navigate(event)) return true;
  if(getDialog().interField(event)) return true;
  FILTER(event);
  String oldstr = getBuffer();
  unsigned char ch = EVENTASCII(event);
  if(ch >= ' ') {
    if(isSelected()) {
      substSelected(ch);
    } else if(insertChar(ch)) {
      charRight();
    }
    if(getBuffer() != oldstr) {
      postChangeEvent();
    }
    return true;
  }
  return false;
}

void StringField::exitDialog() {
  fieldToData();
}

void StringField::killFocus() {
  exitDialog();
}

/* ----------------------------------------------------------------------- */

IntField::IntField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
 : DialogField(dlg,id,left,top,width,height,label)
{
  m_var = 0;
}

void IntField::dataToField() {
  const String tmp = format(_T("%d"), getInt());
  setBuffer(tmp.cstr());
}

void IntField::fieldToData() {
  int d;
  if(_stscanf(getBuffer().cstr(),_T("%d"),&d) != 1)
    d = 0;
  getInt() = d;
}

void IntField::enterDialog() {
  dataToField();
  setCursorPos(0);
}

void IntField::gotFocus() {
  enterDialog();
  setCursorPos();
}

bool IntField::key(int event) {
  if(navigate(event)) {
    return true;
  }
  if(getDialog().interField(event)) {
    return true;
  }
  FILTER(event);
  String oldstr = getBuffer();
  char ch = EVENTASCII(event);
  if(isdigit(ch) || ch == '-' || ch == '+') {
    if(isSelected()) {
      substSelected(ch);
    } else if(insertChar(ch)) {
      charRight();
    }
    if(getBuffer() != oldstr) {
      postChangeEvent();
    }
    return true;
  }
  return false;
}

void IntField::exitDialog() {
  fieldToData();
}

void IntField::killFocus() {
  exitDialog();
}

ShortField::ShortField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
 : DialogField(dlg,id,left,top,width,height,label)
{
  m_var = 0;
}

void ShortField::dataToField() {
  const String tmp = format(_T("%d"), getShort());
  setBuffer(tmp.cstr());
}

void ShortField::fieldToData() {
  int d;
  if(_stscanf(getBuffer().cstr(),_T("%d"),&d) != 1)
    d = 0;
  getShort() = d;
}

void ShortField::enterDialog() {
  dataToField();
  setCursorPos(0);
}

void ShortField::gotFocus() {
  enterDialog();
  setCursorPos();
}

bool ShortField::key(int event) {
  if(navigate(event)) {
    return true;
  }
  if(getDialog().interField(event)) {
    return true;
  }
  FILTER(event);
  String oldstr = getBuffer();
  char ch = EVENTASCII(event);
  if(isdigit(ch) || ch == '-' || ch == '+') {
    if(isSelected()) {
      substSelected(ch);
    } else if(insertChar(ch)) {
      charRight();
    }
    if(getBuffer() != oldstr) {
      postChangeEvent();
    }
    return true;
  }
  return false;
}

void ShortField::exitDialog() {
  fieldToData();
}

void ShortField::killFocus() {
  exitDialog();
}

DoubleField::DoubleField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
: DialogField(dlg,id,left,top,width,height,label)
{
  m_var = 0;
}

void DoubleField::dataToField() {
  String tmp = format(_T("%lf"),getDouble());
  setBuffer(tmp.cstr());
}

void DoubleField::fieldToData() {
  double d;
  if(_stscanf(getBuffer().cstr(),_T("%lf"),&d) != 1) {
    d = 0;
  }
  getDouble() = d;
}

void DoubleField::enterDialog() {
  dataToField();
  setCursorPos(0);
}

void DoubleField::gotFocus() {
  enterDialog();
  setCursorPos();
}

bool DoubleField::key(int event) {
  if(navigate(event)) {
    return true;
  }
  if(getDialog().interField(event)) {
    return true;
  }
  FILTER(event);
  String oldstr = getBuffer();
  char ch = EVENTASCII(event);
  if(isdigit(ch) || ch == '-' || ch == '+') {
    if(isSelected()) {
      substSelected(ch);
    } else if(insertChar(ch)) {
      charRight();
    }
    if(getBuffer() != oldstr) {
      postChangeEvent();
    }
    return true;
  }
  return false;
}

void DoubleField::exitDialog() {
  fieldToData();
}

void DoubleField::killFocus() {
  exitDialog();
}


BoolField::BoolField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label)
 : DialogField(dlg,id,left,top,width,height,label)
{
  m_var = false;
}

void BoolField::dataToField() {
  const String tmp = getBool()?_T("X"):_T(" ");
  setBuffer(tmp.cstr());
}

void BoolField::fieldToData() {
  if(getBuffer()[0] == _T(' ')) {
    getBool() = false;
  } else {
    getBool() = true;
  }
}

void BoolField::enterDialog() {
  dataToField();
  setCursorPos(0);
}

void BoolField::gotFocus() {
  enterDialog();
  setCursorPos();
}

bool BoolField::key(int event) {
  if(getDialog().interField(event)) {
    return true;
  }

  FILTER(event);
  switch(EVENTASCII(event)) {
  case ' ':
    if(getBuffer()[0] == ' ') {
      setBuffer(_T("X"));
    } else {
      setBuffer(_T(" "));
    }
    postChangeEvent();
    return true;
  case 'y':
  case 'Y':
  case 'j':
  case 'J':
    setBuffer(_T("X"));
    postChangeEvent();
    return true;
  case 'n':
  case 'N':
    setBuffer(_T(" "));
    postChangeEvent();
    return true;
  }
  return false;
}

void BoolField::exitDialog() {
  fieldToData();
}

void BoolField::killFocus() {
  exitDialog();
}
