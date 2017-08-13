#include "stdafx.h"
#include "DebugTextWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDebugTextWindow::CDebugTextWindow() : m_breakPoints(10), m_possibleBreakPointLines(10) {
  m_allowMarking = true;
}

CDebugTextWindow::~CDebugTextWindow() {
}

BEGIN_MESSAGE_MAP(CDebugTextWindow, CListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
END_MESSAGE_MAP()

void CDebugTextWindow::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;
  CListBox *oldCtrl = (CListBox*)parent->GetDlgItem(id);
  if(oldCtrl == NULL) {
    AfxMessageBox(format(_T("%s:Control with id=%d does not exist"), method,id).cstr(), MB_ICONWARNING);
    return;
  }
  CRect wr;
  oldCtrl->GetWindowRect(&wr);
  parent->ScreenToClient(&wr);
  DWORD style   = oldCtrl->GetStyle();
  style |= LBS_OWNERDRAWFIXED | WS_BORDER;
  style &= ~(LBS_SORT);

  const DWORD exStyle = oldCtrl->GetExStyle();

  CFont *font = oldCtrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }

  oldCtrl->DestroyWindow();

  if(!Create(style, wr, parent, id)) {
    AfxMessageBox(format(_T("%s:Create failed"), method).cstr(), MB_ICONWARNING);
    return;
  }
  ModifyStyleEx(0, exStyle | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
  SetFont(font);
  m_currentLine = -1;
  m_lineMarkBitmap.LoadBitmap(IDB_BITMAP_BLACK_RIGHTARROW);
  m_arrowBitmapSize = getBitmapSize(m_lineMarkBitmap);
  m_breakPointBitmap.LoadBitmap(IDB_BITMAP_BREAKPOINT);
  m_breakPointBitmapSize = getBitmapSize(m_breakPointBitmap);
}

void CDebugTextWindow::clear() {
  ResetContent();
  m_codeText = EMPTYSTRING;
}

void CDebugTextWindow::scrollToBottom() {
  const int lines = GetCount();
  if(lines > 0) {
    SetTopIndex(lines-1);
  }
}

void CDebugTextWindow::setText(const String &text) {
  if(text == m_codeText) {
    return;
  }
  m_breakPoints.clear();
  m_currentLine = -1;
  ResetContent();
  m_textLines = StringArray(Tokenizer(text, _T("\n")));
  for(size_t i = 0; i < m_textLines.size(); i++) {
    AddString(EMPTYSTRING);
  }
  m_codeText = text;
}

bool CDebugTextWindow::isItemVisible(int index) {
  const int topIndex = GetTopIndex();
  if(index < topIndex) {
    return false;
  } else {
    CRect ir, cr = getClientRect(this);
    const int itemsVisible = cr.Size().cy / m_itemHeight;
    GetItemRect(index, &ir);
    return ir.bottom <= cr.bottom;
  }
}

void CDebugTextWindow::setItemVisible(int index) {
  const int oldTopIndex = GetTopIndex();
  CRect cr = getClientRect(this);
  const int itemsVisible = cr.Size().cy / m_itemHeight;
  const int newTopIndex = max(0, index - itemsVisible/2);
  if(newTopIndex != oldTopIndex) {
    SetTopIndex(newTopIndex);
  } else {
    redrawItem(index);
  }
}

int CDebugTextWindow::getHighestBreakPointLine(int line) const {
  for(;line >= 0; line--) {
    if(m_possibleBreakPointLines.contains(line)) {
      break;
    }
  }
  return line;
}

void CDebugTextWindow::markCurrentLine(int line) {
  const int lineCount = GetCount();
  if(line != m_currentLine) {
    if((line >= 0) && (line < lineCount)) {
      m_currentLine = line;
      if(!isItemVisible(line)) {
        setItemVisible(line);
      } else {
        redrawItem(line);
      }
    } else {
      const int oldCurrent = m_currentLine;
      m_currentLine = -1;
      if((oldCurrent >= 0) && (oldCurrent < lineCount) && isItemVisible(oldCurrent)) {
        redrawItem(oldCurrent);
      }
    }
  }
}

void CDebugTextWindow::setAllowMarking(bool allowMarking, const BitSet *possibleBreakPointLines) {
  if(allowMarking != m_allowMarking) {
    m_allowMarking = allowMarking;
    Invalidate();
  }
  if(possibleBreakPointLines) {
    m_possibleBreakPointLines = *possibleBreakPointLines;
  }
}

void CDebugTextWindow::addBreakPoint(UINT line) {
  const int l = getHighestBreakPointLine(line);
  if(l < 0) return;

  const int lineCount = GetCount();
  if(lineCount > (int)m_breakPoints.getCapacity()) {
    m_breakPoints.setCapacity(lineCount);
  }
  if((l < lineCount) && !m_breakPoints.contains(line)) {
    m_breakPoints.add(l);
    redrawItem(l);
  }
}

void CDebugTextWindow::removeBreakPoint(UINT line) {
  const int l = getHighestBreakPointLine(line);
  if(l < 0) return;
  const int lineCount = GetCount();
  if((l < lineCount) && m_breakPoints.contains(l)) {
    m_breakPoints.remove(l);
    redrawItem(l);
  }
}

void CDebugTextWindow::redrawItem(UINT index) {
  CRect cr = getClientRect(this);
  CRect ir;
  GetItemRect(index, &ir);
  CRect r2;
  if(r2.IntersectRect(&cr, &ir)) {
    RedrawWindow(&ir); // , NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
  }
}

void CDebugTextWindow::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
  CDC dc;
  dc.Attach(lpDrawItemStruct->hDC);

  // Save these value to restore them when done drawing.
  const COLORREF crOldTextColor = dc.GetTextColor();
  const COLORREF crOldBkColor   = dc.GetBkColor();

  const int textLeft = m_allowMarking ? (m_breakPointBitmapSize.cx + m_arrowBitmapSize.cx + 4) : 2;

  if(m_hasFocus && ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))) {
    dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
    CRect r = lpDrawItemStruct->rcItem;
    r.left = textLeft;
    dc.FillSolidRect(&r, ::GetSysColor(COLOR_HIGHLIGHT));
  } else {
    dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
  }

  const CRect       &rc    = lpDrawItemStruct->rcItem;
  const UINT        index = lpDrawItemStruct->itemID;

  if(index < m_textLines.size()) {
    if(m_allowMarking && ((index == m_currentLine) || m_breakPoints.contains(index))) {
      CDC tmpDC;
      tmpDC.CreateCompatibleDC(NULL);

      if(index == m_currentLine) {
        CBitmap *oldBitmap = tmpDC.SelectObject(&m_lineMarkBitmap);
        const int arrowLeft = m_breakPointBitmapSize.cx + 3;
        const int arrowTop  = (m_itemHeight - m_arrowBitmapSize.cy)/2;
        dc.BitBlt(arrowLeft, rc.top + arrowTop, m_arrowBitmapSize.cx, m_arrowBitmapSize.cy, &tmpDC, 0,0, SRCCOPY);
        tmpDC.SelectObject(oldBitmap);
      }
      if(m_breakPoints.contains(index)) {
        const int breakOffset = (m_itemHeight - m_breakPointBitmapSize.cy)/2;
        CBitmap *oldBitmap = tmpDC.SelectObject(&m_breakPointBitmap);
        dc.BitBlt(1, rc.top+breakOffset, m_breakPointBitmapSize.cx, m_breakPointBitmapSize.cy, &tmpDC, 0,0, SRCCOPY);
        tmpDC.SelectObject(oldBitmap);
      }
    }
    textOut(dc, textLeft, rc.top, m_textLines[index]);
  }

  dc.SetTextColor(crOldTextColor);
  dc.SetBkColor(  crOldBkColor  );

  dc.Detach();
}

void CDebugTextWindow::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
  m_itemHeight = lpMeasureItemStruct->itemHeight;
}

void CDebugTextWindow::OnSetfocus() {
  m_hasFocus = true;
  const int curSel = GetCurSel();
  if(curSel >= 0) {
    if(!isItemVisible(curSel)) {
      setItemVisible(curSel);
    } else {
      redrawItem(curSel);
    }
  }
}

void CDebugTextWindow::OnKillfocus() {
  m_hasFocus = false;
  redrawItem(GetCurSel());
}
