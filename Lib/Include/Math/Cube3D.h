#pragma once

#include <NumberInterval.h>
#include <Math/Point3D.h>

template<typename T> class Cube3DTemplate {
private:
  // lbn = left,bottom,near, rtf = lbn+size
  Point3DTemplate<T> m_lbn;
  Size3DTemplate<T>  m_size;
public:
  Cube3DTemplate() : m_lbn(0,0,0), m_size(0,0,0) {
  }

  template<typename X, typename Y, typename Z, typename W, typename D, typename H> Cube3DTemplate(const X &x, const Y &y, const Z &z, const W &w, const D &d, const H &h)
    : m_lbn(x, y, z)
    , m_size(w, d, h)
  {
  }
  template<typename C> Cube3DTemplate(const Cube3DTemplate<C> &src)
    : m_lbn(src.LBN())
    , m_size(src.size())
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const Point3DTemplate<T1> &lbn, const Point3DTemplate<T2> &rtf)
    : m_lbn(lbn)
    , m_size(rtf-lbn)
  {
  }
  template<typename P, typename S> Cube3DTemplate(const Point3DTemplate<P> &lbn, const Size3DTemplate<S> &size)
    : m_lbn(lbn)
    , m_size(size)
  {
  }
  Cube3DTemplate(const NumberInterval<T> &xInterval, const NumberInterval<T> &yInterval, const NumberInterval<T> &zInterval)
    : m_lbn(xInterval.getMin(), yInterval.getMin(), zInterval.getMin())
    , m_size(xInterval.getLength(), yInterval.getLength(), zInterval.getLength())
  {
  }
  // x-axis
  inline const T &Left() const {
    return m_lbn.x;
  }
  // x-axis
  inline const T &Width() const {
    return m_size.cx;
  }
  // x-axis
  inline T Right() const {
    return Left() + Width();
  }
  // y-axis
  inline const T &Near() const {
    return m_lbn.y;
  }
  // y-axis
  inline const T &Depth() const {
    return m_size.cy;
  }
  // y-axis
  inline T Far() const {
    return Near() + Depth();
  }
  // z-axis
  inline const T &Bottom() const {
    return m_lbn.z;
  }
  // z-axis
  inline const T &Height() const {
    return m_size.cz;
  }
  // z-axis
  inline T Top() const {
    return Bottom() + Height();
  }
  inline const Size3DTemplate<T> &size() const {
    return m_size;
  }
  inline T volume() const {
    return m_size.volume();
  }
  inline const Point3DTemplate<T> &LBN() const {
    return m_lbn;
  }
  inline Point3DTemplate<T> RBN() const {
    return Point3DTemplate<T>(Right(), Near(), Bottom());
  }
  inline Point3DTemplate<T> LTN() const {
    return Point3DTemplate<T>(Left(), Near(), Top());
  }
  inline Point3DTemplate<T> RTN() const {
    return Point3DTemplate<T>(Right(), Near(), Top());
  }
  inline Point3DTemplate<T> LBF() const {
    return Point3DTemplate<T>(Left(), Far(), Bottom());
  }
  inline Point3DTemplate<T> RBF() const {
    return Point3DTemplate<T>(Right(), Far(), Bottom());
  }
  inline Point3DTemplate<T> LTF() const {
    return Point3DTemplate<T>(Left(), Far(), Top());
  }
  inline Point3DTemplate<T> RTF() const {
    return Point3DTemplate<T>(m_lbn + m_size);
  }
  inline Point3DTemplate<T> center() const {
    return Point3DTemplate<T>(m_lbn + m_size/2);
  }
  inline T getMinX() const {
    return min(Left(),Right());
  }
  inline T getMaxX() const {
    return max(Left(), Right());
  }
  inline T getMinY() const {
    return min(Near(), Far());
  }
  inline T getMaxY() const {
    return max(Near(), Far());
  }
  inline T getMinZ() const {
    return min(Bottom(), Top());
  }
  inline T getMaxZ() const {
    return max(Bottom(), Top());
  }

  inline NumberInterval<T> getXInterval() const {
    return NumberInterval<T>(getMinX(), getMaxX());
  }
  inline NumberInterval<T> getYInterval() const {
    return NumberInterval<T>(getMinY(), getMaxY());
  }
  inline NumberInterval<T> getZInterval() const {
    return NumberInterval<T>(getMinZ(), getMaxZ());
  }
  template<typename TP> bool contains(const Point3DTemplate<TP> &p) const {
    return (getMinX() <= (T)p.x) && ((T)p.x <= getMaxX()) && (getMinY() <= (T)p.y) && ((T)p.y <= getMaxY()) && (getMinZ() <= (T)p.z) && ((T)p.z <= getMaxZ());
  }

  template<typename TP> Cube3DTemplate<T> &operator+=(const Point3DTemplate<TP> &dp) {
    m_lbn += dp;
    return *this;
  }
  template<typename TP> Cube3DTemplate<T> &operator-=(const Point3DTemplate<TP> &dp) {
    m_lbn -= dp;
    return *this;
  }
  template<typename TP> Cube3DTemplate<T> operator+(const Point3DTemplate<TP> &dp) {
    return Cube3DTemplate(LBN() + dp, size());
  }
  template<typename TP> Cube3DTemplate<T> operator-(const Point3DTemplate<TP> &dp) {
    return Cube3DTemplate(LBN() - dp, size());
  }
  inline bool operator==(const Cube3DTemplate &c) const {
    return (LBN() == c.LBN()) && (size() == c.size());
  }
  inline bool operator!=(const Cube3DTemplate &c) const {
    return !(*this == c);
  }
  inline bool contains(const Cube3DTemplate &c) const {
    return contains(c.LBN()) && contains(c.RTF());
  }
  inline String toString(int dec = 3) const {
    return format(_T("Cube:(%s,%s)"), LBN().toString(dec).cstr(), RTF().toString(dec).cstr());
  }
};

typedef Cube3DTemplate<float>  FloatCube3D;
typedef Cube3DTemplate<double> Cube3D;

template<typename T1, typename T2> Cube3DTemplate<T1> getUnion(const Cube3DTemplate<T1> &c1, const Cube3DTemplate<T2> &c2) {
  const T1 minX = min(c1.getMinX(), c2.getMinX()), maxX = max(c1.getMaxX(), c2.getMaxX());
  const T1 minY = min(c1.getMinY(), c2.getMinY()), maxY = max(c1.getMaxY(), c2.getMaxY());
  const T1 minZ = min(c1.getMinZ(), c2.getMinZ()), maxZ = max(c1.getMaxZ(), c2.getMaxZ());
  return Cube3DTemplate<T1>(minX, minY, minZ, maxX - minX, maxY - minY, maxZ - minZ);
}


template<typename T> class Point3DTemplateArray : public CompactArray<Point3DTemplate<T> > {
public:
  Point3DTemplateArray() {
  }
  explicit Point3DTemplateArray(size_t capacity) : CompactArray(capacity) {
  }
  template<typename S> Point3DTemplateArray(const Point3DTemplateArray<S> &src) : CompactArray(src.size) {
    const size_t n = src.size();
    for(const Point3DTemplate<S> p : src) {
      add(Point3DTemplate<T>(p));
    }
  }
  template<typename S> Point3DTemplateArray<T> &operator=(const Point3DTemplateArray<S> &src) {
    if((void*)&src == (void*)this) {
      return *this;
    }
    clear(src.size());
    for(const Point3DTemplate<S> p : src) {
      add(Point3DTemplate<T>(p));
    }
    return *this;
  }
  Cube3DTemplate<T> getBoundingBox() const {
    if(size() == 0) {
      return Cube3DTemplate<T>();
    }

    const Point3DTemplate<T> *p   = &first();
    const Point3DTemplate<T> *end = &last();
    T minX = p->x, maxX = minX, minY = p->y, maxY = minY, minZ = p->z, maxZ = minZ;
    while(p++ < end) {
      if(p->x < minX) minX = p->x; else if(p->x > maxX) maxX = p->x;
      if(p->y < minY) minY = p->y; else if(p->y > maxY) maxY = p->y;
      if(p->z < minZ) minZ = p->z; else if(p->z > maxZ) maxZ = p->z;
    }
    return Cube3DTemplate<T>(minX,minY,minZ, maxX-minX, maxY-minY,maxZ-minZ);
  }
};

typedef Point3DTemplateArray<float   > FloatPoint3DArray;
typedef Point3DTemplateArray<double  > Point3DArray;
typedef Point3DTemplateArray<Double80> D80Point3DArray;
typedef Point3DTemplateArray<Real>     RealPoint3DArray;
