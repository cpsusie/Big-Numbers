#include "pch.h"
#include <D3DGraphics/D3Scene.h>

D3SceneObject *D3Scene::getPickedObject(const CPoint &point, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  D3Ray tmpRay, &pickedRay = ray ? *ray : tmpRay;
  pickedRay = getPickedRay(point);
  return getPickedObject(pickedRay, mask, hitPoint, dist, info);
}

D3SceneObject *D3Scene::getPickedObject(const D3Ray &ray, long mask, D3DXVECTOR3 *hitPoint, float *dist, D3PickedInfo *info) const {
  float          minDistance   = -1;
  D3SceneObject *closestObject = NULL;
  for(Iterator<D3SceneObject*> it = getObjectIterator(mask); it.hasNext();) {
    D3SceneObject *obj = it.next();
    if(!obj->isVisible()) {
      continue;
    }
    float distance;
    if(obj->intersectsWithRay(ray, distance, info)) {
      if((closestObject == NULL) || (distance < minDistance)) {
        closestObject = obj;
        minDistance = distance;
      }
    }
  }
  if(closestObject) {
    if(hitPoint) {
      *hitPoint = ray.getHitPoint(minDistance);
    }
    if(dist) {
      *dist = minDistance;
    }
  }
  return closestObject;
}

D3Ray D3Scene::getPickedRay(const CPoint &point) const {
  const CSize winSize = getClientRect(m_hwnd).Size();

  const D3DXMATRIX matProj = getDevProjMatrix();

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * point.x) / winSize.cx) - 1) / matProj._11 * m_nearViewPlane;
  v.y = -(((2.0f * point.y) / winSize.cy) - 1) / matProj._22 * m_nearViewPlane;
  v.z = -m_nearViewPlane;

  const D3DXMATRIX camWorld = m_camPDUS.getWorldMatrix();
  return D3Ray(camWorld*v, v*camWorld);
}

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

bool D3SceneObject::intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info) const {
  LPD3DXMESH mesh = getMesh();
  if(mesh == NULL) return false;

  const D3DXMATRIX  m      = invers(getWorldMatrix());
  const D3DXVECTOR3 rayPos = m * ray.m_orig;
  const D3DXVECTOR3 rayDir = ray.m_dir * m;

  BOOL         hit;
  DWORD        faceIndex;
  float        U, V;
  LPD3DXBUFFER infoBuffer;
  DWORD        hitCount;
  V(D3DXIntersect(mesh
                , &rayPos
                , &rayDir
                , &hit
                , &faceIndex
                , &U, &V
                , &dist
                , &infoBuffer
                , &hitCount
  ));
  hit = FALSE;
  if(hitCount > 0) {
    D3DXINTERSECTINFO *infoArray = (D3DXINTERSECTINFO*)infoBuffer->GetBufferPointer();
    quickSort(infoArray, hitCount, sizeof(D3DXINTERSECTINFO), DistComparator());
    LPDIRECT3DINDEXBUFFER  indexBuffer;
    LPDIRECT3DVERTEXBUFFER vertexBuffer;
    void *indexItems = NULL, *vertexItems = NULL;
    try {
      V(mesh->GetIndexBuffer(&indexBuffer));
      D3DINDEXBUFFER_DESC inxdesc;
      V(indexBuffer->GetDesc(&inxdesc));
      const bool use32Bit = inxdesc.Format == D3DFMT_INDEX32;
      V(indexBuffer->Lock(0, 0, &indexItems, D3DLOCK_READONLY));
      V(mesh->GetVertexBuffer(&vertexBuffer));
      D3DVERTEXBUFFER_DESC vtxdesc;
      V(vertexBuffer->GetDesc(&vtxdesc));
      const size_t itemSize = FVFToSize(vtxdesc.FVF);
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
        D3DXVECTOR3 c = crossProduct(vtx[2] - vtx[0], vtx[1] - vtx[0]);
        if(rayDir * c < 0) {
          hit = TRUE;
          dist = hi.Dist;
          if(info) {
            *info = D3PickedInfo(faceIndex, inx, vtx, hi.U, hi.V);
          }
          break;
        }
      }
      V(indexBuffer->Unlock());  indexItems = NULL;
      V(vertexBuffer->Unlock()); vertexItems = NULL;
    } catch (...) {
      if(indexItems  != NULL) indexBuffer->Unlock();
      if(vertexItems != NULL) vertexBuffer->Unlock();
      throw;
    }
  }
  return hit ? true : false;
}

String D3PickedInfo::toString(int dec) const {
  return isEmpty()
    ? EMPTYSTRING
    : format(_T("Face:%5d:[%5d,%5d,%5d], (U,V):(%s,%s), MP:%s")
            ,m_faceIndex
            ,m_vertexIndex[0], m_vertexIndex[1], m_vertexIndex[2]
            ,::toString(m_U, dec).cstr(), ::toString(m_V, dec).cstr()
            ,::toString(getMeshPoint(), dec).cstr()
            );
}
