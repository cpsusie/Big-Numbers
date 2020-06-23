#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectLineArray : public D3SceneObjectWithVertexBuffer {
protected:
  void initBuffer(const Line3D *lines, UINT n);
public:
  D3SceneObjectLineArray(D3Scene             &scene                                        , const String &name = _T("LineArray"));
  D3SceneObjectLineArray(D3Scene             &scene , const Line3D *lines, UINT n          , const String &name = _T("LineArray"));
  D3SceneObjectLineArray(D3Scene             &scene , const CompactArray<Line3D> &lineArray, const String &name = _T("LineArray"));
  D3SceneObjectLineArray(D3SceneObjectVisual *parent                                       , const String &name = _T("LineArray"));
  D3SceneObjectLineArray(D3SceneObjectVisual *parent, const Line3D *lines, UINT n          , const String &name = _T("LineArray"));
  D3SceneObjectLineArray(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lineArray, const String &name = _T("LineArray"));
  void draw() override;
};
