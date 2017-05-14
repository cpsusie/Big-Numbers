#include "pch.h"

void notifyIconAdd(HWND hwnd, UINT uID, HICON hicon, const String &toolTip, int callbackMessage) {
  NOTIFYICONDATA tnid;
  tnid.cbSize           = sizeof(NOTIFYICONDATA);
  tnid.hWnd             = hwnd;
  tnid.uID              = uID;
  tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  tnid.uCallbackMessage = callbackMessage;
  tnid.hIcon            = hicon;
  lstrcpyn(tnid.szTip, toolTip.cstr(), min((int)toolTip.length()+1, sizeof(tnid.szTip)));

  if(!Shell_NotifyIcon(NIM_ADD, &tnid)) {
    throwException(_T("%s:Shell_NotifyIcon NIM_ADD failed"), __TFUNCTION__);
  }
}

void notifyIconDelete(HWND hwnd, UINT uID) {
  NOTIFYICONDATA tnid;
  memset(&tnid, 0, sizeof(tnid));
//    tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID  = uID;
  if(!Shell_NotifyIcon(NIM_DELETE, &tnid)) {
    throwException(_T("%s:Shell_NotifyIcon NIM_DELETE failed"), __TFUNCTION__);
  }
}

void notifyIconSetToolTip(HWND hwnd, UINT uID, const String &toolTip) {
  NOTIFYICONDATA tnid;
  memset(&tnid, 0, sizeof(tnid));
  tnid.cbSize           = sizeof(NOTIFYICONDATA);
  tnid.hWnd             = hwnd;
  tnid.uID              = uID;
  tnid.uFlags           = NIF_TIP;
  lstrcpyn(tnid.szTip, toolTip.cstr(), min((int)toolTip.length()+1, sizeof(tnid.szTip)));
  if(!Shell_NotifyIcon(NIM_MODIFY, &tnid)) {
    throwException(_T("%s:Shell_NotifyIcon NIM_MODIFY failed"), __TFUNCTION__);
  }
}
