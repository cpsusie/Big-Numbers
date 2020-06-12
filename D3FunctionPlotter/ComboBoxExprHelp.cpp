#include "stdafx.h"
#include <WinTools.h>
#include <Math/Expression/Expression.h>
#include "ComboBoxExprHelp.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CComboBoxExprHelp::CComboBoxExprHelp() {
}

CComboBoxExprHelp::~CComboBoxExprHelp() {
}

BEGIN_MESSAGE_MAP(CComboBoxExprHelp, CComboBox)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
END_MESSAGE_MAP()

void CComboBoxExprHelp::substituteControl(CWnd *parent, int ctrlId) {
  CComboBox *ctrl    = (CComboBox*)parent->GetDlgItem(ctrlId);
  DWORD      style   = ctrl->GetStyle() | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL;
  DWORD      exStyle = ctrl->GetExStyle();
  CFont     *font    = parent->GetFont();
  CRect      r;

  ctrl->GetDroppedControlRect(&r);
  parent->ScreenToClient(&r);

  ctrl->DestroyWindow();
  findColumnWidth(parent);
  if(!Create(style, r, parent, ctrlId)) {
    showError(_T("%s:Create() failed"), __TFUNCTION__);
    return;
  }
  ModifyStyleEx(0,exStyle);
  SetFont(font, false);
  SetDroppedWidth(getDropDownWidth());
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  for(int i = 0; i < m_itemCount; i++) {
    AddString(format(_T("%d"), i).cstr());
    SetItemData(i, (DWORD)(helpList + i));
  }
}

void CComboBoxExprHelp::findColumnWidth(CWnd *parent) {
  CClientDC dc(parent);

  CFont *font    = parent->GetFont();
  CFont *oldfont = dc.SelectObject(font);
  m_columnWidth1 = m_columnWidth2 = m_itemHeight = 0;
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  for(int i = 0; i < count; i++) {
    const ExpressionDescription &item = helpList[i];
    const CString syntax = item.getSyntax();
    const CString desc   = item.getDescription();
    CSize sz = dc.GetTextExtent(syntax);
    m_columnWidth1 = max(m_columnWidth1, sz.cx+2);
    m_itemHeight   = max(m_itemHeight, sz.cy+2);
    sz = dc.GetTextExtent(desc);
    m_columnWidth2 = max(m_columnWidth2, sz.cx+2);
    m_itemHeight   = max(m_itemHeight, sz.cy+2);
  }
  m_itemCount = count;
}

void CComboBoxExprHelp::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
  if(lpMeasureItemStruct->itemID != (UINT)-1) {
    lpMeasureItemStruct->itemHeight = m_itemHeight;
    lpMeasureItemStruct->itemWidth  = getDropDownWidth();
  }
}

#define LEFT_MARG  3
#define RIGHT_MARG 3

void CComboBoxExprHelp::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
   ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);

   const ExpressionDescription *helpItem = (const ExpressionDescription*)lpDrawItemStruct->itemData;
   if((int)helpItem == -1) return;

   CDC dc;

   dc.Attach(lpDrawItemStruct->hDC);

   // Save these value to restore th½½em when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor   = dc.GetBkColor();

   // If this item is selected, set the background color and the text color to appropriate values.
   // Erase the rect by filling it with the background color.
   if((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED)) {
     dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
     dc.SetBkColor(  ::GetSysColor(COLOR_HIGHLIGHT    ));
     dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
   } else {
     dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
   }

   const String text1 = helpItem->getSyntax();
   const String text2 = helpItem->getDescription();

   CRect rc = lpDrawItemStruct->rcItem;
   rc.left = LEFT_MARG; rc.right = rc.left + m_columnWidth1;
   dc.DrawText(text1.cstr(), text1.length(), &rc, /*DT_SINGLELINE|*/DT_VCENTER);

   dc.MoveTo(rc.right+RIGHT_MARG, rc.top   );
   dc.LineTo(rc.right+RIGHT_MARG, rc.bottom);

   rc.left = rc.right + RIGHT_MARG + 1 + LEFT_MARG; rc.right = rc.left + m_columnWidth2;
   dc.DrawText(text2.cstr(), text2.length(), &rc, /*DT_SINGLELINE|*/DT_VCENTER);

   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();
}

int CComboBoxExprHelp::getDropDownWidth() const {
  return LEFT_MARG + m_columnWidth1 + RIGHT_MARG + 1 + LEFT_MARG + m_columnWidth2 + RIGHT_MARG;
}

void CComboBoxExprHelp::OnDropdown() {
}

String CComboBoxExprHelp::getSelectedString() {
  const int selectedIndex = GetCurSel();
  if(selectedIndex < 0) {
    return "";
  }
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  return helpList[selectedIndex].getSyntax();
}

/*
void initExprHelpCombo(CComboBox *ctrl) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  for(int i = 0; i < count; i++) {
    ctrl->AddString(helpList[i].getSyntax());
  }
}
*/

