#include "stdafx.h"
#include "IRemes.h"
#include "ListBoxDiffMarks.h"

IMPLEMENT_DYNAMIC(CListBoxDiffMarks, CListBox)

CListBoxDiffMarks::CListBoxDiffMarks() {
  m_diffMarksEnabled = true;
}

CListBoxDiffMarks::~CListBoxDiffMarks() {
}

BEGIN_MESSAGE_MAP(CListBoxDiffMarks, CListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
END_MESSAGE_MAP()

void CListBoxDiffMarks::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;
  CListBox *oldCtrl = (CListBox*)parent->GetDlgItem(id);
  if(oldCtrl == NULL) {
    AfxMessageBox(format(_T("%s:Control with id=%d does not exist"), method, id).cstr(), MB_ICONWARNING);
    return;
  }
  CRect wr;
  oldCtrl->GetWindowRect(&wr);
  parent->ScreenToClient(&wr);
  DWORD style   = oldCtrl->GetStyle();
  const DWORD exStyle = oldCtrl->GetExStyle();
  style |= LBS_OWNERDRAWFIXED | WS_BORDER;
  style &= ~(LBS_SORT);

  CFont *font = oldCtrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  const CompactIntArray tabOrder = getTabOrder(parent);
  oldCtrl->DestroyWindow();

  if(!Create(style, wr, parent, id)) {
    AfxMessageBox(format(_T("%s:Create failed"), method).cstr(), MB_ICONWARNING);
    return;
  }
  setTabOrder(parent, tabOrder);
  ModifyStyleEx(0, exStyle);
  SetFont(font);
  m_charSize.cx = m_charSize.cy = 0;
  setEqualSelected(::GetSysColor(COLOR_HIGHLIGHTTEXT),::GetSysColor(COLOR_HIGHLIGHT   ));
  setDiffSelected( ::GetSysColor(COLOR_HIGHLIGHT    ),::GetSysColor(COLOR_HIGHLIGHTTEXT));
}

typedef enum {
  EQ_NOTSELECTED
 ,DIFF_NOTSELECTED
} ColorAttr;

void CListBoxDiffMarks::setEqualNotSelected(COLORREF txtColor, COLORREF bckColor) {
  DiffMarkColorSet &cs = m_colorSet[0];
  cs.m_textEqual    = txtColor;
  cs.m_bckEqual     = bckColor;
  m_colorsInitialized.add(EQ_NOTSELECTED);
}
void CListBoxDiffMarks::setDiffNotSelected(COLORREF txtColor, COLORREF bckColor) {
  DiffMarkColorSet &cs = m_colorSet[0];
  cs.m_textDiff     = txtColor;
  cs.m_bckDiff      = bckColor;
  m_colorsInitialized.add(DIFF_NOTSELECTED);
}
void CListBoxDiffMarks::setEqualSelected(COLORREF txtColor, COLORREF bckColor) {
  DiffMarkColorSet &cs = m_colorSet[1];
  cs.m_textEqual    = txtColor;
  cs.m_bckEqual     = bckColor;
}
void CListBoxDiffMarks::setDiffSelected(COLORREF txtColor, COLORREF bckColor) {
  DiffMarkColorSet &cs = m_colorSet[1];
  cs.m_textDiff     = txtColor;
  cs.m_bckDiff      = bckColor;
}

void CListBoxDiffMarks::clear() {
  ResetContent();
  m_lastContent.clear();
  m_diffSetArray.clear();
}

void CListBoxDiffMarks::enableDiffMarks(bool enable) {
  m_diffMarksEnabled = enable;
}

void CListBoxDiffMarks::setLines(const StringArray &lines) {
  const size_t lastSize = m_lastContent.size();
  const size_t n        = lines.size();

  m_diffSetArray.clear();
  for (size_t i = 0; i < n; i++) {
    const String &line = lines[i];
    const size_t  len  = line.length();
    BitSet diffSet(max(len, 10));
    if(len > 0) {
      if (i >= lastSize) {
        diffSet.add(0,len-1);
      } else {
        const String &lastLine = m_lastContent[i];
        const size_t  lastLen  = lastLine.length();
        const size_t  lmin = min(len, lastLen);
        const TCHAR  *cp = line.cstr(), *lcp = lastLine.cstr();
        size_t        j;
        for (j = 0; j < lmin; j++) {
          if(*(cp++) != *(lcp++)) diffSet.add(j);
        }
        if(j < len) diffSet.add(j, len-1);
      }
    }
    m_diffSetArray.add(diffSet);
  }
  ResetContent();
  for(size_t i = 0; i < lines.size(); i++) {
    AddString(EMPTYSTRING);
  }
  m_lastContent = lines;
}


void CListBoxDiffMarks::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
  CDC dc;
  dc.Attach(lpDrawItemStruct->hDC);

  if(m_charSize.cx == 0) {
    CFont *font = GetFont();
    CFont *oldFont = dc.SelectObject(font);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);

    m_charSize.cx = tm.tmAveCharWidth; // tmMaxCharWidth;
    m_charSize.cy = tm.tmHeight;
    dc.SelectObject(oldFont);
  }

  // Save these value to restore them when done drawing.
  const COLORREF crOldTextColor = dc.GetTextColor();
  const COLORREF crOldBkColor   = dc.GetBkColor();

  if (!m_colorsInitialized.contains(EQ_NOTSELECTED)) {
    setEqualNotSelected(crOldTextColor, crOldBkColor);
  }
  if(!m_colorsInitialized.contains(DIFF_NOTSELECTED)) {
    setDiffNotSelected(crOldBkColor, crOldTextColor);
  }
  const DiffMarkColorSet *csp;
  if(m_hasFocus && ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))) {
    csp = m_colorSet + 1;
  } else {
    csp = m_colorSet;
  }

  const CRect &r    = lpDrawItemStruct->rcItem;
  const UINT  index = lpDrawItemStruct->itemID;
  dc.FillSolidRect(&r, csp->m_bckEqual);
  if(m_diffMarksEnabled && (csp->m_bckDiff != csp->m_bckEqual)) {
    BitSet &diffSet = m_diffSetArray[index];
    if (!diffSet.isEmpty()) {
      Iterator<size_t> it = diffSet.getIterator();
      size_t beginIndex = it.next();
      size_t endIndex   = beginIndex;
      while(it.hasNext()) {
        const size_t index = it.next();
        if(index == endIndex+1) {
          endIndex = index;
        } else {
          fillSolidRect(dc, beginIndex, endIndex, lpDrawItemStruct, csp->m_bckDiff);
          beginIndex = endIndex = index;
        }
      }
      fillSolidRect(dc, beginIndex, endIndex, lpDrawItemStruct, csp->m_bckDiff);
    }
  }
  const String &line = m_lastContent[index];
  if(!m_diffMarksEnabled) {
    textOutTransparentBackground(dc, r.TopLeft(), line, *GetFont(), csp->m_textEqual);
  } else {
    String strEq    = line;
    String strDiff  = line;
    size_t n        = line.length();
    BitSet &diffSet = m_diffSetArray[index];
    for (size_t i = 0; i < n; i++) {
      if(diffSet.contains(i)) {
        strEq[i] = ' ';
      } else {
        strDiff[i] = ' ';
      }
    }
    textOutTransparentBackground(dc, r.TopLeft(), strEq  , *GetFont(), csp->m_textEqual);
    textOutTransparentBackground(dc, r.TopLeft(), strDiff, *GetFont(), csp->m_textDiff );
  }

  dc.SetTextColor(crOldTextColor);
  dc.SetBkColor(  crOldBkColor  );

  dc.Detach();
}

void CListBoxDiffMarks::fillSolidRect(CDC &dc, size_t i1, size_t i2, LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF color) {
  const CRect &lineRect = lpDrawItemStruct->rcItem;
  const int leftPos  = (int)i1 * m_charSize.cx;
  if (leftPos < lineRect.right) {
    const int rightPos = (int)(i2+1) * m_charSize.cx;
    CRect r(leftPos, lineRect.top, rightPos, lineRect.bottom);
    dc.FillSolidRect(&r, color);
  }
}

void CListBoxDiffMarks::OnSetfocus() {
  m_hasFocus = true;
/*
  const int curSel = GetCurSel();
  if(curSel >= 0) {
    if(!isItemVisible(curSel)) {
      setItemVisible(curSel);
    } else {
      redrawItem(curSel);
    }
  }
*/
}

void CListBoxDiffMarks::OnKillfocus() {
  m_hasFocus = false;
//  redrawItem(GetCurSel());
}
