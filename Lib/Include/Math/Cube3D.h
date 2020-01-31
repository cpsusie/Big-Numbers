#pragma once

#include <NumberInterval.h>
#include <Math/Point3D.h>

template<typename T> class Cube3DTemplate {
private:
  // lbn = (m_x,m_y,m_z), rtf = (m_x+m_w,m_y+m_h,m_z+m_d)
  T m_x, m_y, m_z, m_w, m_h, m_d;
public:
  Cube3DTemplate() : m_x(0), m_y(0), m_z(0), m_w(0), m_h(0), m_d(0)
  {
  }

  Cube3DTemplate(const T &x, const T &y, const T &z, const T &w, const T &h, const T &d)
    : m_x(x), m_y(y), m_z(z), m_w(w), m_h(h), m_d(d)
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const Point3DTemplate<T1> &lbn, const Point3DTemplate<T2> &rtf)
    : m_x((T)lbn.x), m_y((T)lbn.y), m_z((T)lbn.z)
    , m_w((T)rtf.x - (T)lbn.x) , m_h((T)rtf.y - (T)lbn.y), m_d((T)rtf.z - (T)lbn.z)
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const Point3DTemplate<T1> &p, const Size3DTemplate<T2> &size)
    : m_x((T)p.x), m_y((T)p.y), m_z((T)p.z)
    , m_w((T)size.cx), m_h((T)size.cy), m_d((T)size.cx)
  {
  }
  Cube3DTemplate(const NumberInterval<T> &xInterval, const NumberInterval<T> &yInterval, const NumberInterval<T> &zInterval)
    : m_x(xInterval.getMin()), m_y(yInterval.getMin()), m_z(zInterval.getMin())
    , m_w(xInterval.getLength()), m_h(yInterval.getLength()), m_d(zInterval.getLength())
  {
  }
  inline const T &getX() const {
    return m_x;
  }
  inline const T &getY() const {
    return m_y;
  }
  inline const T &getZ() const {
    return m_z;
  }
  inline const T &getWidth() const {
    return m_w;
  }
  inline const T &getHeight() const {
    return m_h;
  }
  inline const T &getDepth() const {
    return m_d;
  }
  inline Size3DTemplate<T> getSize() const {
    return Size3DTemplate<T>(m_w, m_h, m_d);
  }
  inline T getVolume() const {
    return getWidth() * getHeight() * getDepth();
  }
  inline Point3DTemplate<T> getLBN() const {
    return Point3DTemplate<T>(m_x, m_y, m_z);
  }
  inline Point3DTemplate<T> getRBN() const {
    return Point3DTemplate<T>(m_x + m_w, m_y, m_z);
  }
  inline Point3DTemplate<T> getLTN() const {
    return Point3DTemplate<T>(m_x, m_y + m_h, m_z);
  }
  inline Point3DTemplate<T> getRTN() const {
    return Point3DTemplate<T>(m_x + m_w, m_y + m_h, m_z);
  }
  inline Point3DTemplate<T> getLBF() const {
    return Point3DTemplate<T>(m_x, m_y, m_z + m_d);
  }
  inline Point3DTemplate<T> getRBF() const {
    return Point3DTemplate<T>(m_x + m_w, m_y, m_z + m_d);
  }
  inline Point3DTemplate<T> getLTF() const {
    return Point3DTemplate<T>(m_x, m_y + m_h, m_z + m_d);
  }
  inline Point3DTemplate<T> getRTF() const {
    return Point3DTemplate<T>(m_x + m_w, m_y + m_h, m_z + m_d);
  }
  inline Point3DTemplate<T> getCenter() const {
    return Point3DTemplate<T>(m_x + m_w / 2, m_y + m_h / 2, m_z + m_d / 2);
  }
  inline T getMinX() const {
    return min(m_x, m_x + m_w);
  }
  inline T getMaxX() const {
    return max(m_x, m_x + m_w);
  }
  inline T getMinY() const {
    return min(m_y, m_y + m_h);
  }
  inline T getMaxY() const {
    return max(m_y, m_y + m_h);
  }
  inline T getMinZ() const {
    return min(m_z, m_z + m_d);
  }
  inline T getMaxZ() const {
    return max(m_z, m_z + m_d);
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
    m_x += (T)dp.x; m_y += (T)dp.y; m_z += (T)dp.z;
    return *this;
  }
  template<typename TP> Cube3DTemplate &operator-=(const Point3DTemplate<TP> &dp) {
    m_x -= (T)dp.x; m_y -= (T)dp.y; m_z -= (T)dp.z;
    return *this;
  }
  template<typename TP> Cube3DTemplate operator+(const Point3DTemplate<TP> &dp) {
    return Cube3DTemplate(getLBN() + dp, getSize());
  }
  template<typename TP> Cube3DTemplate operator-(const Point3DTemplate<TP> &dp) {
    return Cube3DTemplate(getLBN() - dp, getSize());
  }
  inline bool operator==(const Cube3DTemplate &c) const {
    return (m_x == c.m_x) && (m_y == c.m_y) && (m_z == c.m_z) && (m_w == c.m_w) && (m_h == c.m_h) && (m_d == c.m_d);
  }
  inline bool operator!=(const Cube3DTemplate &c) const {
    return !(*this == c);
  }

  inline bool contains(const Cube3DTemplate &c) const {
    return contains(c.getLBN()) && contains(c.getRTF());
  }
  inline String toString(int dec = 3) const {
    return format(_T("Cube:(%s,%s)"), getLBN().toString(dec).cstr(), getRTF().toString(dec).cstr());
  }
};

typedef Cube3DTemplate<double> Cube3D;
typedef Cube3DTemplate<float>  FloatCube3D;
