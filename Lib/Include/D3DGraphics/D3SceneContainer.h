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

#define SC_RENDER3D           0x01
#define SC_RENDERINFO         0x02
#define SC_RENDERNOW          0x04
#define SC_RENDERALL          (SC_RENDER3D|SC_RENDERINFO)
#define SC_RENDERUSER(attr)   ((attr)<<3)
#define SC_RENDERUSERMASK     0xf8

class D3SceneContainer {
private:
  BYTE          m_renderLevel;
  CameraSet     m_accCameraSet;
  BYTE          m_accRenderFlags;
  FastSemaphore m_lock;
  // no lock
  void renderAccumulatedAndReset();
protected:
  // no lock
  void logRenderCall(BYTE flags, CameraSet cameraSet);
public:
  D3SceneContainer() : m_renderLevel(0), m_accRenderFlags(0) {
  }
  virtual D3Scene   &getScene() = 0;
  // Should return how many 3D-windows (cameras) to create
  virtual UINT       get3DWindowCount() const = 0;
  // Should return the window to receieve messages (from contextmenu)
  virtual HWND       getMessageWindow() const = 0;
  // Assume index = [0..get3DWindowCount()-1].
  // Should return the 3D-window with the given indexed
  virtual HWND       get3DWindow(UINT index) const = 0;
  virtual void       modifyContextMenu(HMENU menu) {
  }
  virtual bool       canSplit3DWindow(HWND hwnd) const {
    return false;
  }
  virtual WindowPair split3DWindow(HWND current, bool vertical) {
    return WindowPair();
  }
  virtual bool       canDelete3DWindow(HWND hwnd) const {
    return false;
  }
  virtual bool       delete3DWindow(HWND hwnd) {
    return false;
  }
  inline CSize       getWinSize(UINT index) const {
    return getClientRect(get3DWindow(index)).Size();
  }
  // incrLevel/decrLevel works in stack-like manner. All calls to render(...) will be accumulated
  // in (m_accCamraSet,m_accRenderFlags) as long as m_renderLevel > 0.
  // When m_renderLevel is decremented to 0, doRender(m_accRenderFlags,m_accCameraSet) is called (like flushing)
  // and m_accRenderFlags,m_accCameraSet are both reset, to begin a new cycle
  // NB:If a call to render with bit SC_RENDERNOW set, doRender is called immediately with the same arguments
  // leaving m_accCameraSet,m_accRenderFlags unchanged
  // with lock
  void               incrLevel();
  // with lock
  void               decrLevel();
  // renderFlags is any combination of SC_RENDER*
  // with lock
  void               render(  BYTE renderFlags, CameraSet cameraSet);
  // Default behavior: if(renderFlags & SC_RENDER3D) getScene().render(cameraSet);
  // no lock
  virtual void       doRender(BYTE renderFlags, CameraSet cameraSet);
};
