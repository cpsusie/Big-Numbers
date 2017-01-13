#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/EditListStringCombo.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditListStringCombo::CEditListStringCombo() {
}

CEditListStringCombo::~CEditListStringCombo() {
}


BEGIN_MESSAGE_MAP(CEditListStringCombo, CComboBox)
    ON_CONTROL_REFLECT(CBN_SETFOCUS , OnSetFocus )
    ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillFocus)
    ON_CONTROL_REFLECT(CBN_DROPDOWN , OnDropDown )
    ON_CONTROL_REFLECT(CBN_CLOSEUP  , OnCloseUp  )
END_MESSAGE_MAP()

static int countBytes(const StringArray &sa) {
  size_t byteCount = 0;
  for(size_t i = 0; i < sa.size(); i++) {
    byteCount += sa[i].length() + 1;
  }
  byteCount *= sizeof(TCHAR);
  return (int)(sa.size() ? (byteCount / sa.size()) : 1);
}

BOOL CEditListStringCombo::Create(CWnd *parent, int id, const StringArray &stringArray, UINT flags) {
  DEFINEMETHODNAME;
  const CRect r(10,10,20,20);

  m_flags = flags;

  const DWORD dropStyle = m_flags & (LF_STRCOMBO_DROPLIST | LF_STRCOMBO_DROPDOWN);
  if(dropStyle == 0) {
    throwInvalidArgumentException(method, _T("One of LF_STRCOMBO_DROPLIST and LF_STRCOMBO_DROPDOWN must be specified"));
  } else if(dropStyle == (LF_STRCOMBO_DROPLIST | LF_STRCOMBO_DROPDOWN)) {
    throwInvalidArgumentException(method, _T("Only one of LF_STRCOMBO_DROPLIST and LF_STRCOMBO_DROPDOWN can be specified"));
  }

  DWORD dwStyle = WS_CHILD | WS_VSCROLL | CBS_AUTOHSCROLL;
  if(m_flags & LF_STRCOMBO_DROPLIST) {
    dwStyle |= CBS_DROPDOWNLIST;
  } else {
    dwStyle |= CBS_DROPDOWN;
  }

  const BOOL ok = CComboBox::Create(dwStyle, r, parent, id);
  if(ok) {
    SetFont(parent->GetFont());
  }
  if(stringArray.size() > 100) {
    if(InitStorage((int)stringArray.size(), countBytes(stringArray)) == CB_ERRSPACE) {
      /* ignore */
    }
  }
  for(size_t i = 0; i < stringArray.size(); i++) {
    AddString(stringArray[i].cstr());
  }
  CClientDC dc(this);
  CFont *oldFont = dc.SelectObject(GetFont());
  TEXTMETRIC tm;
  dc.GetTextMetrics(&tm);
  const CSize saExtent = getMaxTextExtent(dc, stringArray);
  dc.SelectObject(oldFont);
  SetHorizontalExtent(saExtent.cx + tm.tmAveCharWidth);

  return ok;
}

String CEditListStringCombo::getListString(int index) const {
  if((index < 0) || (index >= GetCount())) {
    return _T("");
  }
  CString str;
  GetLBText(index, str);
  return (LPCTSTR)str;
}

void CEditListStringCombo::paintComboBoxArrow(HDC hdc, const CRect &cellRect, bool enabled) { // static
  static CBitmap downArrowBitmap[2];
  if(downArrowBitmap[0].m_hObject == NULL) {
    downArrowBitmap[0].LoadOEMBitmap(OBM_DNARROW);
    downArrowBitmap[1].LoadOEMBitmap(OBM_DNARROWI);
  }
  static HDC srcDC = CreateCompatibleDC(NULL);
  HGDIOBJ oldGDI = SelectObject(srcDC, downArrowBitmap[enabled?0:1]);
  const CRect r = getArrowRect(cellRect);
//  const CSize sz       = getComboArrowSize();
//  const int   leftEdge = max(0,cellRect.right - sz.cx - 2);
//  const int   topEdge  = cellRect.top + 3;
//  BitBlt(hdc, leftEdge,topEdge,sz.cx,sz.cy,srcDC,0,0, SRCCOPY);
  BitBlt(hdc, r.left,r.top, r.Width(), r.Height(), srcDC,0,0, SRCCOPY);
  SelectObject(srcDC, oldGDI);
//  DeleteDC(srcDC);
}

CRect CEditListStringCombo::getArrowRect(const CRect &cellRect) { // static
  const CSize sz   = getComboArrowSize();
  const int left   = max(cellRect.left   , cellRect.right - sz.cx - 2);
  const int top    = min(cellRect.top + 3, cellRect.bottom           );
  const int right  = min(cellRect.right  , left + sz.cx              );
  const int bottom = min(cellRect.bottom , top  + sz.cy              );
  return CRect(left, top, right, bottom);
}

void CEditListStringCombo::setEditable(bool canEdit) {
  EnableWindow(canEdit ? TRUE : FALSE);
}

String CEditListStringCombo::getStringValue() {       // Used for style LF_STRCOMBO_DROPDOWN
  assertIsDropDown(__TFUNCTION__);
  return getWindowText(this);
}

void CEditListStringCombo::setStringValue(const String &value) {
  assertIsDropDown(__TFUNCTION__);
  setWindowText(this, value);
}

int CEditListStringCombo::getIntValue() {
  assertIsDropList(__TFUNCTION__);
  return GetCurSel();
}

void CEditListStringCombo::setIntValue(int value) {
  assertIsDropList(__TFUNCTION__);
  const int count = GetCount();
  if(value >= count) {
    return;
  }
  SetCurSel(value);
}

void CEditListStringCombo::assertIsDropDown(const TCHAR *function) const {
  if(!(m_flags & LF_STRCOMBO_DROPDOWN)) {
    throwException(_T("%s():Combo not DropDown"), function);
  }
}

void CEditListStringCombo::assertIsDropList(const TCHAR *function) const {
  if(!(m_flags & LF_STRCOMBO_DROPLIST)) {
    throwException(_T("%s():Combo not DropList"), function);
  }
}

void CEditListStringCombo::OnSetFocus() {
//  debugLog("  Combo OnSetfocus\n");
  if(getListCtrl()->lastMessageWasMouseClick()) {
    const CPoint p  = getLastMouseClick();
    const CRect  ar = getArrowRect(getListCtrl()->getCurrentCellRect());
    if(ar.PtInRect(p)) {
      ShowDropDown();
    }
  }
}

void CEditListStringCombo::OnKillFocus() {
//  debugLog("  Combo OnKillfocus\n");
}

void CEditListStringCombo::OnDropDown() {
//  debugLog("  Combo OnDropdown\n");
}

void CEditListStringCombo::OnCloseUp() {
//  debugLog("  Combo OnCloseup\n");
}
