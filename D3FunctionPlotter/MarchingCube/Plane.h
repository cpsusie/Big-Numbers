#pragma once

namespace ThreeD {

/* Plane, the mathematical entity defined by a point and a normal vector. */
class Plane {
protected:
  double m_a, m_b, m_c, m_d;

public:
  Plane() : m_a(0.0), m_b(0.0), m_c(1.0), m_d(0.0) {
  }
  
  /* Constructs a plane using the components @p a, @p b, @p c and @p d.*/
  Plane(double a, double b, double c, double d) : m_a(a), m_b(b), m_c(c), m_d(d) {
  }
  
  /* Constructs a plane using the three vertices @p v0, @p v1 and @p v2.*/
  Plane(const D3DXVECTOR3 &v0, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    // _abc = (v0 - v1) % (v1 - v2);
    double v0_v1_x = (v0 - v1).x;
    double v0_v1_y = (v0 - v1).y;
    double v0_v1_z = (v0 - v1).z;
    double v1_v2_x = (v1 - v2).x;
    double v1_v2_y = (v1 - v2).y;
    double v1_v2_z = (v1 - v2).z;
    m_a = (v0_v1_y * v1_v2_z) - (v0_v1_z * v1_v2_y);
    m_b = (v0_v1_z * v1_v2_x) - (v0_v1_x * v1_v2_z);
    m_c = (v0_v1_x * v1_v2_y) - (v0_v1_y * v1_v2_x);
    // m_d = -(v0 * _abc);
    m_d = -( (v0.x * m_a) + (v0.y * m_b) + (v0.z * m_c) );
  }

  /* Constructs a plane to be a copy of the @p model plane. */
  Plane(const Plane &model) { operator=(model); }

  /* @return a plane that is a normalization of this plane. */
  Plane normalized() const {
    double len = sqrt(m_a * m_a + m_b * m_b + m_c * m_c);
    if(len == 0.0) {
      len = 1.0;
    }
    return Plane(m_a / len, m_b / len, m_c / len, m_d / len);
  }
  
  double a() const { return m_a; }
  double b() const { return m_b; }
  double c() const { return m_c; }
  double d() const { return m_d; }
  
  const D3DXVECTOR3 normal() const {
    return D3DXVECTOR3((float)m_a, (float)m_b, (float)m_c);
  }
  
  /*
   * @return the product of this plane with the vector @p v.
   *
   * May be used to classify a point (a vector) with regards to the plane:
   * a zero result means the point is on the plane;  a negative result
   * means the point is behind the plane;  a positive result means the
   * point is in front of the plane. 
   */
  double classify(const D3DXVECTOR3 &v) const {
    return - (m_a * v.x + m_b * v.y + m_c * v.z + m_d);
  }
  
  /*
   * Determines if a line bound by @p v1 and @p v2 intersects this plane.
   *
   * If it does, @p vx is set to the point of intersection, and the
   * return value reflects the side of the intersection:  negative if
   * behind, positive if in front.
   *
   * If there is no intersection, a zero is returned.
   *
   * @return the side of the intersection, or zero if no intersection.
   */
  int intersect(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *vx) const;
  
  friend inline bool operator==(const Plane &a, const Plane &b);
  friend inline bool operator!=(const Plane &a, const Plane &b);

  String toString() const {
    return format("Plane:(%lf,%lf,%lf,%lf)", m_a, m_b, m_c, m_d);
  }
};


inline bool operator==(const Plane &a, const Plane &b) {
  return fabs(a.m_a - b.m_a) < 1e-1
      && fabs(a.m_b - b.m_b) < 1e-1
      && fabs(a.m_c - b.m_c) < 1e-1
      && fabs(a.m_d - b.m_d) < 1e-1;
}

inline bool operator!=(const Plane &a, const Plane &b) {
  return fabs(a.m_a - b.m_a) > 1e-1
      || fabs(a.m_b - b.m_b) > 1e-1 
      || fabs(a.m_c - b.m_c) > 1e-1 
      || fabs(a.m_d - b.m_d) > 1e-1;
}


} // namespace ThreeD
