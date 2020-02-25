#pragma once

#include <MFCUtil/WinTools.h>
#include "D3CameraArray.h"

class D3Scene;

class WindowPair {
private:
  HWND m_hwnd[2];
public:
  WindowPair() {
    clear();
  }
  WindowPair(HWND w1, HWND w2) {
    m_hwnd[0] = w1;
    m_hwnd[1] = w2;
  }
  inline void clear() {
    m_hwnd[0] = (HWND)INVALID_HANDLE_VALUE;
    m_hwnd[1] = (HWND)INVALID_HANDLE_VALUE;
  }
  inline bool isEmpty() const {
    return !IsWindow(m_hwnd[0]);
  }
  inline HWND getWindow(UINT index) const {
    return m_hwnd[index];
  }
};
class D3SceneContainer {
public:
  virtual D3Scene &getScene() = 0;
  virtual UINT     get3DWindowCount() const = 0;
  virtual HWND     getMessageWindow() const = 0;
  virtual HWND     get3DWindow(UINT index) const = 0;
  // renderFlags is any combination of RENDER_3D,RENDER_INFO
  virtual void     render(BYTE renderFlags, CameraSet cameraSet) = 0;
  virtual void     modifyContextMenu(HMENU menu) {
  }
  virtual bool     canSplit3DWindow(HWND hwnd) const {
    return false;
  }
  virtual WindowPair createNew3DWindow(HWND current, bool vertical) {
    return WindowPair();
  }
  inline CSize     getWinSize(UINT index) const {
    return getClientRect(get3DWindow(index)).Size();
  }
};
