#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectSolidBox.h>

D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const D3Cube &cube, const String &name)
: D3SceneObjectWithMesh(scene, NULL, name)
{
  init(cube.getMin(), cube.getMax());
}

D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const Vertex &p1, const Vertex &p2, const String &name)
: D3SceneObjectWithMesh(scene, NULL, name)
{
  init(p1, p2);
}

D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3SceneObjectVisual *parent, const D3Cube &cube, const String &name)
: D3SceneObjectWithMesh(parent, NULL, name)
{
  init(cube.getMin(), cube.getMax());
}

D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3SceneObjectVisual *parent, const Vertex    &p1, const Vertex &p2, const String &name)
: D3SceneObjectWithMesh(parent, NULL, name)
{
  init(p1, p2);
}

void D3SceneObjectSolidBox::init(const Vertex &p1, const Vertex &p2) {
  MeshBuilder mb;
  const D3DXVECTOR3 pmin = p1, pmax = p2;
  const int  lbn = mb.addVertex(pmin.x, pmin.y, pmin.z); // left  bottom near corner
  const int  lbf = mb.addVertex(pmin.x, pmin.y, pmax.z); // left  bottom far  corner
  const int  ltn = mb.addVertex(pmin.x, pmax.y, pmin.z); // left  top    near corner
  const int  ltf = mb.addVertex(pmin.x, pmax.y, pmax.z); // left  top    far  corner
  const int  rbn = mb.addVertex(pmax.x, pmin.y, pmin.z); // right bottom near corner
  const int  rbf = mb.addVertex(pmax.x, pmin.y, pmax.z); // right bottom far  corner
  const int  rtn = mb.addVertex(pmax.x, pmax.y, pmin.z); // right top    near corner
  const int  rtf = mb.addVertex(pmax.x, pmax.y, pmax.z); // right top    far  corner

  mb.addSquareFace(lbn, lbf, rbf, rbn);                // bottom
  mb.addSquareFace(ltn, rtn, rtf, ltf);                // top
  mb.addSquareFace(lbn, ltn, ltf, lbf);                // left side
  mb.addSquareFace(lbf, ltf, rtf, rbf);                // back side
  mb.addSquareFace(rbf, rtf, rtn, rbn);                // right side
  mb.addSquareFace(rbn, rtn, ltn, lbn);                // front side

  m_mesh = mb.createMesh(getDevice(), false);
}
