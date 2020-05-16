#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectLineArrow.h>
#include <D3DGraphics/D3ToString.h>

// ----------------------------------------------------- D3SceneObjectLineArrow ------------------------------------------------------

D3SceneObjectLineArrow::D3SceneObjectLineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, const String &name)
: D3SceneObjectWithVertexBuffer(scene, name)
{
  init(from, to);
}

D3SceneObjectLineArrow::D3SceneObjectLineArrow(D3SceneObjectVisual *parent, const Vertex &from, const Vertex &to, const String &name)
: D3SceneObjectWithVertexBuffer(parent, name)
{
  init(from, to);
}

void D3SceneObjectLineArrow::init(const Vertex &from, const Vertex &to) {
#define FANCOUNT 12
#define ITEMCOUNT (5 + 2 * FANCOUNT)

  const D3DXVECTOR3 v = (D3DXVECTOR3)to - (D3DXVECTOR3)from;
  const D3DXVECTOR3 vn = unitVector(v), vnneg = -vn;

  VertexNormal *vertices = allocateVertexArray<VertexNormal>(ITEMCOUNT);

  vertices[0].setPos(from);
  vertices[1].setPos(to).setNormal(vn);
  const D3DXVECTOR3 cirkelCenter = (D3DXVECTOR3)to - 0.1f * vn;
  D3DXVECTOR3       radius1      = ortonormalVector(v) * 0.04f, radius2 = radius1, radiusUnit = unitVector(radius1);
  D3DXVECTOR3       p            = cirkelCenter + radius1;
  D3DXQUATERNION    rot1         = createRotation(vn,  2 * D3DX_PI / FANCOUNT);
  D3DXQUATERNION    rot2         = createRotation(vn, -2 * D3DX_PI / FANCOUNT);
  VertexNormal     *vtx1         = vertices + 2, *vtx2 = vertices + FANCOUNT + 3;

  vtx1->setPos(p).setNormal(radius1);          vtx1++;
  vtx2->setPos(cirkelCenter).setNormal(vnneg); vtx2++;
  vtx2->setPos(p).setNormal(vnneg);            vtx2++;

  for(int i = 0; i < FANCOUNT; i++) {
    radius1 = rotate(radius1, rot1);
    radius2 = rotate(radius2, rot2);
    vtx1[i].setPos(cirkelCenter + radius1).setNormal(radiusUnit);
    vtx2[i].setPos(cirkelCenter + radius2).setNormal(vnneg);
  }
  unlockVertexArray();
}

void D3SceneObjectLineArrow::draw() {
  if(hasVertexBuffer()) {
    setDeviceMaterialIfExist();
    setStreamSource().setWorldMatrix(getWorld())
                     .setFillMode(getFillMode())
                     .setShadeMode(getShadeMode())
                     .setLightingEnable(getLightingEnable())
                     .drawPrimitive(D3DPT_LINELIST, 0, 1)
                     .drawPrimitive(D3DPT_TRIANGLEFAN, 1, FANCOUNT)
                     .drawPrimitive(D3DPT_TRIANGLEFAN, FANCOUNT + 3, FANCOUNT);
  }
  __super::draw();
}
