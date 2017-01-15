#include "stdafx.h"

using namespace ThreeD;

void BoundingBox::merge(const BoundingBox &bbox2) {
  const D3DXVECTOR3 &vmin = bbox2.m_vmin;
  const D3DXVECTOR3 &vmax = bbox2.m_vmax;

  if(isNullVector(m_vmin) && isNullVector(m_vmax)) {
    m_vmin = vmin;
    m_vmax = vmax;
    return;
  }

  float xmin = min(vmin.x, m_vmin.x);
  float ymin = min(vmin.y, m_vmin.y);
  float zmin = min(vmin.z, m_vmin.z);
  float xmax = max(vmax.x, m_vmax.x);
  float ymax = max(vmax.y, m_vmax.y);
  float zmax = max(vmax.z, m_vmax.z);

  m_vmin = D3DXVECTOR3(xmin, ymin, zmin);
  m_vmax = D3DXVECTOR3(xmax, ymax, zmax);
}

void BoundingBox::fminmax(float f1, float f2, float *fmin, float *fmax) {
  if(f1 < f2) {
    *fmin = f1;
    *fmax = f2;
  } else {
    *fmin = f2;
    *fmax = f1;
  }
}

void BoundingBox::vminmax(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *vmin, D3DXVECTOR3 *vmax) {
  float xmin, ymin, zmin;
  float xmax, ymax, zmax;
  fminmax(v1.x, v2.x, &xmin, &xmax);
  fminmax(v1.y, v2.y, &ymin, &ymax);
  fminmax(v1.z, v2.z, &zmin, &zmax);
  *vmin = D3DXVECTOR3(xmin, ymin, zmin);
  *vmax = D3DXVECTOR3(xmax, ymax, zmax);
}

bool BoundingBox::splitByPoint(const D3DXVECTOR3 &point, std::list<BoundingBox> *outBoxes) const {
  // skip processing if point is outside bounding box
  if(point.x < m_vmin.x || point.x > m_vmax.x
  || point.y < m_vmin.y || point.y > m_vmax.y
  || point.z < m_vmin.z || point.z > m_vmax.z) {
    return false;
  }

  // skip processing if point is one of the eight corners
  // of the bounding box
  if((point.x == m_vmin.x || point.x == m_vmax.x)
  && (point.y == m_vmin.y || point.y == m_vmax.y)
  && (point.z == m_vmin.z || point.z == m_vmax.z)) {
    return false;
  }

  // construct new bounding boxes, two boxes at a time,
  // the near box followed by the far box
  BoundingBox bboxes[8];
  int num = 0;
  D3DXVECTOR3 vmin, vmax;

  // bottom left
  bboxes[num].m_vmin = D3DXVECTOR3(m_vmin.x, m_vmin.y, m_vmin.z);
  bboxes[num].m_vmax = D3DXVECTOR3(point.x, point.y, point.z);
  ++num;

  bboxes[num].m_vmin = D3DXVECTOR3(m_vmin.x, m_vmin.y, point.z);
  bboxes[num].m_vmax = D3DXVECTOR3(point.x, point.y, m_vmax.z);
  ++num;

  // bottom right
  bboxes[num].m_vmin = D3DXVECTOR3(point.x, m_vmin.y, m_vmin.z);
  bboxes[num].m_vmax = D3DXVECTOR3(m_vmax.x, point.y, point.z);
  ++num;

  bboxes[num].m_vmin = D3DXVECTOR3(point.x, m_vmin.y, point.z);
  bboxes[num].m_vmax = D3DXVECTOR3(m_vmax.x, point.y, m_vmax.z);
  ++num;

  // top left
  bboxes[num].m_vmin = D3DXVECTOR3(m_vmin.x, point.y, m_vmin.z);
  bboxes[num].m_vmax = D3DXVECTOR3(point.x, m_vmax.y, point.z);
  ++num;

  bboxes[num].m_vmin = D3DXVECTOR3(m_vmin.x, point.y, point.z);
  bboxes[num].m_vmax = D3DXVECTOR3(point.x, m_vmax.y, m_vmax.z);
  ++num;

  // top right
  bboxes[num].m_vmin = D3DXVECTOR3(point.x, point.y, m_vmin.z);
  bboxes[num].m_vmax = D3DXVECTOR3(m_vmax.x, m_vmax.y, point.z);
  ++num;

  bboxes[num].m_vmin = D3DXVECTOR3(point.x, point.y, point.z);
  bboxes[num].m_vmax = D3DXVECTOR3(m_vmax.x, m_vmax.y, m_vmax.z);
  ++num;

  // add those boxes that aren't degenerate
  for(int i = 0; i < 8; ++i) {
    const BoundingBox &bbox = bboxes[i];
    if(bbox.m_vmin.x == bbox.m_vmax.x
    || bbox.m_vmin.y == bbox.m_vmax.y
    || bbox.m_vmin.z == bbox.m_vmax.z) {
      continue;
    }
    outBoxes->push_back(bbox);
  }

  return true;
}

bool BoundingBox::splitByBox(const BoundingBox &bbox2, std::list<BoundingBox> *outBoxes) const {
  const D3DXVECTOR3 &vmin2 = bbox2.vmin();
  const D3DXVECTOR3 &vmax2 = bbox2.vmax();

  // compute the coordinates of the eight corners of bbox2
  D3DXVECTOR3 corners[8];
  int num = 0;
  // bottom left
  corners[num++] = vmin2;
  corners[num++] = D3DXVECTOR3(vmin2.x, vmin2.y, vmax2.z);
  // bottom right
  corners[num++] = D3DXVECTOR3(vmax2.x, vmin2.y, vmin2.z);
  corners[num++] = D3DXVECTOR3(vmax2.x, vmin2.y, vmax2.z);
  // top left
  corners[num++] = D3DXVECTOR3(vmin2.x, vmax2.y, vmin2.z);
  corners[num++] = D3DXVECTOR3(vmin2.x, vmax2.y, vmax2.z);
  // top right
  corners[num++] = D3DXVECTOR3(vmax2.x, vmax2.y, vmin2.z);
  corners[num++] = D3DXVECTOR3(vmax2.x, vmax2.y, vmax2.z);

  // split this box by the computed corners of bbox2
  std::list<BoundingBox> tempBoxes;
  tempBoxes.push_back(*this);

  bool anySplits = false;

  for(int i = 0; i < 8; ++i) {
    const D3DXVECTOR3 &point = corners[i];
    std::list<BoundingBox>::iterator it = tempBoxes.begin();
    while(it != tempBoxes.end()) {
      const BoundingBox &tempBox = (*it);
      if(tempBox.splitByPoint(point, &tempBoxes)) {
        it = tempBoxes.erase(it);
        anySplits = true;
      } else {
        ++it;
      }
    }
  }

  if(!anySplits) {
    return false;
  }

  outBoxes->splice(outBoxes->end(), tempBoxes);
  return true;
}

bool BoundingBox::splitTwoBoxes(const BoundingBox &bbox1, const BoundingBox &bbox2, std::list<BoundingBox> *outBoxes) {
  std::list<BoundingBox> boxes1, boxes2;

  // split bbox1 by bbox2.  if cannot, then just return the
  // split of bbox2 by bbox1
  if(!bbox1.splitByBox(bbox2, &boxes1)) {
    return bbox2.splitByBox(bbox1, outBoxes);
  }

  std::list<BoundingBox>::const_iterator it1 = boxes1.begin();
  while(it1 != boxes1.end()) {
    const BoundingBox &box = (*it1);
    ++it1;
    bbox2.splitByBox(box, &boxes2);
  }

  // find and remove duplicate boxes
  it1 = boxes1.begin();
  while(it1 != boxes1.end()) {
    const BoundingBox &box1 = (*it1);
    ++it1;

    std::list<BoundingBox>::iterator it2 = boxes2.begin();
    while(it2 != boxes2.end()) {
      const BoundingBox &box2 = (*it2);
      if(box2 == box1) {
        it2 = boxes2.erase(it2);
      } else {
        ++it2;
      }
    }
    outBoxes->push_back(box1);
  }

  //outBoxes->splice(outBoxes->end(), boxes1);
  outBoxes->splice(outBoxes->end(), boxes2);
  return true;
}

void BoundingBox::transform(const D3DXMATRIX &trans) {
  D3DXVECTOR3 v1 = m_vmin;
  D3DXVECTOR3 v8 = m_vmax;
  D3DXVECTOR3 v2(v1.x, v1.y, v8.z);
  D3DXVECTOR3 v3(v1.x, v8.y, v1.z);
  D3DXVECTOR3 v4(v1.x, v8.y, v8.z);
  D3DXVECTOR3 v5(v8.x, v1.y, v1.z);
  D3DXVECTOR3 v6(v8.x, v1.y, v8.z);
  D3DXVECTOR3 v7(v8.x, v8.y, v1.z);

  v1 = trans * v1;
  v2 = trans * v2;
  v3 = trans * v3;
  v4 = trans * v4;
  v5 = trans * v5;
  v6 = trans * v6;
  v7 = trans * v7;
  v8 = trans * v8;

  /*merge(v1, v2);
  merge(v3, v4);
  merge(v5, v6);
  merge(v7, v8);*/

  set(v1, v8);
  merge(v2, v7);
  merge(v3, v6);
  merge(v4, v5);
}
