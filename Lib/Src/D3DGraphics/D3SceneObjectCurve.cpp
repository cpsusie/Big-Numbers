#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

// ----------------------------------------------- D3SceneObjectCurve ------------------------------------------------------------

D3SceneObjectCurve::D3SceneObjectCurve(D3Scene &scene, const VertexArray &points) : D3SceneObjectWithVertexBuffer(scene) {
  m_primitiveCount = (int)points.size() - 1;

  Vertex *vertices = allocateVertexArray<Vertex>((UINT)points.size());
  memcpy(vertices, points.getBuffer(), sizeof(Vertex)*points.size());
  unlockVertexArray();
}

void D3SceneObjectCurve::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    drawPrimitive(D3DPT_LINESTRIP, 0, m_primitiveCount);
  }
}
