#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectLineArray : public D3SceneObjectWithVertexBuffer {
protected:
  void initBuffer(const Line3D *lines, UINT n);
public:
  D3SceneObjectLineArray(D3Scene &scene) : D3SceneObjectWithVertexBuffer(scene) {
  }
  D3SceneObjectLineArray(D3Scene &scene, const Line3D *lines, UINT n);
  D3SceneObjectLineArray(D3Scene &scene, const CompactArray<Line3D> &lineArray);
  void draw(D3Device &device);
};
