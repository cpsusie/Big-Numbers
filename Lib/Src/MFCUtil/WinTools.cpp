#include "pch.h"

CRect getRelativeClientRect(const CWnd *wnd, int id) {
  CWnd *subWin = wnd->GetDlgItem(id);
  if(subWin == NULL) return CRect(0,0,0,0);
  CRect wr,cr;
  subWin->GetWindowRect(&wr);
  wnd->ScreenToClient(wr); // wr is now subwin.rect relative to wnd
  subWin->GetClientRect(&cr);
  const int dw = (wr.Width()  - cr.Width() ) / 2;
  const int dh = (wr.Height() - cr.Height()) / 2;
  // calculate cr as the rectangle with the same dimension as clientRect centered in wr
  return CRect(wr.left+dw, wr.top+dh, wr.right-dw, wr.bottom-dh);
}

CRect getClientRect(const CWnd *wnd, int id) {
  CWnd *child = wnd->GetDlgItem(id);
  if(child == NULL) return CRect(0,0,0,0);
  CRect r;
  child->GetClientRect(&r);
  return r;
}

CRect getClientRect(const CWnd *wnd) {
  if(wnd == NULL) return CRect(0,0,0,0);
  CRect r;
  wnd->GetClientRect(&r);
  return r;
}

CRect getClientRect(HWND wnd) {
  if(wnd == NULL) return CRect(0,0,0,0);
  CRect r;
  GetClientRect(wnd, &r);
  return r;
}

CRect getRelativeWindowRect(const CWnd *wnd, int id) {
  return getWindowRect(wnd->GetDlgItem(id));
}

bool setRelativeWindowRect(CWnd *wnd, int id, const CRect &rect) {
  return setWindowRect(wnd->GetDlgItem(id), rect);
}

CRect getWindowRect(const CWnd *wnd) {
  if(wnd == NULL) return CRect(0,0,0,0);
  WINDOWPLACEMENT wp;
  wnd->GetWindowPlacement(&wp);
  return wp.rcNormalPosition;
}

CRect getWindowRect(HWND wnd) {
  if(wnd == NULL) return CRect(0,0,0,0);
  CRect rect;
  ::GetWindowRect(wnd, &rect);
  return rect;
}

CRect getWindowRect(const CWnd *wnd, int id) {
  return getWindowRect(wnd->GetDlgItem(id));
}

bool setWindowRect(CWnd *wnd, const CRect &rect) {
  if(wnd == NULL) return false;
  WINDOWPLACEMENT wp;
  wnd->GetWindowPlacement(&wp);
  wp.rcNormalPosition = rect;
  return wnd->SetWindowPlacement(&wp) ? true : false;
//  return wnd->SetWindowPos(NULL,rect.left,rect.top,rect.Width(), rect.Height(), SWP_NOZORDER |SWP_DRAWFRAME|SWP_NOCOPYBITS) ? true : false;
}

bool setWindowRect(CWnd *wnd, int id, const CRect &rect) {
  return setWindowRect(wnd->GetDlgItem(id), rect);
}

bool setClientRectSize(CWnd *wnd, const CSize &size) {
  if(wnd == NULL) return false;
  const CSize csz = getClientRect(wnd).Size();
  const CSize wsz = getWindowSize(wnd);

  const int dw = size.cx - csz.cx;
  const int dh = size.cy - csz.cy;

  return setWindowSize(wnd, CSize(wsz.cx + dw, wsz.cy + dh));
/*
  WINDOWPLACEMENT wpl;
  wnd->GetWindowPlacement(&wpl);
  wpl.rcNormalPosition.right  = max(wpl.rcNormalPosition.left, wpl.rcNormalPosition.right  + dw);
  wpl.rcNormalPosition.bottom = max(wpl.rcNormalPosition.top , wpl.rcNormalPosition.bottom + dh);
  wnd->SetWindowPlacement(&wpl);
*/
}

bool setClientRectSize(CWnd *wnd, int id, const CSize &size) {
  return setClientRectSize(wnd->GetDlgItem(id), size);
}

bool setWindowSize(CWnd *wnd, int id, const CSize &size) {
  return setWindowSize(wnd->GetDlgItem(id), size);
}

bool setWindowSize(CWnd *wnd, const CSize &size) {
  if(wnd == NULL) return false;
  WINDOWPLACEMENT wpl;
  if(!wnd->GetWindowPlacement(&wpl)) return false;
  wpl.rcNormalPosition.right  = wpl.rcNormalPosition.left + size.cx;
  wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top  + size.cy;
  return wnd->SetWindowPlacement(&wpl) ? true : false;
}

CSize getWindowSize(const CWnd *wnd, int id) {
  return getWindowSize(wnd->GetDlgItem(id));
}

CSize getWindowSize(const CWnd *wnd) {
  return getWindowRect(wnd).Size();
}

CSize getWindowSize(HWND hwnd) {
  if(hwnd == NULL) return CSize(0,0);
  return getWindowRect(hwnd).Size();
}

CRect getTaskBarRect() {
  HWND taskBar = FindWindow(_T("Shell_traywnd"),NULL);
  if(taskBar == NULL) {
    return CRect(0,0,0,0);
  } else {
    CRect taskBarRect;
    GetWindowRect(taskBar, &taskBarRect);
    return taskBarRect;
  }
}

CRect getScreenRect() { // the visible rectangle NOT overlapping the taskbar 
  const CSize screenSize  = getScreenSize();
  const CRect taskBarRect = getTaskBarRect();
  if(taskBarRect.Width() == 0 && taskBarRect.Height() == 0) {
    return CRect(0,0,screenSize.cx,screenSize.cy);
  } else {
    if(taskBarRect.Width() == screenSize.cx) { // taskbar is at the top or bottom
      if(taskBarRect.top == 0) { // top
        return CRect(0,taskBarRect.bottom,screenSize.cx,screenSize.cy);
      } else {                   // bottom
        return CRect(0,0,screenSize.cx,taskBarRect.top);
      }
    } else {
      if(taskBarRect.left == 0) { // left
        return CRect(taskBarRect.right,0,screenSize.cx, screenSize.cy);
      } else {                    // right
        return CRect(0,0,taskBarRect.left,screenSize.cy);
      }
    }
  }
}

bool centerWindow(CWnd *wnd) {
  CWnd *parent = wnd->GetParent();
  const CSize windowSize = getWindowSize(wnd);

  CPoint p0;
  if(parent == NULL) {
    const CSize screenSize = getScreenRect().Size();
    p0 = CPoint((screenSize.cx-windowSize.cx)/2,(screenSize.cy-windowSize.cy)/2);
  } else {
    const CRect parentRect = getWindowRect(parent);
    const CSize parentSize = parentRect.Size();
    p0 = CPoint(parentRect.left + (parentSize.cx-windowSize.cx)/2,parentRect.top + (parentSize.cy-windowSize.cy)/2);
  }

  p0.x = max(0, p0.x);
  p0.y = max(0, p0.y);

  return setWindowPosition(wnd, p0);
}

void putWindowBesideWindow(CWnd *wnd, CWnd *otherWindow) {
  const CRect opr        = getWindowRect(otherWindow);
  const CSize screenSize = getScreenSize(false);
  const CSize sz         = getWindowRect(wnd).Size();
  int left, right, top, bottom;

  if(sz.cy < opr.Size().cy) {
    bottom   = opr.bottom;
    top      = bottom - sz.cy;
    if(top < 0) {
      bottom -= top;
      top    = 0;
    }
  } else {
    top    = opr.top;
    bottom = top + sz.cy;
    if(bottom > screenSize.cy-8) {
      bottom = screenSize.cy-8;
      top    = bottom - sz.cy;
    }
  }
  const int spaceRightFor = screenSize.cx - opr.right;
  const int spaceLeftFor  = opr.left;
  if((spaceRightFor >= sz.cx) || (opr.left < sz.cx/2)) {
    left  = opr.right + 4;
    right = left + sz.cx;
  } else if(opr.left > sz.cx) {
    right = opr.left - 4;
    left  = right - sz.cx;
  } else if(spaceLeftFor > spaceRightFor) { // put it leftmost
    left  = 0;
    right = sz.cx;
  } else {
    right = screenSize.cx-1;
    left  = right - sz.cx;
  }
  const CRect wr(left,top, right, bottom);
  setWindowRect(wnd, wr);
}

CSize getScreenSize(bool includeTaskBar) {
  HDC hdcScreen = getScreenDC();
  CSize size    = getDCSizeInPixels(hdcScreen);
  DeleteDC(hdcScreen);

  if(!includeTaskBar) {
    CSize taskBarSize = getTaskBarRect().Size();
    if(taskBarSize.cx == size.cx) {
      size.cy -= taskBarSize.cy;
    } else {
      size.cx -= taskBarSize.cx;
    }
  }

  return size;
}

HDC getScreenDC() {
  HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
  if(screenDC == NULL) {
    throwLastErrorOnSysCallException(_T("CreateDC"));
  }
  return screenDC;
};

int getScreenPlanes() {
  static int planes = -1;
  if(planes < 0) {
    HDC screenDC = getScreenDC();
    planes = GetDeviceCaps(screenDC, PLANES);
    DeleteDC(screenDC);
  }
  return planes;
}

int getScreenBitsPixel() {
  static int bitsPixel = -1;
  if(bitsPixel < 0) {
    HDC screenDC = getScreenDC();
    bitsPixel = GetDeviceCaps(screenDC, BITSPIXEL);
    DeleteDC(screenDC);
  }
  return bitsPixel;
}


CSize getScreenSizeInMillimeters() {
  HDC screenDC     = getScreenDC();
  const CSize size = getDCSizeInMillimeters(screenDC);
  DeleteDC(screenDC);
  return size;
}

CPoint getWindowPosition(const CWnd *wnd, int id) {
  return getWindowPosition(wnd->GetDlgItem(id));
}

bool setWindowPosition(CWnd *wnd, int id, const CPoint &point) {
  return setWindowPosition(wnd->GetDlgItem(id), point);
}

CPoint getWindowPosition(const CWnd *wnd) {
  WINDOWPLACEMENT wpl;
  wnd->GetWindowPlacement(&wpl);
  return CPoint(wpl.rcNormalPosition.left, wpl.rcNormalPosition.top);
}

bool setWindowPosition(CWnd *wnd, const CPoint &point) {
  WINDOWPLACEMENT wpl;
  if(!wnd->GetWindowPlacement(&wpl)) return false;
  int dx = point.x - wpl.rcNormalPosition.left;
  int dy = point.y - wpl.rcNormalPosition.top;
  wpl.rcNormalPosition.left   += dx;
  wpl.rcNormalPosition.right  += dx;
  wpl.rcNormalPosition.top    += dy;
  wpl.rcNormalPosition.bottom += dy;
  return wnd->SetWindowPlacement(&wpl) ? true : false;
}

int getFocusCtrlId(const CWnd *wnd) {
  CWnd *ctrl = wnd->GetFocus();
  return ctrl ? ctrl->GetDlgCtrlID() : -1;
}

void gotoEditBox(CWnd *wnd, int id) {
  CEdit *e = (CEdit*)(wnd->GetDlgItem(id));
  e->SetFocus();
  CString s;
  e->GetWindowText(s);
  e->SetSel(0,s.GetLength());
}

CSize getDCSizeInPixels(HDC hdc) {
  const int w = GetDeviceCaps(hdc,HORZRES);
  const int h = GetDeviceCaps(hdc,VERTRES);
  return CSize(w,h);
}

CSize getDCSizeInMillimeters(HDC hdc) {
  return pixelsToMillimeters(hdc,getDCSizeInPixels(hdc));
}

#pragma warning(disable : 4244)

CSize pixelsToMillimeters(HDC hdc, const CSize &size) { // convert size in pixels -> size in millimeters
  const int xPixPerInch = GetDeviceCaps(hdc,LOGPIXELSX); // pixels / inch
  const int yPixPerInch = GetDeviceCaps(hdc,LOGPIXELSY); // pixels / inch

  return CSize((size.cx * 25.4) / xPixPerInch, (size.cy * 25.4) / yPixPerInch);
}

CRect makePositiveRect(const CRect &r) {
  CRect result = r;
  if (result.Width() < 0) {
    std::swap(result.left, result.right);
  }
  if (result.Height() < 0) {
    std::swap(result.top, result.bottom);
  }
  return result;
}

