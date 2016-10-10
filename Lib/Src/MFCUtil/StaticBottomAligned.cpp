#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/StaticBottomAligned.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStaticBottomAligned::CStaticBottomAligned() {
}

CStaticBottomAligned::~CStaticBottomAligned() {
}

BEGIN_MESSAGE_MAP(CStaticBottomAligned, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CStaticBottomAligned::substituteControl(CWnd *parent, int id) {
  CStatic *oldCtrl = (CStatic*)parent->GetDlgItem(id);
  if(oldCtrl == NULL) {
    AfxMessageBox(format(_T("CStaticBottomAligned::substituteControl:Control with id=%d does not exist"), id).cstr(), MB_ICONWARNING);
    return;
  }
  oldCtrl->GetWindowText(m_text);
  CRect wr;
  oldCtrl->GetWindowRect(&wr);
  parent->ScreenToClient(&wr);
  const DWORD style   = oldCtrl->GetStyle();
  const DWORD exStyle = oldCtrl->GetExStyle();

  CFont *font = oldCtrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  LOGFONT lf;
  font->GetLogFont(&lf);
  m_lineHeight = abs(lf.lfHeight);

  oldCtrl->DestroyWindow();

  if(!Create(m_text, style, wr, parent, id)) {
    AfxMessageBox(_T("CStaticBottomAligned::substituteControl:Create failed"), MB_ICONWARNING);
    return;
  }
  SetFont(font);
  ModifyStyleEx(0, exStyle);
}

void CStaticBottomAligned::OnPaint() {
  CPaintDC dc(this);
  repaint(dc);  
}

void CStaticBottomAligned::repaint(CDC &dc) {
  CRect cr;
  GetClientRect(&cr);
  const CSize clientSize = cr.Size();
  CFont      *font       = GetFont();

  StringArray    lineArray(Tokenizer(m_text, _T("\n")));
  const COLORREF bkcolor  = ::GetSysColor(COLOR_BTNFACE);
  CFont         *oldFont  = dc.SelectObject(font);
  const CSize    textSize = getTextExtent1(dc, lineArray);
  int            topPos   = clientSize.cy - textSize.cy;
  CRgn           rgn;

  rgn.CreateRectRgnIndirect(&cr);
  dc.SelectClipRgn(&rgn);
  dc.FillSolidRect(&cr, bkcolor);

  if(topPos < 0) {
    topPos = 0;
  }
  dc.SetBkColor(bkcolor);

  const int textAlign = GetStyle() & 0x3;
  int y = topPos;
  switch(textAlign) {
  case SS_LEFT:
    { for(size_t i = 0; (i < lineArray.size()) && (y < clientSize.cy); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
        dc.TextOut(0, y, line.cstr());
        y += m_lineHeight = lineSize.cy;
      }
    }
    break;
  case SS_RIGHT:
    { for(size_t i = 0; (i < lineArray.size()) && (y < clientSize.cy); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
        dc.TextOut(max(0, clientSize.cx - lineSize.cx), y, line.cstr());
        y += m_lineHeight = lineSize.cy;
      }
    }
    break;
  case SS_CENTER:
    { for(size_t i = 0; (i < lineArray.size()) && (y < clientSize.cy); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
        dc.TextOut(max(0, (clientSize.cx - lineSize.cx)/2), y, line.cstr());
        y += m_lineHeight = lineSize.cy;
      }
    }
    break;
  }
  dc.SelectObject(oldFont);
}

void CStaticBottomAligned::SetWindowText(LPCTSTR string) {
  m_text = string;
  if(IsWindow(m_hWnd)) {
    repaint(CClientDC(this));
  }
}
