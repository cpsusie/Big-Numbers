#pragma once

#include "D3SceneObjectLineArray.h"

class D3SceneObjectWireFrameBox : public D3SceneObjectLineArray {
private:
  void init(const Vertex &p1, const Vertex &p2);
public:
  D3SceneObjectWireFrameBox(D3Scene             &scene , const D3Cube &cube                , const String &name = _T("WireFrameBox"));
  D3SceneObjectWireFrameBox(D3Scene             &scene , const Vertex &p1, const Vertex &p2, const String &name = _T("WireFrameBox"));
  D3SceneObjectWireFrameBox(D3SceneObjectVisual *parent, const D3Cube &cube                , const String &name = _T("WireFrameBox"));
  D3SceneObjectWireFrameBox(D3SceneObjectVisual *parent, const Vertex &p1, const Vertex &p2, const String &name = _T("WireFrameBox"));
};
