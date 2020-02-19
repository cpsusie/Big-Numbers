#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3Device;

class D3SceneObjectLineArrow : public D3SceneObjectWithVertexBuffer {
public:
  D3SceneObjectLineArrow(D3Scene &scene, const Vertex &from, const Vertex &to);
  void draw(D3Device &device);
};
