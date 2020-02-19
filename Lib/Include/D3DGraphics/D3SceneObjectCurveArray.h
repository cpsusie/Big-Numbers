#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectCurveArray : public D3SceneObjectWithVertexBuffer {
private:
  CompactIntArray m_curveSize;
public:
  D3SceneObjectCurveArray(D3Scene &scene, const CurveArray &curves);
  void draw();
};
