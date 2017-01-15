#pragma once

#include "Color.h"
#include "Plane.h"

namespace ThreeD {

class MeshFace {
public:

  const D3DXVECTOR3 *m_v[3];
  D3DXVECTOR3        m_planeNormal;

#ifdef USE_MATERIAL
  Material      m_mat;
#endif

public:
#ifdef USE_MATERIAL
  /* Constructs a mesh face as a triangle bound by the three vertices
   * v0, v1 and v2, and having material mat.
   */
  MeshFace(const D3DXVECTOR3 *v0
          ,const D3DXVECTOR3 *v1
          ,const D3DXVECTOR3 *v2
          ,const Material &mat)
  {
    m_v[0]        = v0;
    m_v[1]        = v1;
    m_v[2]        = v2;
    m_mat         = mat;
    m_planeNormal = unitVector(Plane(*m_v[2], *m_v[1], *m_v[0]).normal());
  }
#else
  /* Constructs a mesh face as a triangle bound by the three vertices
   * v0, v1 and v2, and having material mat.
   */
  MeshFace(const D3DXVECTOR3 *v0, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2){
    m_v[0]        = v0;
    m_v[1]        = v1;
    m_v[2]        = v2;
    m_planeNormal = unitVector(Plane(*m_v[2], *m_v[1], *m_v[0]).normal());
  }
#endif

  /* Return pointer to vertex n. */
  const D3DXVECTOR3 *vertexPtr(int n) const { return m_v[n]; }

  /* Return actual value of vertex n. */
  const D3DXVECTOR3 &vertex(int n) const { return *m_v[n]; }

  const D3DXVECTOR3 &planeNormal() const {
    return m_planeNormal;
  }

  /* Return minimum and maximum points in each dimension.
   */
  void getMinMax(D3DXVECTOR3 *vmin, D3DXVECTOR3 *vmax) const;

  /* Set vertex pointers. */
  void setVertexPointers(const D3DXVECTOR3 **vptrs) {
    m_v[0] = vptrs[0];
    m_v[1] = vptrs[1];
    m_v[2] = vptrs[2];
  }

  bool isBetween(const D3DXVECTOR3 &vmin, const D3DXVECTOR3 &vmax) const {
    return ::isBetween(vertex(0), vmin, vmax)
        && ::isBetween(vertex(1), vmin, vmax)
        && ::isBetween(vertex(2), vmin, vmax);
  }
  String toString() const;
};

} // namespace ThreeD
