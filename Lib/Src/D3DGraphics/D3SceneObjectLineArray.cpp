#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3SceneObjectLineArray.h>
#include <D3DGraphics/D3ToString.h>

D3SceneObjectLineArray::D3SceneObjectLineArray(D3Scene &scene, const String &name)
: D3SceneObjectWithVertexBuffer(scene, name)
{
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3Scene &scene, const Line3D *lines, UINT n, const String &name)
: D3SceneObjectWithVertexBuffer(scene, name)
{
  initBuffer(lines, n);
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3Scene &scene, const CompactArray<Line3D> &lineArray, const String &name)
: D3SceneObjectWithVertexBuffer(scene, name)
{
  initBuffer(lineArray.getBuffer(), (UINT)lineArray.size());
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3SceneObjectVisual *parent, const String &name)
: D3SceneObjectWithVertexBuffer(parent, name)
{
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3SceneObjectVisual *parent, const Line3D *lines, UINT n, const String &name)
: D3SceneObjectWithVertexBuffer(parent, name)
{
  initBuffer(lines, n);
}

D3SceneObjectLineArray::D3SceneObjectLineArray(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lineArray, const String &name)
: D3SceneObjectWithVertexBuffer(parent, name)
{
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
    setDeviceMaterialIfExist();
    setDeviceStreamSource().setWorldMatrix(getWorld())
                           .setLightingEnable(getLightingEnable())
                           .drawPrimitive(D3DPT_LINELIST, 0, m_primitiveCount);
  }
  __super::draw();
}
