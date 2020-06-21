#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>

D3Cube MeshBuilder::getBoundingBox() const {
  D3DXVECTOR3  pmin,pmax;
  const size_t faceCount   = m_faceArray.size();
  bool         firstTime   = true;
  const UINT   vertexCount = getVertexCount();
  for(size_t i = 0; i < faceCount; i++) {
    const Face &f = m_faceArray[i];
    if(f.isEmpty()) continue;
    const UINT       n = f.getIndexCount();
    const VNTIArray &a = f.getIndexArray();
    for(UINT j = 0; j < n; j++) {
      const UINT vi = a[j].m_vIndex;
      if(vi < vertexCount) {
        const Vertex &v = m_vertices[vi];
        if(firstTime) {
          pmin = pmax = v;
          firstTime = false;
        } else {
          if(v.x < pmin.x) {
            pmin.x = v.x;
          } else if(v.x > pmax.x) {
            pmax.x = v.x;
          }
          if(v.y < pmin.y) {
            pmin.y = v.y;
          } else if(v.y > pmax.y) {
            pmax.y = v.y;
          }
          if(v.z < pmin.z) {
            pmin.z = v.z;
          } else if(v.z > pmax.z) {
            pmax.z = v.z;
          }
        }
      }
    }
  }
  return D3Cube(pmin,pmax);
}
