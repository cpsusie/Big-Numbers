#pragma once

#include <NumberInterval.h>
#include <Math/Point3D.h>
#include <D3DX9.h>

template<class C, class V> class Cube3DTemplate {
  private:
  C m_lbn;
  C m_rtf;
public:
  Cube3DTemplate() {
  }
  inline Cube3DTemplate(const C &lbn, const C &rtf) : m_lbn(lbn), m_rtf(rtf) {
  }
  inline Cube3DTemplate(V minX, V minY, V minZ, V maxX, V maxY, V maxZ)
    : m_lbn(minX, minY, minZ)
    , m_rtf(maxX, maxY, maxZ)
  {
  }
  inline Cube3DTemplate(const NumberInterval<V> &xInterval, const NumberInterval<V> &yInterval, const NumberInterval<V> &zInterval) 
    : m_lbn(xInterval.getMin(), yInterval.getMin(), zInterval.getMin())
    , m_rtf(xInterval.getMax(), yInterval.getMax(), zInterval.getMax())
  {
  }
  // x-dimension
  inline V getLength() const {
    return m_rtf.x - m_lbn.x;
  }
  // y-dimension
  inline V getHeight() const {
    return m_rtf.y - m_lbn.y;
  }
  // z-dimension
  inline V getDepth() const {
    return m_rtf.z - m_lbn.z;
  }
  inline V getVolume() const {
    return getLength() * getHeight() * getDepth();
  }
  inline void setMinX(V v) {
    m_lbn.x = v;
  }
  inline void setMaxX(V v) {
    m_rtf.x = v;
  }
  inline void setMinY(V v) {
    m_lbn.y = v;
  }
  inline void setMaxY(V v) {
    m_rtf.y = v;
  }
  inline void setMinZ(V v) {
    m_lbn.z = v;
  }
  inline void setMaxZ(V v) {
    m_rtf.z = v;
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
  inline void setXInterval(const NumberInterval<V> &interval) {
    setMinX(interval.getMin()); setMaxX(interval.getMax());
  }
  inline void setYInterval(const NumberInterval<V> &interval) {
    setMinY(interval.getMin()); setMaxY(interval.getMax());
  }
  inline void setZInterval(const NumberInterval<V> &interval) {
    setMinZ(interval.getMin()); setMaxZ(interval.getMax());
  }
  inline NumberInterval<V> getXInterval() const {
    return NumberInterval<V>(getMinX(), getMaxX());
  }
  inline NumberInterval<V> getYInterval() const {
    return NumberInterval<V>(getMinY(), getMaxY());
  }
  inline NumberInterval<V> getZInterval() const {
    return NumberInterval<V>(getMinZ(), getMaxZ());
  }
  inline void setMin(const C &p) {
    m_lbn = p; 
  }
  inline void setMax(const C &p) {
    m_rtf = p; 
  }
  inline const C &getMin() const {
    return m_lbn;
  }
  inline const C &getMax() const {
    return m_rtf;
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
