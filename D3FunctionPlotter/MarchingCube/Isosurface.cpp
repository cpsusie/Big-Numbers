#include "stdafx.h"

using namespace ThreeD;

Isosurface::Isosurface() {
  m_globalTrans = m_localTrans = createIdentityMatrix();
}

Isosurface::~Isosurface() {
}

void Isosurface::setTransform(const D3DXMATRIX &t) {
  m_localTrans = t;
}

void Isosurface::addBoundingBox(const BoundingBox &bbox) {
  m_bbox.merge(bbox);
}

BoundingBox Isosurface::getBoundingBox(const D3DXMATRIX &combinedTrans) {
  m_globalTrans    = m_localTrans;
  m_globalTrans   *= combinedTrans;
  m_globalTransInv = invers(m_globalTrans);

  BoundingBox bbox(m_bbox);
  bbox.transform(m_globalTrans);
  return bbox;
}

#if defined(USE_MATERIAL)

Material avg3(const Material &m1, const Material &m2, const Material &m3) {
  Material m;
  m.color      = (m1.color      + m2.color      + m3.color     ) / 3.0f;
  m.ambient    = (m1.ambient    + m2.ambient    + m3.ambient   ) / 3.0f;
  m.diffuse    = (m1.diffuse    + m2.diffuse    + m3.diffuse   ) / 3.0f;
  m.specular   = (m1.specular   + m2.specular   + m3.specular  ) / 3.0f;
  m.brilliance = (m1.brilliance + m2.brilliance + m3.brilliance) / 3.0f;
  return m;
}

#endif
