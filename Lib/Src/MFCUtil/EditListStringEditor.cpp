#include "pch.h"
#include <MFCUtil/EditListStringEditor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditListStringEditor::CEditListStringEditor() {
}

CEditListStringEditor::~CEditListStringEditor() {
}


BEGIN_MESSAGE_MAP(CEditListStringEditor, CEdit)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CEditListStringEditor::Create(CWnd *parent, int id, UINT flags) {
  const char *function = "CEditListStringEditor::Create";
  const CRect r(10,10,20,20);
  const BOOL ok = CEdit::Create(WS_CHILD | ES_AUTOHSCROLL, r, parent, id);
  if(ok) {
    SetFont(parent->GetFont());
  }
  SetMargins(0,0);
  m_flags = flags;
  return ok;
}

void CEditListStringEditor::setValue(const String &v) {
  validateValue(v);
  const String str = valueToString(v);
  setWindowText(this, str);
  SetSel(0, (int)str.length());
}

String CEditListStringEditor::getValue() {
  const String v = getWindowText(this);
  validateValue(v);
  return v;
}

void   CEditListStringEditor::setEditable(bool canEdit) {
  ::SendMessage(m_hWnd, EM_SETREADONLY, canEdit ? FALSE : TRUE, 0);
}

String CEditListStringEditor::valueToString(const String &v) const {
  return v;
}

void CEditListStringEditor::validateValue(const String &v) const {
}

void CEditListStringEditor::OnSetFocus(CWnd *pOldWnd) {
//  debugLog("  StringEditor::OnSetFocus() cell:%s\n", getCellStr().cstr());
  CEdit::OnSetFocus(pOldWnd);
}

void CEditListStringEditor::OnKillFocus(CWnd *pNewWnd) {
//  debugLog("  StringEditor::OnKillFocus() cell:%s\n", getCellStr().cstr());
  CEdit::OnKillFocus(pNewWnd);
}

