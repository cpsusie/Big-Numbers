#include "pch.h"
#include <D3DGraphics/D3Cube.h>
#include <D3DGraphics/Profile2D.h>
#include <D3DGraphics/Profile3D.h>

Point2DTo3DConverter::Point2DTo3DConverter(char rotateAxis, char rotateAxisAlignsTo) {
  m_rotateAxis = rotateAxis;         // ['x','y','z'] - rotationaxis in 3D space
  m_rotateAxisAlignsTo = rotateAxisAlignsTo; // ['x','y']     - axis in 2D space aligned with rotationAxis
  checkIsValid();
  switch (m_rotateAxis) {
  case 'x':
    switch (m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'y'; break;
    case 'y': m_xTo3Dcoord = 'z'; m_yTo3Dcoord = 'x'; break;
    }
    break;
  case 'y':
    switch (m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'y'; m_yTo3Dcoord = 'z'; break;
    case 'y': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'y'; break;
    }
    break;
  case 'z':
    switch (m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'z'; m_yTo3Dcoord = 'y'; break;
    case 'y': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'z'; break;
    }
    break;
  }
}

void Point2DTo3DConverter::checkIsValid() const { // throws Exception if not valid
  if (strchr("xyz", m_rotateAxis) == NULL) {
    throwException(_T("RotateAxis must be 'x','y' or 'z'"));
  }
  if (strchr("xy", m_rotateAxisAlignsTo) == NULL) {
    throwException(_T("RotateAxisAlignsTo must be 'x' or 'y'"));
  }
}

VertexCurve3D::VertexCurve3D(const VertexCurve2D &c, const Point2DTo3DConverter &converter) {
  const size_t  n = c.size();
  setCapacity(n);
  for (const Vertex2D v : c) {
    add(VertexNormal(converter.convertPoint(v.m_pos), converter.convertPoint(v.m_normal)));
  }
}

VertexCurve3D VertexCurve3D::rotate(const D3DXQUATERNION &rot) const {
  VertexCurve3D result(size());
  for(const VertexNormal v : *this) {
    result.add(VertexNormal(::rotate(v.getPos(), rot), ::rotate(v.getNormal(), rot)));
  }
  return result;
}

VertexProfile3D::VertexProfile3D(const VertexProfile2D &p, const Point2DTo3DConverter &converter) {
  const size_t    n = p.size();
  setCapacity(n);
  for (size_t i = 0; i < n; i++) {
    add(VertexCurve3D(p[i], converter));
  }
}

VertexProfile3D VertexProfile3D::rotate(const D3DXQUATERNION &rot) const {
  const size_t    n = size();
  VertexProfile3D result(n);
  for (size_t i = 0; i < n; i++) {
    result.add((*this)[i].rotate(rot));
  }
  return result;
}

D3Cube VertexProfile3D::getBoundingBox() const {
  const VertexProfile3D &p = *this;
  const size_t n = size();
  if(n == 0) return D3Cube();
  D3Cube result = ::getBoundingBox(p[0]);
  for(size_t i = 1; i < n; i++) {
    result += ::getBoundingBox(p[i]);
  }
  return result;
}

