#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/GifCtrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGifCtrl::CGifCtrl() {
  m_isPainted    = false;
  m_suspended    = false;
  m_paintedIndex = -1;
    //{{AFX_DATA_INIT(CGifCtrl)
	//}}AFX_DATA_INIT
}

CGifCtrl::~CGifCtrl() {
}

BEGIN_MESSAGE_MAP(CGifCtrl, CStatic)
  ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()

DEFINECLASSNAME(CGifCtrl);

void CGifCtrl::substituteControl(CWnd *wnd, int id) {
  CWnd *ctrl = wnd->GetDlgItem(id);
  if(ctrl == NULL) {
    wnd->MessageBox(format(_T("%s::%s:Control %d not found"), s_className, _T(__FUNCTION__), id).cstr(), _T("Error"), MB_ICONWARNING);
    return;
  }
  const DWORD  style   = ctrl->GetStyle();
  const DWORD  exStyle = ctrl->GetExStyle();
  const CRect  rect    = getWindowRect(ctrl);
  const String str     = getWindowText(ctrl);
  ctrl->DestroyWindow();
  if(!Create(str.cstr(), style, rect, wnd, id)) {
    wnd->MessageBox(format(_T("%s::%s:Create failed"), s_className, _T(__FUNCTION__)).cstr(), _T("Error"), MB_ICONWARNING);
    return;
  }
  ModifyStyleEx(0,exStyle);
}

void CGifCtrl::OnPaint() {
  CWnd::OnPaint();
    
  if(isSuspended()) {
    resume();
  } else if(isPainted()) {
    if(m_paintedIndex == -1) {
      m_image.paintAllFrames(getClientRect(this));
    } else {
      m_image.paintWork(CClientDC(this), ORIGIN);
    }
  }
}

void CGifCtrl::OnShowWindow(BOOL bShow, UINT nStatus) {
  CStatic::OnShowWindow(bShow, nStatus);
	
  if(bShow) {
    if(isSuspended()) {
      resume();
    }
  } else {
    if(isPlaying()) {
      suspend();
    }
  }
}

void CGifCtrl::suspend() {
  stop();
  m_suspended = true;
}

void CGifCtrl::resume() {
  m_suspended = false;
  play(true);
}

void CGifCtrl::OnDestroy() {
  if(isPlaying()) {
    m_image.stopAnimation();
  }
  unload();
  CStatic::OnDestroy();
}

void CGifCtrl::load(const String &fileName) {
  if(isLoaded()) {
    hide();
  }
  m_image.load(this, fileName);
}

void CGifCtrl::loadFromResource(int resId, const String &typeName) {
  if(isLoaded()) {
    hide();
  }
  m_image.loadFromResource(this, resId, typeName);
}

void CGifCtrl::loadFromGifFile(const GifFileType *gifFile) {
  if(isLoaded()) {
    hide();
  }
  m_image.createFromGifFile(this, gifFile);
}

void CGifCtrl::play(bool force) {
  if(isLoaded() && !isPlaying()) {
    if(force || IsWindowVisible()) {
      hide();
      m_image.startAnimation(ORIGIN);
    } else {
      m_suspended = true;
    }
  }
}

void CGifCtrl::stop() {
  m_suspended = false;
  if(isPlaying()) {
    m_image.stopAnimation();
  }
}

void CGifCtrl::unload() {
  hide();
  m_image.unload();
}

void CGifCtrl::hide() {
  if(isPlaying() || isPainted() || isSuspended()) {
    m_image.hide();
    m_isPainted    = false;
    m_suspended    = false;
    m_paintedIndex = -1;
  }
}

#define CANPAINT() (isLoaded() && !isPlaying() && !isSuspended())
void CGifCtrl::paintFrame(unsigned int index) {
  if(CANPAINT() && index < (unsigned int)m_image.getFrameCount()) {
    m_isPainted    = true;
    m_suspended    = false;
    m_paintedIndex = index;
    m_image.paintFrames(ORIGIN, index);
    m_image.paintWork(CClientDC(this), ORIGIN);
  }
}

void CGifCtrl::paintAllFrames() {
  if(CANPAINT()) {
    m_isPainted    = true;
    m_suspended    = false;
    m_paintedIndex = -1;
    Invalidate();
  }
}

void CGifCtrl::OnSize(UINT nType, int cx, int cy) {
  if((cx == 0) || (cy == 0)) {
    if(isPlaying()) {
      suspend();
    }
  }
  CStatic::OnSize(nType, cx, cy);
}
