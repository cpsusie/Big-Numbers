#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectCurve : public D3SceneObjectWithVertexBuffer {
private:
  void init(const VertexArray &points);
public:
  D3SceneObjectCurve(D3Scene             &scene , const VertexArray &points, const String &name = _T("Curve"));
  D3SceneObjectCurve(D3SceneObjectVisual *parent, const VertexArray &points, const String &name = _T("Curve"));

  void draw();
};
