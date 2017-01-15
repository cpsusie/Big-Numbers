#include "stdafx.h"

using namespace ThreeD;

CsgIsosurface::CsgIsosurface() {
  m_maxPoints = 0;
}

CsgIsosurface::~CsgIsosurface() {
  std::list<Isosurface *>::iterator it = m_children.begin();
  while(it != m_children.end()) {
    Isosurface *child = (*it);
    it = m_children.erase(it);
    delete child;
  }

  if(m_maxPoints) {
    delete[] m_densities;
    m_maxPoints = 0;
  }
}

void CsgIsosurface::setCsgMode(CSGMode csgMode) {
  m_csgMode = csgMode;
}

void CsgIsosurface::addChild(Isosurface *child) {
  m_children.push_back(child);
}

BoundingBox CsgIsosurface::getBoundingBox(const D3DXMATRIX &combinedTrans) {
  D3DXMATRIX t(m_localTrans);
  t *= combinedTrans;

  BoundingBox bbox;
  std::list<Isosurface *>::iterator it = m_children.begin();
  while(it != m_children.end()) {
    Isosurface *child = (*it);
    ++it;
    const BoundingBox &child_bbox = child->getBoundingBox(t);
    bbox.merge(child_bbox);
  }
  return bbox;
}

void CsgIsosurface::fDensity_n(float x0, float y0, float z0, float dz, int pointCount, float *densities) {
  // the case where there are no actual children in the csg
  if(m_children.empty()) {
    for(int i = 0; i < pointCount; ++i) {
      densities[i] = 1.0f;
    }
    return;
  }

  // do the first child isosurface
  std::list<Isosurface *>::const_iterator it = m_children.begin();
  Isosurface *child = (*it);
  ++it;
  child->fDensity(x0, y0, z0, dz, pointCount, densities);

  // allocate temporary storage for CSG operation
  if(pointCount > m_maxPoints) {
    if(m_maxPoints) {
      delete[] m_densities;
    }
    m_maxPoints = pointCount;
    m_densities = new float[m_maxPoints];
  }

  if(m_csgMode == CSG_UNION) { // union
    while(it != m_children.end()) {
      Isosurface *child = (*it);
      ++it;
      child->fDensity(x0, y0, z0, dz, pointCount, m_densities);
      for(int i = 0; i < pointCount; ++i) {
        if(m_densities[i] < densities[i]) {
          densities[i] = m_densities[i];
        }
      }
    }
  } else if(m_csgMode == CSG_INTERSECTION) { // intersection
    while(it != m_children.end()) {
      Isosurface *child = (*it);
      ++it;
      child->fDensity(x0, y0, z0, dz, pointCount, m_densities);
      for(int i = 0; i < pointCount; ++i) {
        if(m_densities[i] > densities[i]) {
          densities[i] = m_densities[i];
        }
      }
    }
  } else if(m_csgMode == CSG_DIFFERENCE) { // difference
    while(it != m_children.end()) {
      Isosurface *child = (*it);
      ++it;
      child->fDensity(x0, y0, z0, dz, pointCount, m_densities);
      for(int i = 0; i < pointCount; ++i) {
        if(-m_densities[i] > densities[i]) {
          densities[i] = -m_densities[i];
        }
      }
    }
  }
}

void CsgIsosurface::fDensity(float x0, float y0, float z0, float dz, int pointCount, float *densities) {
  if(pointCount != 1) {
    fDensity_n(x0, y0, z0, dz, pointCount, densities);
    return;
  }

  // the case where there are no actual children in the csg
  if(m_children.empty()) {
    *densities = 1.0f;
    return;
  }

  float density, density2;

  // do the first child isosurface
  std::list<Isosurface *>::const_iterator it = m_children.begin();
  (*it)->fDensity(x0, y0, z0, 0, 1, &density);
  ++it;

  while(it != m_children.end()) {
    (*it)->fDensity(x0, y0, z0, 0, 1, &density2);
    ++it;
    if(m_csgMode == CSG_DIFFERENCE) {
      density2 = -density2;
    }
    if((m_csgMode == CSG_UNION && density2 < density)
    || (m_csgMode != CSG_UNION && density2 > density)) {
      density = density2;
    }
  }
  *densities = density;
}

Isosurface *CsgIsosurface::findIsosurface(float x, float y, float z) {
  float density, density2;

  // try the first child isosurface
  std::list<Isosurface *>::const_iterator it = m_children.begin();
  Isosurface *iso = (*it);
  ++it;
  iso->fDensity(x, y, z, 0, 1, &density);

  while(it != m_children.end()) {
    Isosurface *iso2 = (*it);
    ++it;
    iso2->fDensity(x, y, z, 0, 1, &density2);
    if(m_csgMode == CSG_DIFFERENCE) {
      density2 = -density2;
    }
    if((m_csgMode == CSG_UNION && density2 < density)
    || (m_csgMode != CSG_UNION && density2 > density)) {
      density = density2;
      iso = iso2;
    }
  }
  return iso;
}

void CsgIsosurface::fNormal(const D3DXVECTOR3 &point, D3DXVECTOR3 &normal) {
  Isosurface *iso = findIsosurface(point.x, point.y, point.z);
  iso->fNormal(point, normal);
}

#ifdef USE_MATERIAL
const Material &CsgIsosurface::fMaterial(const D3DXVECTOR3 &point, float density) {
  Isosurface *iso = findIsosurface(point.x, point.y, point.z);
  return iso->fMaterial(point, density);
}
#endif
