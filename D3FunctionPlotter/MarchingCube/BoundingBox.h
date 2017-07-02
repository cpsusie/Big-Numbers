#pragma once

#include <list>

namespace ThreeD {

class BoundingBox {
private:
  D3DXVECTOR3 m_vmin, m_vmax;

public:
  BoundingBox() : m_vmin(0,0,0), m_vmax(0,0,0) {
  }

  BoundingBox(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    set(v1, v2);
  }

  /* Sets the bounds of this bounding box. */
  void set(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    vminmax(v1, v2, &m_vmin, &m_vmax);
  }

  /* Merges the specified bounding box into this bounding box. */
  void merge(const BoundingBox &bbox2);

  /* Merges the specified bounds into this bounding box. */
  void merge(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    BoundingBox bbox;
    vminmax(v1, v2, &bbox.m_vmin, &bbox.m_vmax);
    merge(bbox);
  }

  /* Returns the minimum and maximum of two values */
  static inline void fminmax(float f1, float f2, float *fmin, float *fmax);

  /* Returns the minimum and maximum of two vectors */
  static void vminmax(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *vmin, D3DXVECTOR3 *vmax);

  /* Split the bounding box by a point.  The operation will
   * result in eight bounding boxes covering the volume of
   * the original bounding box.  Each one of the eight
   * boxes will have the point as one of its corners.
   */
  inline bool splitByPoint(const D3DXVECTOR3 &point, std::list<BoundingBox> *outBoxes) const;

  /* Split the bounding box by another bounding box.  The
   * operation will result in a number of bounding boxes
   * covering the volume of the original bounding boxes,
   * but no two boxes will share the same volume.
   */
  bool splitByBox(const BoundingBox &bbox2, std::list<BoundingBox> *outBoxes) const;

  static bool BoundingBox::splitTwoBoxes(const BoundingBox &bbox1, const BoundingBox &bbox2, std::list<BoundingBox> *outBoxes);

  void transform(const D3DXMATRIX &trans);

  const D3DXVECTOR3 &vmin() const { return m_vmin; }
  const D3DXVECTOR3 &vmax() const { return m_vmax; }

  D3DXVECTOR3 vTopLeftNear()  const { return D3DXVECTOR3(m_vmin.x, m_vmax.y, m_vmin.z); }
  D3DXVECTOR3 vTopLeftFar()   const { return D3DXVECTOR3(m_vmin.x, m_vmax.y, m_vmax.z); }
  D3DXVECTOR3 vBotLeftNear()  const { return m_vmin; }
  D3DXVECTOR3 vBotLeftFar()   const { return D3DXVECTOR3(m_vmin.x, m_vmin.y, m_vmax.z); }
  D3DXVECTOR3 vTopRightNear() const { return D3DXVECTOR3(m_vmax.x, m_vmax.y, m_vmin.z); }
  D3DXVECTOR3 vTopRightFar()  const { return m_vmax; }
  D3DXVECTOR3 vBotRightNear() const { return D3DXVECTOR3(m_vmax.x, m_vmin.y, m_vmin.z); }
  D3DXVECTOR3 vBotRightFar()  const { return D3DXVECTOR3(m_vmax.x, m_vmin.y, m_vmax.z); }

  float width()  const { return (m_vmax.x - m_vmin.x); }
  float height() const { return (m_vmax.y - m_vmin.y); }
  float depth()  const { return (m_vmax.z - m_vmin.z); }

  /* @return true if this bounding box is wholly contained by
   * a larger bounding box @p bbox2
   */
  bool isInside(const BoundingBox &bbox2) const {
    const D3DXVECTOR3 &vmin1 = m_vmin;
    const D3DXVECTOR3 &vmax1 = m_vmax;
    const D3DXVECTOR3 &vmin2 = bbox2.m_vmin;
    const D3DXVECTOR3 &vmax2 = bbox2.m_vmax;

    return vmin2.x >= vmin1.x && vmax2.x <= vmax1.x
        && vmin2.y >= vmin2.y && vmax2.y <= vmax2.y
        && vmin2.z >= vmin2.z && vmax2.z <= vmax2.z;
  }

  /* @return true if this bounding box is overlapping
   * with another box @p bbox2
   */
  bool overlapping(const BoundingBox &bbox2) const {
    return overlapping(*this, bbox2);
  }

  /* @return true if two bounding boxes are overlapping  */
  static bool overlapping(const BoundingBox &bbox1, const BoundingBox &bbox2) {
    const D3DXVECTOR3 &vmin1 = bbox1.m_vmin;
    const D3DXVECTOR3 &vmax1 = bbox1.m_vmax;
    const D3DXVECTOR3 &vmin2 = bbox2.m_vmin;
    const D3DXVECTOR3 &vmax2 = bbox2.m_vmax;

    bool non_overlapping = vmax1.x <= vmin2.x || vmax2.x <= vmin1.x
                        || vmax1.y <= vmin2.y || vmax2.y <= vmin1.y
                        || vmax1.z <= vmin2.z || vmax2.z <= vmin1.z;
    return !non_overlapping;
  }

  /* @return the intersection bounding box of two bounding boxes
   * @p bbox1 and @p bbox2 which are known to be overlapping
   */
  static BoundingBox intersection(const BoundingBox &bbox1, const BoundingBox &bbox2) {
    const D3DXVECTOR3 &vmin1 = bbox1.m_vmin;
    const D3DXVECTOR3 &vmax1 = bbox1.m_vmax;
    const D3DXVECTOR3 &vmin2 = bbox2.m_vmin;
    const D3DXVECTOR3 &vmax2 = bbox2.m_vmax;

    BoundingBox bbox;
    bbox.m_vmin = D3DXVECTOR3(max(vmin1.x, vmin2.x), max(vmin1.y, vmin2.y), max(vmin1.z, vmin2.z));
    bbox.m_vmax = D3DXVECTOR3(min(vmax1.x, vmax2.x), min(vmax1.y, vmax2.y), min(vmax1.z, vmax2.z));

    return bbox;
  }

  bool operator==(const BoundingBox &other) const {
    return (m_vmin == other.m_vmin && m_vmax == other.m_vmax);
  }

  bool operator!=(const BoundingBox &other) const {
    return (m_vmin != other.m_vmin || m_vmax != other.m_vmax);
  }
};

} // namespace ThreeD
