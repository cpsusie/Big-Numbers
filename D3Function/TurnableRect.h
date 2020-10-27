#pragma once

#include <Math/Polygon2D.h>
#include "ProfileEditor.h"

typedef enum {
  TL // top-left-corner
 ,TR // top-right-corner
 ,BL // bottom-left-corner
 ,BR // bottom-right-corner
 ,TC // top-center
 ,BC // bottom-center
 ,LC // left-center
 ,RC // right-center
 ,C  // center
 ,UNKNOWN
} MarkId;

class RectMark : public CRect { // screen rect
public:
  CBitmap          *m_image;
  MarkId            m_markId;
  RectMark() : m_image(nullptr), m_markId(UNKNOWN) {
  }
  RectMark(MarkId markId, const CPoint &p, int imageId, int degree);
};

class TurnableRect : public Polygon2D {
private:
  CompactArray<RectMark> m_marks;
  int                    m_selectedMarkIndex;
  void addMarkRect(Viewport2D &vp, MarkId id, int imageId, int degree);
  Point2D getMarkPoint(MarkId id);
  FunctionR2R2 *getMoveTransformation(   const Point2D &dp);
  FunctionR2R2 *getStretchTransformation(const Point2D &dp);
  FunctionR2R2 *getRotateTransformation( const Point2D &dp);
  FunctionR2R2 *getSkewTransformation(   const Point2D &dp);
  FunctionR2R2 *getMirrorTransformation( bool horizontal );

  Point2D getStretchOrigin();
  Point2D getStretchDir();
  Point2D getSkewDir();
  Point2D getRotateDir();
  Point2D getU1() { return getTopRight()   - getTopLeft(); }
  Point2D getU2() { return getBottomLeft() - getTopLeft(); }
  void applyFunction(FunctionR2R2 *f, Point2DRefArray &pointArray);
public:
  Point2D m_rotationCenter;
  Point2D &getTopLeft()     { return (Point2D&)((*this)[0]); }
  Point2D &getTopRight()    { return (Point2D&)((*this)[1]); }
  Point2D &getBottomRight() { return (Point2D&)((*this)[2]); }
  Point2D &getBottomLeft()  { return (Point2D&)((*this)[3]); }
  Point2D getCenter();
  Point2D getTopCenter();
  Point2D getBottomCenter();
  Point2D getLeftCenter();
  Point2D getRightCenter();
  TurnableRect(const Rectangle2D &r);
  TurnableRect();
  double width()  { return getU1().length(); }
  double height() { return getU2().length(); }
  void move(   const Point2D &dp, Point2DRefArray &pointArray);
  void stretch(const Point2D &dp, Point2DRefArray &pointArray);
  void rotate( const Point2D &dp, Point2DRefArray &pointArray);
  void mirror( bool  horizontal , Point2DRefArray &pointArray);
  Point2D getSelectedMarkPoint() ;
  void scale(double factor);
  void repaint(Viewport2D &vp, ProfileEditorState state);
  bool pointOnMarkRect(const CPoint &p);
  MarkId getSelectedMark() const { return m_selectedMarkIndex >= 0 ? m_marks[m_selectedMarkIndex].m_markId : UNKNOWN; }
};
