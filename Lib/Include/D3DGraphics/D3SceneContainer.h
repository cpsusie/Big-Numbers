#pragma once

#include <MFCUtil/WinTools.h>
#include "D3CameraArray.h"

class D3Scene;

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
  inline CSize     getWinSize(UINT index) const {
    return getClientRect(get3DWindow(index)).Size();
  }
};
