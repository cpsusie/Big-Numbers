#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

// ------------------------------------------------ D3SceneObjectLineArray -----------------------------------------------------------

D3SceneObjectLineArray::D3SceneObjectLineArray(D3Scene &scene, const Line3D *lines, UINT n) : D3SceneObjectWithVertexBuffer(scene) {
  initBuffer(lines, n);
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3Scene &scene, const CompactArray<Line3D> &lineArray) : D3SceneObjectWithVertexBuffer(scene) {
  initBuffer(lineArray.getBuffer(), (UINT)lineArray.size());
}

void D3SceneObjectLineArray::initBuffer(const Line3D *lines, UINT n) {
  Vertex *vertices = allocateVertexArray<Vertex>(2 * n);
  m_primitiveCount = n;
  memcpy(vertices, lines, sizeof(Line3D)*n);
  unlockVertexArray();
}

void D3SceneObjectLineArray::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    drawPrimitive(D3DPT_LINELIST, 0, m_primitiveCount);
  }
}
