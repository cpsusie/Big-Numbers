#pragma once

#include <TinyBitSet.h>

class D3Camera;

class CameraSet : public TinyBitSet<LPARAM> {
public:
  inline CameraSet() {
  }
  inline explicit CameraSet(LPARAM bits) : TinyBitSet(bits) {
  }
};

class D3CameraArray : public CompactArray<D3Camera*> {
public:
  // p in screen-coordinates
  // return index of the camera corresponding to the window clicked on, if p in the clientRect, or -1 if none found
  int       findCameraIndex(const CPoint &p) const;
  int       findCameraIndex(HWND hwnd) const;
  D3Camera *findCamera(     HWND hwnd) const;
  CameraSet getVisibleCameraSet() const;

};
