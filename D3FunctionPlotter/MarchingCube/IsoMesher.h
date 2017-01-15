#pragma once

#include "Isosurface.h"
#include "Mesh.h"

namespace ThreeD {

class IsoMesher {
protected:
  Isosurface   &m_iso;
  D3DXVECTOR3   m_voxelSize;
  Mesh         *m_mesh;

  bool        (*m_progressFunc)(void *, int);
  void         *m_progressParm;
  int           m_progressPercent;
  long          m_progressTime;

  struct Point {
    float        density;
    D3DXVECTOR3 *v;
  };

  /* Intersect a voxel edge along the x,y or z axis */
  void intersectXaxis(const Point &p0, const Point &p1, Point &out) const;
  void intersectYaxis(const Point &p0, const Point &p1, Point &out) const;
  void intersectZaxis(const Point &p0, const Point &p1, Point &out) const;

  /* Invoke progress function to update the percent */
  bool invokeProgressFunc(int percent) {
    if(!m_progressFunc) {
      return false;
    }
    m_progressPercent = percent;
    return m_progressFunc(m_progressParm, m_progressPercent);
  }

  /* Invoke progress function without updating the percent */
  bool invokeProgressFunc();

public:
  IsoMesher(Isosurface &iso);
  void setVoxelSize(float x, float y, float z);
  void setProgressFunc(bool (*func)(void *, int), void *parm);
  virtual Mesh *createMesh() = 0;
};

} // namespace ThreeD
