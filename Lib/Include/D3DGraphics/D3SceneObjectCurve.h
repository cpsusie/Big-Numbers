#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectCurve : public D3SceneObjectWithVertexBuffer {
public:
  D3SceneObjectCurve(D3Scene &scene, const VertexArray &points);
  void draw(D3Device &device);
};
