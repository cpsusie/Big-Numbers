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
  m_boundingBox.rotate(dp, getPointRefArray());
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
    bool        firstTime = true;
    Rectangle2D box;
    for(auto it = getIterator(); it.hasNext();) {
      if(firstTime) {
        box = it.next()->getBoundingBox();
        firstTime     = false;
      } else {
        box = getUnion(box, it.next()->getBoundingBox());
      }
    }
    m_boundingBox = box;
  }
}

bool PolygonSet::isPointOnMarkRect(const CPoint &p) {
  return m_boundingBox.isPointOnMarkRect(p);
}
