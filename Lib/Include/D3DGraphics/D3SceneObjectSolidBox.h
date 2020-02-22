#pragma once

#include "D3SceneObjectWithMesh.h"

class MeshBuilder;

class D3SceneObjectSolidBox : public D3SceneObjectWithMesh {
private:
  void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3);
  void init(const Vertex &p1, const Vertex &p2);
public:
  D3SceneObjectSolidBox(D3Scene             &scene , const D3DXCube3 &cube                  , const String &name = _T("SolidBox"));
  D3SceneObjectSolidBox(D3Scene             &scene , const Vertex    &p1  , const Vertex &p2, const String &name = _T("SolidBox"));
  D3SceneObjectSolidBox(D3SceneObjectVisual *parent, const D3DXCube3 &cube                  , const String &name = _T("SolidBox"));
  D3SceneObjectSolidBox(D3SceneObjectVisual *parent, const Vertex    &p1  , const Vertex &p2, const String &name = _T("SolidBox"));
};
