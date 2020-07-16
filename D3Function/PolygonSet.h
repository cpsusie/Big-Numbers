#pragma once

#include <D3DGraphics/Profile2D.h>
#include "TurnableRect.h"

class PolygonSet : public CompactArray<ProfilePolygon2D*> {
private:
  TurnableRect m_boundingBox; // world rect
  Point2DRefArray getPointRefArray();
public:
  void evaluateBox();
  void repaint(Viewport2D &vp, ProfileEditorState state);
  bool pointOnMarkRect(const CPoint &p); // screen point
  bool pointInRect(const Point2D &p) const { return m_boundingBox.contains(p) >= 0; }      // world point
  void invertNormals();
  void mirror(bool horizontal);
  void move(   const Point2D &dp);
  void stretch(const Point2D &dp);
  void rotate( const Point2D &dp);
  Point2D getSelectedMarkPoint() { return m_boundingBox.getSelectedMarkPoint(); }
  String toString() const;
};
