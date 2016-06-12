#include "stdafx.h"
#include <MyUtil.h>
#include <Tokenizer.h>
#include <MFCUTIL/WinTools.h>
#include <MFCUTIL/ColoredStatic.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColoredStatic::CColoredStatic() {
    m_textColor  = RGB(0,0,0);
    m_bkColorSet = false;
    //{{AFX_DATA_INIT(CColoredStatic)
	//}}AFX_DATA_INIT
}

CColoredStatic::~CColoredStatic() {
}

BEGIN_MESSAGE_MAP(CColoredStatic, CStatic)
    //{{AFX_MSG_MAP(CColoredStatic)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColoredStatic::OnPaint() {
  CPaintDC dc(this);
  repaint(dc);
}

void CColoredStatic::repaint(CDC &dc) {
  CFont *font = GetFont();
  if(font == NULL) {
    font = GetParent()->GetFont();
  }
  CFont *oldFont = dc.SelectObject(font);
  if(m_bkColorSet) {
    dc.SetBkColor(m_bkColor);
  }
  dc.SetTextColor(m_textColor);

  String text = getWindowText(this);
  StringArray lineArray(Tokenizer(text, "\n"));

  const int textAlign = GetStyle() & 0x3;
  int y = 0;
  switch(textAlign) {
  case SS_LEFT:
    { for(size_t i = 0; i < lineArray.size(); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
        dc.TextOut(0,y,line.cstr());
        y += lineSize.cy;
      }
    }
    break;
  case SS_RIGHT:
    { const CSize winSize = getWindowSize(this);
      for(size_t i = 0; i < lineArray.size(); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
        dc.TextOut(max(0, winSize.cx - lineSize.cx), y, line.cstr());
        y += lineSize.cy;
      }
    }
    break;
  case SS_CENTER:
    { const CSize winSize = getWindowSize(this);
      for(size_t i = 0; i < lineArray.size(); i++) {
        const String &line = lineArray[i];
        const CSize lineSize = getTextExtent(dc, line);
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
