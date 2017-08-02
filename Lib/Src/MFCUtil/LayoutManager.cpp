#include "pch.h"

#pragma warning(disable : 4244)

DEFINECLASSNAME(LayoutManager);

LayoutManager::LayoutManager() : m_wnd(NULL) {
  m_currentFontScale = 1;
  m_font             = NULL;
}

LayoutManager::~LayoutManager() {
  setFont(NULL);
}

void LayoutManager::setFont(CFont *font) {
  if(m_font && (m_font != m_startFont)) {
    m_font->DeleteObject();
    SAFEDELETE(m_font);
  }
  m_font = font;
}

#define BOTHSET(flags, f1,f2) (((flags) & ((f1)|(f2))) == ((f1)|(f2)))

void LayoutManager::OnInitDialog(CWnd *wnd, int flags) {
  if(wnd == NULL) {
    AfxMessageBox(format(_T("%s::OnInitDialog:wnd==NULL"), s_className).cstr(), MB_ICONSTOP);
  }

  if(BOTHSET(flags, FONT_RELATIVE_SIZE, RETAIN_ASPECTRATIO)) {
    AfxMessageBox(format(_T("%s:Invalid bit-combination for window flags:%s"), s_className, sprintbin((short)flags).cstr()).cstr(), MB_ICONSTOP);
    return;
  }

  m_wnd       = wnd;
  m_flags     = flags;
  resetWinStartSize();
  m_startFont = m_wnd->GetFont();
  m_font      = m_startFont;
}

void LayoutManager::resetWinStartSize() {
  m_winStartSize = getClientRect(m_wnd).Size();
  m_borderSize = getWindowSize(m_wnd) - m_winStartSize;
}

CSize LayoutManager::findScaledClientSize(const CSize &size, bool isCorner) const {
  const CSize oldSize = getClientRect(m_wnd).Size();
  const CSize &size0  = m_winStartSize;
  double scale;
  if(isCorner || (size.cy == oldSize.cy)) {
    scale = (double)size.cx / size0.cx;
  } else {
    scale = (double)size.cy / size0.cy;
  }
  return CSize(round(size0.cx * scale), round(size0.cy * scale));
}

static inline bool isCorner(UINT fwSide) {
  return (fwSide == WMSZ_TOPLEFT) || (fwSide == WMSZ_TOPRIGHT) || (fwSide == WMSZ_BOTTOMLEFT) || (fwSide == WMSZ_BOTTOMRIGHT);
}

void LayoutManager::OnSizing(UINT fwSide, LPRECT pRect) {
  if(!isInitialized()) return;
  if(m_flags & RETAIN_ASPECTRATIO) {
    const CSize oldClientSize = getClientRect(m_wnd).Size();
    const CSize clientSize    = CSize(pRect->right - pRect->left, pRect->bottom - pRect->top) - m_borderSize;
    const CSize newClientSize = findScaledClientSize(clientSize, isCorner(fwSide));
    const CSize newWinSize    = newClientSize + m_borderSize;
    const CSize screenSize    = getScreenSize(true);
    const int   dw            = newClientSize.cx - oldClientSize.cx;
    const int   dh            = newClientSize.cy - oldClientSize.cy;

#define SETLEFT      { pRect->left   = pRect->right  - newWinSize.cx; }
#define SETRIGHT     { pRect->right  = pRect->left   + newWinSize.cx; }
#define SETTOP       { pRect->top    = pRect->bottom - newWinSize.cy; }
#define SETBOTTOM    { pRect->bottom = pRect->top    + newWinSize.cy; }
#define ADJUSTWIDTH  { if((dw>=0)&&(pRect->right  + dw<=screenSize.cx) || (dw<0)&&(pRect->left>=0)) SETRIGHT  else SETLEFT }
#define ADJUSTHEIGHT { if((dh>=0)&&(pRect->bottom + dh<=screenSize.cy) || (dh<0)&&(pRect->top >=0)) SETBOTTOM else SETTOP  }

    switch(fwSide) {
    case WMSZ_LEFT       : SETLEFT;   ADJUSTHEIGHT; break;
    case WMSZ_RIGHT      : SETRIGHT;  ADJUSTHEIGHT; break;
    case WMSZ_TOP        : SETTOP;    ADJUSTWIDTH;  break;
    case WMSZ_BOTTOM     : SETBOTTOM; ADJUSTWIDTH;  break;
    case WMSZ_TOPLEFT    : SETTOP;    SETLEFT;      break;
    case WMSZ_TOPRIGHT   : SETTOP;    SETRIGHT;     break;
    case WMSZ_BOTTOMLEFT : SETBOTTOM; SETLEFT;      break;
    case WMSZ_BOTTOMRIGHT: SETBOTTOM; SETRIGHT;     break;
    }
  }
}

void LayoutManager::setFontScale(double scale, bool resizeWindow, bool redraw) {
  checkIsInitialized();
  if(scale <= 0) {
    return;
  }
  if(scale == 1) {
    setFont(m_startFont);
  } else {
    setFont(createScaledFont(*m_startFont, scale));
  }
  m_currentFontScale = scale;
  if(resizeWindow) {
    setClientRectSize(getWindow(), m_winStartSize*scale);
  }
}

void LayoutManager::checkIsInitialized() const {
  if(!isInitialized()) {
    AfxMessageBox(format(_T("%s not initialized. Call OnInitDialog"), s_className).cstr(), MB_ICONSTOP);
  }
}

CWnd *LayoutManager::getWindow() {
  checkIsInitialized();
  return m_wnd;
}

const CWnd *LayoutManager::getWindow() const {
  checkIsInitialized();
  return m_wnd;
}

CWnd *LayoutManager::getChild(int ctrlId) {
  return getWindow()->GetDlgItem(ctrlId);
}

const CWnd *LayoutManager::getChild(int ctrlId) const {
  return getWindow()->GetDlgItem(ctrlId);
}

CFont *LayoutManager::createScaledFont(const CFont &src, double scale) { // static
  LOGFONT lf;
  ((CFont&)src).GetLogFont(&lf);
  lf.lfHeight = (int)(scale * lf.lfHeight);
  CFont *nf = new CFont(); TRACE_NEW(nf);
  nf->CreateFontIndirect(&lf);
  return nf;
}
