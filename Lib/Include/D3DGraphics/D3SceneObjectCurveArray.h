#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectCurveArray : public D3SceneObjectWithVertexBuffer {
private:
  CompactIntArray m_curveSize;
  void init(const CurveArray &curves);
public:
  D3SceneObjectCurveArray(D3Scene             &scene , const CurveArray &curves, const String &name = _T("CurveArray"));
  D3SceneObjectCurveArray(D3SceneObjectVisual *parent, const CurveArray &curves, const String &name = _T("CurveArray"));
  void draw() override;
};
