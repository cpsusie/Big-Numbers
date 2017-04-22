#include "pch.h"
#include <MFCUtil/EditListBooleanEditor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditListBooleanEditor::CEditListBooleanEditor() {
}

CEditListBooleanEditor::~CEditListBooleanEditor() {
}


BEGIN_MESSAGE_MAP(CEditListBooleanEditor, CButton)
    ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL CEditListBooleanEditor::Create(CWnd *parent, int id) {
  const CRect r(CPoint(0,0), getCheckBoxSize());
  const BOOL ok = CButton::Create(EMPTYSTRING, WS_CHILD | BS_AUTOCHECKBOX | BS_NOTIFY, r, parent, id);
  if(ok) {
    ModifyStyleEx(0, /*WS_EX_STATICEDGE | */WS_EX_CLIENTEDGE);
  }
  return ok;
}

void CEditListBooleanEditor::setEditable(bool canEdit) {
  EnableWindow(canEdit ? TRUE : FALSE);
}

CPoint CEditListBooleanEditor::getCheckBoxPosition(const CRect &cellRect) { // static
  const CSize sz = getCheckBoxSize();
  const int leftEdge = max(cellRect.CenterPoint().x - sz.cx/2, cellRect.left);
  const int topEdge  = max(cellRect.CenterPoint().y - sz.cy/2-1, cellRect.top );
  return CPoint(leftEdge, topEdge);
}

void CEditListBooleanEditor::setValue(bool checked) {
  SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
  m_checked = checked;
}

bool CEditListBooleanEditor::getValue() {
  return m_checked;
}

void CEditListBooleanEditor::paintCheckBox(HDC hdc, const CPoint &p, bool checked, bool enabled) { // static
  static CBitmap checkBoxesBitmap;
  if(checkBoxesBitmap.m_hObject == NULL) {
    checkBoxesBitmap.LoadOEMBitmap(OBM_CHECKBOXES);
  }

  int srcY = 0;
  int srcX = 0;
  const CSize sz = getCheckBoxSize();
  if(!enabled) srcX += sz.cx*2;
  if( checked) srcX += sz.cx;
  HDC srcDC = CreateCompatibleDC(hdc);
  HGDIOBJ oldGDI = SelectObject(srcDC, checkBoxesBitmap);
  BitBlt(hdc, p.x,p.y,sz.cx,sz.cy,srcDC,srcX,srcY, SRCCOPY);
  SelectObject(srcDC, oldGDI);
  DeleteDC(srcDC);
}

void CEditListBooleanEditor::OnClicked() {
  setValue(!getValue());
  Invalidate();
}

void CEditListBooleanEditor::OnSetFocus(CWnd *pOldWnd) {
  CButton::OnSetFocus(pOldWnd);
  if(getListCtrl()->lastMessageWasMouseClick()) {
    OnClicked();
  }
}
