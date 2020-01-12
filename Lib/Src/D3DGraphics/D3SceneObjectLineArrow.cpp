#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

// ----------------------------------------------------- D3SceneObjectLineArrow ------------------------------------------------------

D3SceneObjectLineArrow::D3SceneObjectLineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, D3DCOLOR color) : D3SceneObjectWithVertexBuffer(scene) {
#define FANCOUNT 14
#define ITEMCOUNT (5 + 2 * FANCOUNT)

  const D3DXVECTOR3 v = (D3DXVECTOR3)to - (D3DXVECTOR3)from;
  const D3DXVECTOR3 vn = unitVector(v);

  VertexNormal *vertices = allocateVertexArray<VertexNormal>(ITEMCOUNT);

  vertices[0].setPos(from);
  vertices[1].setPos(to); vertices[1].setNormal(vn);
  const D3DXVECTOR3 cirkelCenter = (D3DXVECTOR3)to - 0.1f * vn;
  D3DXVECTOR3   radius1 = ortonormalVector(v) * 0.04f;
  D3DXVECTOR3   p = cirkelCenter + radius1;
  VertexNormal *vtx1 = vertices + 2;
  vtx1->setPos(p); vtx1->setNormal(radius1); vtx1++;

  D3DXVECTOR3   radius2 = radius1;
  VertexNormal *vtx2 = vertices + FANCOUNT + 3;
  vtx2->setPos(cirkelCenter); vtx2->setNormal(-vn); vtx2++;
  vtx2->setPos(p);            vtx2->setNormal(-vn); vtx2++;

  for (int i = 0; i < FANCOUNT; i++) {
    radius1 = rotate(radius1, v, radians(360.0f / FANCOUNT));
    radius2 = rotate(radius2, v, -radians(360.0f / FANCOUNT));
    vtx1[i].setPos(cirkelCenter + radius1); vtx1[i].setNormal(unitVector(radius1));
    vtx2[i].setPos(cirkelCenter + radius2); vtx2[i].setNormal(-vn);
  }
  unlockVertexArray();
  m_materialIndex = -1;
  setColor(color);
}

void D3SceneObjectLineArrow::draw() {
  if (hasVertexBuffer()) {
    getScene().setFillMode(D3DFILL_SOLID).setShadeMode(D3DSHADE_GOURAUD);
    setStreamSource();
    setLightingEnable(true);
    setSceneMaterial();
    drawPrimitive(D3DPT_LINELIST, 0, 1);
    drawPrimitive(D3DPT_TRIANGLEFAN, 1, FANCOUNT);
    drawPrimitive(D3DPT_TRIANGLEFAN, FANCOUNT + 3, FANCOUNT);
  }
}

void D3SceneObjectLineArrow::setColor(D3DCOLOR color) {
  const D3DCOLORVALUE cv = colorToColorValue(color);
  if (hasMaterial()) {
    MATERIAL mat = getMaterial();
    mat.Diffuse = cv;
    mat.Emissive = cv;
    getScene().setMaterial(mat);
  }
  else {
    D3DMATERIAL mat;
    memset(&mat, 0, sizeof(mat));
    mat.Diffuse = cv;
    mat.Emissive = cv;
    m_materialIndex = getScene().addMaterial(mat);
  }
}
