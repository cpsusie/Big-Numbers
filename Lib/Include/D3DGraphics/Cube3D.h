#pragma once

#include <Math/Point3D.h>
#include <D3DX9.h>

template<class C, class V> class Cube3DTemplate {
public:
  C m_lbn;
  C m_rtf;
  Cube3DTemplate() {
  }
  Cube3DTemplate(const C &lbn, const C &rtf) : m_lbn(lbn), m_rtf(rtf) {
  }
  inline V getLength() const { // dimension x
    return m_rtf.x - m_lbn.x;
  }
  inline V getHeight() const { // dimension y
    return m_rtf.y - m_lbn.y;
  }
  inline V getDepth() const {  // dimension z
    return m_rtf.z - m_lbn.z;
  }
  inline V volume() const {
    return getLength() * getHeight() * getDepth();
  }
  inline V getMinX() const {
    return m_lbn.x;
  }
  inline V getMaxX() const {
    return m_rtf.x;
  }
  inline V getMinY() const {
    return m_lbn.y;
  }
  inline V getMaxY() const {
    return m_rtf.y;
  }
  inline V getMinZ() const {
    return m_lbn.z;
  }
  inline V getMaxZ() const {
    return m_rtf.z;
  }
  inline bool contains(const C &p) const {
    return (m_lbn <= p) && (p <= m_rtf);
  }
  inline bool contains(const Cube3DTemplate &cube) const {
    return (m_lbn <= cube.m_lbn) && (cube.m_rtf <= m_rtf);
  }
  inline C getCenter() const {
    return (m_lbn + m_rtf)/2.0;
  }
  inline String toString(int dec = 3) const {
    return format(_T("Cube:(%s,%s)"), ::toString(m_lbn, dec).cstr(), ::toString(m_rtf, dec).cstr());
  }
};

typedef Cube3DTemplate<Point3D    , double> Cube3D;
typedef Cube3DTemplate<D3DXVECTOR3, float>  D3DXCube3;
