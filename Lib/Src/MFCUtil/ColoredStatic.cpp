#include "pch.h"
#include <Tokenizer.h>
#include <MFCUTIL/ColoredStatic.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CColoredStatic::CColoredStatic() {
    m_textColor  = RGB(0,0,0);
    m_bkColorSet = false;
}

CColoredStatic::~CColoredStatic() {
}

BEGIN_MESSAGE_MAP(CColoredStatic, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CColoredStatic::OnPaint() {
  CPaintDC dc(this);
  repaint(dc);
}

void CColoredStatic::repaint(CDC &dc) {
  CFont *font = GetFont();
  if(font == nullptr) {
    font = GetParent()->GetFont();
  }
  CFont *oldFont = dc.SelectObject(font);
  if(m_bkColorSet) {
    dc.SetBkColor(m_bkColor);
  }
  dc.SetTextColor(m_textColor);

  String      text = getWindowText(this);
  StringArray lineArray(Tokenizer(text, "\n"));

  const int textAlign = GetStyle() & 0x3;
  int y = 0;
  switch(textAlign) {
  case SS_LEFT:
    { for(auto it = lineArray.getIterator(); it.hasNext();) {
        const String &line     = it.next();
        const CSize   lineSize = getTextExtent(dc, line);
        dc.TextOut(0,y,line.cstr());
        y += lineSize.cy;
      }
    }
    break;
  case SS_RIGHT:
    { const CSize winSize = getWindowSize(this);
      for(auto it = lineArray.getIterator(); it.hasNext();) {
        const String &line     = it.next();
        const CSize   lineSize = getTextExtent(dc, line);
        dc.TextOut(max(0, winSize.cx - lineSize.cx), y, line.cstr());
        y += lineSize.cy;
      }
    }
    break;
  case SS_CENTER:
    { const CSize winSize = getWindowSize(this);
      for(auto it = lineArray.getIterator(); it.hasNext();) {
        const String &line     = it.next();
        const CSize   lineSize = getTextExtent(dc, line);
        dc.TextOut(max(0, (winSize.cx - lineSize.cx)/2), y, line.cstr());
        y += lineSize.cy;
      }
    }
    break;
  }
  dc.SelectObject(oldFont);
}

void CColoredStatic::setBKColor(COLORREF color) {
  m_bkColor    = color;
  m_bkColorSet = true;
  if(IsWindow(m_hWnd)) {
    repaint(CClientDC(this));
  }
}

void CColoredStatic::setTextColor(COLORREF color) {
  if(color != m_textColor) {
    m_textColor = color;
    if(IsWindow(m_hWnd)) {
      repaint(CClientDC(this));
    }
  }
}
