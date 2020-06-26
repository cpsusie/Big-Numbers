#include "pch.h"
#include <D3DGraphics/D3Ray.h>
#include <D3DGraphics/D3PickedInfo.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>

class DistComparator : public Comparator<D3DXINTERSECTINFO> {
public:
  int compare(const D3DXINTERSECTINFO &i1, const D3DXINTERSECTINFO &i2) {
    return sign(i1.Dist - i2.Dist);
  }
  AbstractComparator *clone() const {
    return new DistComparator;
  }
};

#pragma warning(disable:4703) // potentially uninitialized local pointer variable

bool D3SceneObjectVisual::intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info) const {
  LPD3DXMESH mesh = getMesh();
  BOOL       hit  = FALSE;
  if(mesh) {
    const D3DXMATRIX m = inverse(getWorld());
    const D3Ray      mray(m*ray.m_orig, unitVector(ray.m_dir*m));

    DWORD        faceIndex;
    float        U, V;
    LPD3DXBUFFER infoBuffer;
    DWORD        hitCount;
    V(D3DXIntersect(mesh
                   ,&mray.m_orig
                   ,&mray.m_dir
                   ,&hit
                   ,&faceIndex
                   ,&U, &V
                   ,&dist
                   ,&infoBuffer
                   ,&hitCount
    ));
    if(hitCount > 0) {
      D3DXINTERSECTINFO *infoArray = (D3DXINTERSECTINFO*)infoBuffer->GetBufferPointer();
      quickSort(infoArray, hitCount, sizeof(D3DXINTERSECTINFO), DistComparator());
      LPDIRECT3DINDEXBUFFER  indexBuffer;
      LPDIRECT3DVERTEXBUFFER vertexBuffer;
      void *indexItems = NULL, *vertexItems = NULL;
      try {
        V(mesh->GetIndexBuffer(&indexBuffer)); TRACE_REFCOUNT(indexBuffer);
        D3DINDEXBUFFER_DESC inxdesc;
        V(indexBuffer->GetDesc(&inxdesc));
        const bool use32Bit = inxdesc.Format == D3DFMT_INDEX32;
        V(indexBuffer->Lock(0, 0, &indexItems, D3DLOCK_READONLY));
        V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_REFCOUNT(vertexBuffer);
        D3DVERTEXBUFFER_DESC vtxdesc;
        V(vertexBuffer->GetDesc(&vtxdesc));
        const UINT itemSize = FVFToSize(vtxdesc.FVF);
        V(vertexBuffer->Lock(0, 0, &vertexItems, D3DLOCK_READONLY));

        for(UINT h = 0; h < hitCount; h++) {
          const D3DXINTERSECTINFO &hi = infoArray[h];
          faceIndex = hi.FaceIndex;
          const int vertex0Index = faceIndex * 3;
          int inx[3], i = 0;
          if(use32Bit) {
            for(const ULONG *ip = (ULONG*)indexItems + vertex0Index; i < 3;) inx[i++] = *(ip++);
          } else {
            for(const USHORT *ip = (USHORT*)indexItems + vertex0Index; i < 3;) inx[i++] = *(ip++);
          }

          D3DXVECTOR3 vtx[3];
          for(int i = 0; i < 3; i++) {
            const D3DXVECTOR3 &p = (const D3DXVECTOR3&)(((const BYTE*)vertexItems)[inx[i] * itemSize]);
            vtx[i] = p;
          }
          D3DXVECTOR3 c = cross(vtx[2] - vtx[0], vtx[1] - vtx[0]);
          if(mray.m_dir * c < 0) {
            hit  = TRUE;
            const D3PickedInfo pickInfo(this, faceIndex, inx, vtx, hi.U, hi.V);
            dist = length(ray.m_orig - pickInfo.getWorldPoint());
            if(info) {
              *info = pickInfo;
            }
            break;
          }
        }
        V(indexBuffer->Unlock());  indexItems = NULL;
        V(vertexBuffer->Unlock()); vertexItems = NULL;
        SAFERELEASE(vertexBuffer);
        SAFERELEASE(indexBuffer);
      } catch(...) {
        if(indexItems  != NULL) indexBuffer->Unlock();
        if(vertexItems != NULL) vertexBuffer->Unlock();
        SAFERELEASE(vertexBuffer);
        SAFERELEASE(indexBuffer);
        throw;
      }
    }
  }
  if(!hit) {
    for(D3SceneObjectVisual *child : m_children) {
      if(child->isVisible() && child->intersectsWithRay(ray, dist, info)) {
        hit = TRUE;
        break;
      }
    }
  }
  return hit ? true : false;
}
