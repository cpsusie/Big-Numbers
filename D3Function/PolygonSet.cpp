#include "stdafx.h"
#include "PolygonSet.h"

void PolygonSet::repaint(Viewport2D &vp, ProfileEditorState state) {
  if(isEmpty()) {
    return;
  }

  switch(state) {
  case IDLE       :
  case DRAGGING   :
    break;

  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    m_boundingBox.repaint(vp, state);
    break;
  }
}

Point2DRefArray PolygonSet::getPointRefArray() {
  Point2DRefArray result;
  for(size_t i = 0; i < size(); i++) {
    result.addAll((*this)[i]->getAllPointsRef());
  }
  return result;
}

void PolygonSet::move(const Point2D &dp) {
  m_boundingBox.move(dp, getPointRefArray());
}

void PolygonSet::stretch(const Point2D &dp) {
  m_boundingBox.stretch(dp, getPointRefArray());
}

void PolygonSet::rotate(const Point2D &dp) {
  Point2DRefArray pa = getPointRefArray();
  m_boundingBox.rotate(dp, pa);
}

void PolygonSet::mirror(bool horizontal) {
  m_boundingBox.mirror(horizontal, getPointRefArray());
  invertNormals();
}

void PolygonSet::invertNormals() {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i]->reverseOrder();
  }
}

void PolygonSet::evaluateBox() {
  if(!isEmpty()) {
    Rectangle2D box = first()->getBoundingBox();
    for(size_t i = 1; i < size(); i++) {
      box = getUnion(box,(*this)[i]->getBoundingBox());
    }
    m_boundingBox = box;
//    m_boundingBox.scale(1.2);
  }
}

bool PolygonSet::pointOnMarkRect(const CPoint &p) {
  return m_boundingBox.pointOnMarkRect(p);
}
