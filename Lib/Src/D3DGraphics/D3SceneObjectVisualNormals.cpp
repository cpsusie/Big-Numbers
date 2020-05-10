#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>

#define NEEDEDFVFFLAGS (D3DFVF_XYZ | D3DFVF_NORMAL)


D3SceneObjectVisualNormals::D3SceneObjectVisualNormals(D3SceneObjectVisual *parent)
  : D3SceneObjectWithVertexBuffer(parent,format(_T("%s.normals"), parent->getName().cstr()))
{
  if(parent->hasMesh()) {
    LPD3DXMESH mesh = parent->getMesh();
    LPDIRECT3DVERTEXBUFFER vertexBuffer;
    V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_CREATE(vertexBuffer);
    createNormalsObject(vertexBuffer);
    SAFERELEASE(vertexBuffer);
  } else if(parent->hasVertexBuffer()) {
    LPDIRECT3DVERTEXBUFFER vertexBuffer = parent->getVertexBuffer();
    createNormalsObject(vertexBuffer);
  }
};

#define FANCOUNT       14
#define ARROWITEMCOUNT (5 + 2 * FANCOUNT)

void D3SceneObjectVisualNormals::createNormalsObject(LPDIRECT3DVERTEXBUFFER vertexBuffer) {
  D3DVERTEXBUFFER_DESC parentDesc;
  V(vertexBuffer->GetDesc(&parentDesc));
  if((parentDesc.FVF & NEEDEDFVFFLAGS) != NEEDEDFVFFLAGS) {
    throwInvalidArgumentException(__TFUNCTION__, _T("parent has no normals:fvf=%s"), FVFToString(parentDesc.FVF).cstr());
  }
  const UINT    parentItemSize    =  FVFToSize(parentDesc.FVF);
  const UINT    parentVertexCount = parentDesc.Size / parentItemSize;
  VertexNormal *vertices          = allocateVertexArray<VertexNormal>(parentVertexCount*ARROWITEMCOUNT);
  void         *parentVertices;
  V(vertexBuffer->Lock(0, parentDesc.Size, &parentVertices, D3DLOCK_READONLY));
  VertexNormal *dst = vertices;
  for(const char *srcp = (char*)parentVertices, *endp = srcp + parentDesc.Size; srcp < endp; srcp += parentItemSize, dst += ARROWITEMCOUNT) {
    const VertexNormal &srcv = *(VertexNormal*)srcp;
    addArrowVertices(dst, srcv);
  }
  V(vertexBuffer->Unlock());
  unlockVertexArray();
  m_normalCount = parentVertexCount;
}

void D3SceneObjectVisualNormals::addArrowVertices(VertexNormal *dst, const VertexNormal &src) const {

  const D3DXVECTOR3 &from = src.getPos();
  const D3DXVECTOR3 vn    = unitVector(src.getNormal());
  const D3DXVECTOR3 to    = from + 0.8f * vn;

  dst[0].setPos(from);
  dst[1].setPos(to).setNormal(vn);
  const D3DXVECTOR3 cirkelCenter = (D3DXVECTOR3)to - 0.1f * vn;
  D3DXVECTOR3   radius1 = ortonormalVector(vn) * 0.04f;
  D3DXVECTOR3   p = cirkelCenter + radius1;
  VertexNormal *vtx1 = dst + 2;
  vtx1->setPos(p).setNormal(radius1); vtx1++;

  D3DXVECTOR3   radius2 = radius1;
  VertexNormal *vtx2 = dst + FANCOUNT + 3;
  vtx2->setPos(cirkelCenter).setNormal(-vn); vtx2++;
  vtx2->setPos(p).setNormal(-vn);            vtx2++;

  for(int i = 0; i < FANCOUNT; i++) {
    radius1 = rotate(radius1, vn,  radians(360.0f / FANCOUNT));
    radius2 = rotate(radius2, vn, -radians(360.0f / FANCOUNT));
    vtx1[i].setPos(cirkelCenter + radius1).setNormal(unitVector(radius1));
    vtx2[i].setPos(cirkelCenter + radius2).setNormal(-vn);
  }
}

void D3SceneObjectVisualNormals::draw() {
  if(hasVertexBuffer()) {
    D3Device &device = setDeviceMaterialIfExist();
    setStreamSource().setWorldMatrix(getWorld())
                     .setFillMode(getFillMode())
                     .setShadeMode(getShadeMode())
                     .setLightingEnable(getLightingEnable());

    const UINT endj = m_normalCount * ARROWITEMCOUNT;
    for(UINT j = 0; j < endj; j+=ARROWITEMCOUNT) {
       device.drawPrimitive(D3DPT_LINELIST   , j             , 1       )
             .drawPrimitive(D3DPT_TRIANGLEFAN, j+1           , FANCOUNT)
             .drawPrimitive(D3DPT_TRIANGLEFAN, j+FANCOUNT + 3, FANCOUNT);
    }
  }
}
