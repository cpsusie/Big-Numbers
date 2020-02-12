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

  Cube3DTemplate(const T &x, const T &y, const T &z, const T &w, const T &h, const T &d)
    : m_lbn(T(x), T(y), T(z)), m_size(T(w), T(h), T(d))
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const Point3DTemplate<T1> &lbn, const Point3DTemplate<T2> &rtf)
    : m_lbn(lbn), m_size(rtf-lbn)
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const Point3DTemplate<T1> &lbn, const Size3DTemplate<T2> &size)
    : m_lbn(lbn), m_size(size)
  {
  }
  Cube3DTemplate(const NumberInterval<T> &xInterval, const NumberInterval<T> &yInterval, const NumberInterval<T> &zInterval)
    : m_lbn(xInterval.getMin(), yInterval.getMin(), zInterval.getMin())
    , m_size(xInterval.getLength(), yInterval.getLength(), zInterval.getLength())
  {
  }
  inline const T &Left() const {
    return m_lbn.x;
  }
  inline T Right() const {
    return Left() + Width();
  }
  inline const T &Bottom() const {
    return m_lbn.y;
  }
  inline T Top() const {
    return Bottom() + Height();
  }
  inline const T &Near() const {
    return m_lbn.z;
  }
  inline T Far() const {
    return Near() + Depth();
  }
  inline const T &Width() const {
    return m_size.cx;
  }
  inline const T &Height() const {
    return m_size.cy;
  }
  inline const T &Depth() const {
    return m_size.cz;
  }
  inline const Size3DTemplate<T> &size() const {
    return m_size;
  }
  inline T volume() const {
    return m_size.volume();
  }
  inline Point3DTemplate<T> LBN() const {
    return m_lbn;
  }
  inline Point3DTemplate<T> RBN() const {
    return Point3DTemplate<T>(Right(), Bottom(), Near());
  }
  inline Point3DTemplate<T> LTN() const {
    return Point3DTemplate<T>(left(), Top(), Near());
  }
  inline Point3DTemplate<T> RTN() const {
    return Point3DTemplate<T>(Right(), Top(), Near());
  }
  inline Point3DTemplate<T> LBF() const {
    return Point3DTemplate<T>(Left(), Bottom(), Far());
  }
  inline Point3DTemplate<T> RBF() const {
    return Point3DTemplate<T>(Right(), Bottom(), Far());
  }
  inline Point3DTemplate<T> LTF() const {
    return Point3DTemplate<T>(Left(), Top(), Far());
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
    return min(Bottom(), Top());
  }
  inline T getMaxY() const {
    return max(Bottom(), Top());
  }
  inline T getMinZ() const {
    return min(Near(), Far());
  }
  inline T getMaxZ() const {
    return max(Near(), Far());
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

  template<typename TP> Cube3DTemplate &operator+=(const Point3DTemplate<TP> &dp) {
    m_lbn += dp;
    return *this;
  }
  template<typename TP> Cube3DTemplate &operator-=(const Point3DTemplate<TP> &dp) {
    m_lbn -= dp;
    return *this;
  }
  template<typename TP> Cube3DTemplate operator+(const Point3DTemplate<TP> &dp) {
    return Cube3DTemplate(LBN() + dp, size());
  }
  template<typename TP> Cube3DTemplate operator-(const Point3DTemplate<TP> &dp) {
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

typedef Cube3DTemplate<double> Cube3D;
typedef Cube3DTemplate<float>  FloatCube3D;
