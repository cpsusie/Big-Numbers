#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

// ------------------------------------------------ D3SceneObject ---------------------------------------------------

bool D3SceneObject::intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info) const {
  LPD3DXMESH mesh = getMesh();
  if(mesh == NULL) return false;

  const D3DXMATRIX  m      = invers(getWorldMatrix());
  const D3DXVECTOR3 rayPos = m * ray.m_orig;
  const D3DXVECTOR3 rayDir = ray.m_dir * m;

  BOOL         hit;
  DWORD        faceIndex;
  float        pu,pv;
  V(D3DXIntersect(mesh
                 ,&rayPos
                 ,&rayDir
                 ,&hit
                 ,&faceIndex
                 ,&pu,&pv
                 ,&dist
                 ,NULL // when these parameters are NULL, only the closest intersection (if any) will be recognized (dist)
                 ,NULL
                 ));

  if(hit && info != NULL) {
    void *indexItems;
    LPDIRECT3DINDEXBUFFER indexBuffer;
    V(mesh->GetIndexBuffer(&indexBuffer));
    D3DINDEXBUFFER_DESC desc;
    V(indexBuffer->GetDesc(&desc));
    const bool use32Bit = desc.Format == D3DFMT_INDEX32;
    V(indexBuffer->Lock(0,0,&indexItems, D3DLOCK_READONLY));

    const int vertex0Index = faceIndex * 3;
    int i0,i1,i2;
    if(use32Bit) {
      const ULONG *ip = (ULONG*)indexItems + vertex0Index;
      i0 = *(ip++);
      i1 = *(ip++);
      i2 = *(ip++);
    } else {
      const USHORT *ip = (USHORT*)indexItems + vertex0Index;
      i0 = *(ip++);
      i1 = *(ip++);
      i2 = *(ip++);
    }
    V(indexBuffer->Unlock());
    *info = D3PickedInfo(faceIndex, i0,i1,i2,pu,pv,(ray.m_orig + dist * ray.m_dir - getPos()) * m);
  }
  return hit ? true : false;
}



// -----------------------------------------------------------------------------------------------------------

#define SINCOS(degree,c,s) double c = radians(degree), s; sincos(c,s)
#define RSINCOS(degree,r,c,s) SINCOS(degree,c,s); c*=r; s*=r

CurveArray createSphereObject(double r) {
  CurveArray curves;
  for(int fi = 0; fi < 180; fi += 45) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  for(int fi = -180+30; fi < 180; fi += 30) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  return curves;
}
