#pragma once

#include <NumberInterval.h>
#include <Math/Point3D.h>
#include <D3DX9.h>

template<typename VertexType, typename Scalar> class Cube3DTemplate {
private:
  VertexType m_lbn, m_rtf; // m_lbn = left-bottom-near, m_rtf = right-top-far
public:
  Cube3DTemplate() {
  }
  inline Cube3DTemplate(const VertexType &lbn, const VertexType &rtf) : m_lbn(lbn), m_rtf(rtf) {
  }
  inline Cube3DTemplate(Scalar minX, Scalar minY, Scalar minZ, Scalar maxX, Scalar maxY, Scalar maxZ)
    : m_lbn(minX, minY, minZ)
    , m_rtf(maxX, maxY, maxZ)
  {
  }
  inline Cube3DTemplate(const NumberInterval<Scalar> &xInterval, const NumberInterval<Scalar> &yInterval, const NumberInterval<Scalar> &zInterval)
    : m_lbn(xInterval.getMin(), yInterval.getMin(), zInterval.getMin())
    , m_rtf(xInterval.getMax(), yInterval.getMax(), zInterval.getMax())
  {
  }
  // x-dimension
  inline Scalar getLength() const {
    return m_rtf.x - m_lbn.x;
  }
  // y-dimension
  inline Scalar getHeight() const {
    return m_rtf.y - m_lbn.y;
  }
  // z-dimension
  inline Scalar getDepth() const {
    return m_rtf.z - m_lbn.z;
  }
  inline Scalar getVolume() const {
    return getLength() * getHeight() * getDepth();
  }
  inline void setMinX(Scalar v) {
    m_lbn.x = v;
  }
  inline void setMaxX(Scalar v) {
    m_rtf.x = v;
  }
  inline void setMinY(Scalar v) {
    m_lbn.y = v;
  }
  inline void setMaxY(Scalar v) {
    m_rtf.y = v;
  }
  inline void setMinZ(Scalar v) {
    m_lbn.z = v;
  }
  inline void setMaxZ(Scalar v) {
    m_rtf.z = v;
  }
  inline Scalar getMinX() const {
    return m_lbn.x;
  }
  inline Scalar getMaxX() const {
    return m_rtf.x;
  }
  inline Scalar getMinY() const {
    return m_lbn.y;
  }
  inline Scalar getMaxY() const {
    return m_rtf.y;
  }
  inline Scalar getMinZ() const {
    return m_lbn.z;
  }
  inline Scalar getMaxZ() const {
    return m_rtf.z;
  }
  inline void setXInterval(const NumberInterval<Scalar> &interval) {
    setMinX(interval.getMin()); setMaxX(interval.getMax());
  }
  inline void setYInterval(const NumberInterval<Scalar> &interval) {
    setMinY(interval.getMin()); setMaxY(interval.getMax());
  }
  inline void setZInterval(const NumberInterval<Scalar> &interval) {
    setMinZ(interval.getMin()); setMaxZ(interval.getMax());
  }
  inline NumberInterval<Scalar> getXInterval() const {
    return NumberInterval<Scalar>(getMinX(), getMaxX());
  }
  inline NumberInterval<Scalar> getYInterval() const {
    return NumberInterval<Scalar>(getMinY(), getMaxY());
  }
  inline NumberInterval<Scalar> getZInterval() const {
    return NumberInterval<Scalar>(getMinZ(), getMaxZ());
  }
  inline void setMin(const VertexType &p) {
    m_lbn = p;
  }
  inline void setMax(const VertexType &p) {
    m_rtf = p;
  }
  inline const VertexType &getMin() const {
    return m_lbn;
  }
  inline const VertexType &getMax() const {
    return m_rtf;
  }
  inline bool contains(const VertexType &p) const {
    return (m_lbn <= p) && (p <= m_rtf);
  }
  inline bool contains(const Cube3DTemplate &cube) const {
    return (m_lbn <= cube.m_lbn) && (cube.m_rtf <= m_rtf);
  }
  inline VertexType getCenter() const {
    return (m_lbn + m_rtf)/2.0;
  }
  inline String toString(int dec = 3) const {
    return format(_T("Cube:(%s,%s)"), ::toString(m_lbn, dec).cstr(), ::toString(m_rtf, dec).cstr());
  }
  inline Cube3DTemplate<VertexType, Scalar> operator+(const VertexType &offset) {
    return Cube3DTemplate<VertexType, Scalar>(m_lbn + offset, m_rtf + offset);
  }
  inline Cube3DTemplate<VertexType, Scalar> operator-(const VertexType &offset) {
    return Cube3DTemplate<VertexType, Scalar>(m_lbn - offset, m_rtf - offset);
  }
  static Cube3DTemplate<typename VertexType, typename Scalar> getSquareCube(const VertexType &center, Scalar sideLength) {
    if(sideLength < 0) sideLength = -sideLength;
    const Scalar S05 = sideLength / (Scalar)2;
    const VertexType D05(S05, S05, S05); // half diagonal
    return Cube3DTemplate<VertexType, Scalar>(center - D05, center + D05);
  }
  static Cube3DTemplate<typename VertexType, typename Scalar> getStdCube() {
    return Cube3DTemplate<VertexType, Scalar>::getSquareCube(VertexType((Scalar)0, (Scalar)0, (Scalar)0), (Scalar)2);
  }
};

typedef Cube3DTemplate<Point3D    , double> Cube3D;
typedef Cube3DTemplate<D3DXVECTOR3, float>  D3DXCube3;
