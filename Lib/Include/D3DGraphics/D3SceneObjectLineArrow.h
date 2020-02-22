#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3Device;

class D3SceneObjectLineArrow : public D3SceneObjectWithVertexBuffer {
private:
  void init(const Vertex &from, const Vertex &to);
public:
  D3SceneObjectLineArrow(D3Scene             &scene , const Vertex &from, const Vertex &to, const String &name = _T("LineArrow"));
  D3SceneObjectLineArrow(D3SceneObjectVisual *parent, const Vertex &from, const Vertex &to, const String &name = _T("LineArrow"));
  void draw();
};
