#include "stdafx.h"
#include "TextBox.h"

CTextBox::CTextBox(CString &str) : m_str(str) {
  m_marked = false;
  m_redBrush.CreateSolidBrush(RGB(255, 0, 0));
}

#pragma warning(disable:4312)
BOOL CTextBox::CreateEx(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd *pParentWnd, UINT nID) {
  BOOL ret = CWnd::CreateEx(0, _T("EDIT"), NULL
	                         ,dwStyle | WS_CHILD
	                         ,rect.left, rect.top
	                         ,rect.right - rect.left, rect.bottom - rect.top
	                         ,pParentWnd->GetSafeHwnd(), (HMENU)nID, NULL);
  if(ret) {
    ret = ModifyStyleEx(0, dwExStyle);
  }
  return ret;
}

static int findCharacterPosition(const TCHAR *s, const SourcePosition &pos) {
  int lineCount = 1;
  int column    = 0;
  int i;
  for(i = 0; *s; i++, s++) {
    if(lineCount == pos.getLineNumber() && column == pos.getColumn()) {
      break;
    }
    if(*s == '\n') {
      lineCount++;
      column = 0;
    } else {
      column++;
    }
  }
  return i;
}

static int findColumn(const TCHAR *s, SourcePosition &pos) {
  int index = findCharacterPosition(s, pos);
  const TCHAR *t;
  for(t = s + index; t >= s && *t != '\n';) {
    t--;
  }
  t++;
  int column = 0;
  for(s = t; s - t < pos.getColumn(); s++) {
    if(*s == '\t') {
      column += 8 - column%8;
    } else {
      column++;
    }
  }
  return column;
}

void CTextBox::markPos(SourcePosition *pos) {
  if(pos == NULL) {
    m_marked = false;
    draw(CClientDC(this));
  } else {
    m_marked = true;
    m_pos    = *pos;
    draw(CClientDC(this));
  }
}

void CTextBox::draw(CDC &dc) {
  if(m_marked) {
    TEXTMETRIC tm;
    CFont *font = GetFont();
    dc.SelectObject(font);
    dc.GetTextMetrics(&tm);
    const int xoffset = GetScrollPos(SB_HORZ);
    const int yoffset = GetScrollPos(SB_VERT);
    TCHAR *tmp = m_str.GetBuffer(m_str.GetLength());
    CPoint cp;
    cp.x = (findColumn(tmp, m_pos) - xoffset)    * tm.tmMaxCharWidth + 2;
    cp.y = (m_pos.getLineNumber() - yoffset - 1) * tm.tmHeight + 3;

    CRect rect;
    GetClientRect(&rect);
    if(rect.PtInRect(cp)) {
      CDC     tmpDC;
      CBitmap tmpBitmap;
      tmpBitmap.CreateCompatibleBitmap(&dc, tm.tmMaxCharWidth, tm.tmHeight);
      tmpDC.CreateCompatibleDC(&dc);

      CBitmap *oldBitmap = tmpDC.SelectObject(&tmpBitmap);
      CBrush  *oldBrush  = tmpDC.SelectObject(&m_redBrush);
      tmpDC.Rectangle(0, 0, tm.tmMaxCharWidth, tm.tmHeight);

      dc.BitBlt( cp.x, cp.y, tm.tmMaxCharWidth, tm.tmHeight, &tmpDC, 0, 0, SRCPAINT);

      tmpDC.SelectObject(oldBrush);
      tmpDC.SelectObject(oldBitmap);
      tmpDC.DeleteDC();
      tmpBitmap.DeleteObject();
    }
  }
}

void CTextBox::OnPaint() {
  CEdit::OnPaint();
  draw(CClientDC(this));
}

void CTextBox::DoDataExchange(CDataExchange *pDX) {
  DDX_Text(pDX, GetDlgCtrlID(), m_str);
}
