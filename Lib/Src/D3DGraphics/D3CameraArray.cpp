#include "pch.h"
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3CameraArray.h>

// p in screen-coordinates
int D3CameraArray::findCameraIndex(const CPoint &p) const {
  const size_t n = size();
  for(int i = 0; i < n; i++) {
    if((*this)[i]->ptInRect(p)) {
      return i;
    }
  }
  return -1;
}

int D3CameraArray::findCameraIndex(HWND hwnd) const {
  const int n = (int)size();
  for(int i = 0; i < n; i++) {
    if((*this)[i]->getHwnd() == hwnd) {
      return i;
    }
  }
  return -1;
}

D3Camera *D3CameraArray::findCamera(HWND hwnd) const {
  const int index = findCameraIndex(hwnd);
  return (index < 0) ? NULL : (*this)[index];
}

CameraSet D3CameraArray::getVisibleCameraSet() const {
  CameraSet result;
  const UINT n = (UINT)size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i]->isVisible()) {
      result.add(i);
    }
  }
  return result;
}
