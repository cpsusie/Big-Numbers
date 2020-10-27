#include "stdafx.h"
#define APSTUDIO_INVOKED
#include "resource.h"
#include "EditFieldArray.h"

#define FIELDID(r,c) (((r)*9+(c)) + _APS_NEXT_CONTROL_VALUE + 1)

void EditFieldArray::putMatrix(const FieldMatrix &m) {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      m_fields[r][c].putValue(m.get(r,c));
    }
  }
}

FieldMatrix EditFieldArray::getMatrix() const {
  FieldMatrix m;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      m.put(r,c, m_fields[r][c].getValue());
    }
  }
  return m;
}

void EditFieldArray::Create(CWnd *parent) {
  m_parent = parent;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      CRect rect;
      rect.top    = r * 39    + 60;
      rect.bottom = rect.top  + 25;
      rect.left   = c * 38    + 48;
      rect.right  = rect.left + 25;
      MarkableEditField &f = m_fields[r][c];
      f.Create(WS_VISIBLE | WS_GROUP | WS_TABSTOP,rect,parent,FIELDID(r,c));
      f.SetLimitText(1);
    }
  }
  m_fieldsCreated  = true;
}

void EditFieldArray::OnPaint() {
  CFont *font = m_parent->GetFont();
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      MarkableEditField &f = m_fields[r][c];
      if(!m_fontIsSet) {
        f.SetFont(font,false);
      }
      m_fields[r][c].OnPaint();
    }
  }
  m_fontIsSet = true;
}

MarkableEditField *EditFieldArray::findField(const CPoint &p) {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      MarkableEditField *f = &m_fields[r][c];
      CRect r;
      f->GetWindowRect(&r);
      if(r.PtInRect(p)) {
        return f;
      }
    }
  }
  return nullptr;
}

MarkableEditField::MarkableEditField() {
  m_fixed = m_marked = m_wrong = false;
}

void MarkableEditField::setMarked(bool marked) {
  m_marked = marked;
  adjustEnable();
}

void MarkableEditField::setFixed(bool fixed) {
  m_fixed = fixed;
  adjustEnable();
}

void MarkableEditField::setError(bool wrong) {
  m_wrong = wrong;
}

void MarkableEditField::adjustEnable() {
  if(m_marked || m_fixed) {
    EnableWindow(false);
  } else {
    EnableWindow(true);
  }
}

void MarkableEditField::OnPaint() {
  if(m_fixed) {
    return;
  }
  if(m_wrong || m_marked) {
    COLORREF color = m_wrong ? RED : m_marked ? GREEN : WHITE;
    CPaintDC dc(this);
    CRect r;
    String str = getWindowText(this);
    GetClientRect(&r);
    dc.SetBkColor(color);
    dc.SelectObject(GetFont());
    textOut(dc, r.left,r.top, str + _T("  "));
  }
}

int MarkableEditField::getValue() const {
  CString str;
  GetWindowText(str);
  int v;
  if(_stscanf((LPCTSTR)str,_T("%d"),&v) != 1 || v < 1 || v > 9) {
    v = 0;
  }
  return v;
}

void MarkableEditField::putValue(int v) {
  String str;
  if(v > 0) {
    str = format(_T("%d"), v);
  } else {
    str = EMPTYSTRING;
  }
  setWindowText(this, str);
}
