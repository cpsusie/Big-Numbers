#pragma once

#include <Math/Polygon2D.h>
#include "ProfileEditor.h"

typedef enum {
  BL // bottom-left-corner
 ,BR // bottom-right-corner
 ,TR // top-right-corner
 ,TL // top-left-corner
 ,BC // bottom-center
 ,RC // right-center
 ,TC // top-center
 ,LC // left-center
 ,C  // center
 ,UNKNOWN
} MarkId;

MarkId getOppositeMarkId(MarkId id);

class RectMark : public CRect { // screen rect
public:
  MarkId    m_markId;
  CBitmap  *m_image;
  RectMark() : m_image(nullptr), m_markId(UNKNOWN) {
  }
  RectMark(MarkId markId, const CPoint &p, int imageId, int degree);
};

class TurnableRect : public Polygon2D {
private:
  CompactArray<RectMark> m_marks;
  mutable int            m_selectedMarkIndex;
  void           addMarkRect(Viewport2D &vp, MarkId id, int imageId, int degree);
  Point2D        getMarkPoint(MarkId id) const;
  FunctionR2R2  *getMoveTransformation(   const Point2D &dp) const;
  FunctionR2R2  *getStretchTransformation(const Point2D &dp) const;
  FunctionR2R2  *getRotateTransformation( const Point2D &dp) const;
  FunctionR2R2  *getSkewTransformation(   const Point2D &dp) const;
  FunctionR2R2  *getMirrorTransformation( bool horizontal  ) const;
  // Apply f to all points in a + all points in m_marks, then delete f
  void           applyFunction(FunctionR2R2 *fp, Point2DRefArray &a);

  inline Point2D getStretchOrigin() const {
    return getMarkPoint(getOppositeMarkId(getSelectedMark()));
  }
  inline Point2D getStretchDir()    const {
    return getSelectedMarkPoint() - getCenter();
  }

  Point2D        getSkewDir()       const;
  Point2D        getRotateDir()     const;
  // Return Vector parallel to bottom edge
  inline Point2D getU1()            const {
    return getBottomRight() - getBottomLeft();
  }
  // Return Vector parallel to left edge
  inline Point2D getU2()            const {
    return getTopLeft()     - getBottomLeft();
  }
  Point2DRefArray getAllMarkPoints();
public:
  TurnableRect();
  TurnableRect(const Rectangle2D &r);
  Point2D m_rotationCenter;
  inline const Point2D &getBottomLeft()   const { return (*this)[0]; }
  inline const Point2D &getBottomRight()  const { return (*this)[1]; }
  inline const Point2D &getTopRight()     const { return (*this)[2]; }
  inline const Point2D &getTopLeft()      const { return (*this)[3]; }
  inline       Point2D &getBottomLeft()         { return (*this)[0]; }
  inline       Point2D &getBottomRight()        { return (*this)[1]; }
  inline       Point2D &getTopRight()           { return (*this)[2]; }
  inline       Point2D &getTopLeft()            { return (*this)[3]; }
  inline       Point2D  getBottomCenter() const { return (getBottomLeft()  + getBottomRight()) / 2; }
  inline       Point2D  getTopCenter()    const { return (getTopLeft()     + getTopRight()   ) / 2; }
  inline       Point2D  getLeftCenter()   const { return (getBottomLeft()  + getTopLeft()    ) / 2; }
  inline       Point2D  getRightCenter()  const { return (getBottomRight() + getTopRight()   ) / 2; }
  inline       Point2D  getCenter()       const { return (getBottomLeft()  + getTopRight()   ) / 2; }
  inline       double   getWidth()        const { return getU1().length(); }
  inline       double   getHeight()       const { return getU2().length(); }

  inline       void     move(   const Point2D &dp, Point2DRefArray &pointArray) {
    applyFunction(getMoveTransformation(dp), pointArray);
  }
  inline       void     stretch(const Point2D &dp, Point2DRefArray &pointArray) {
    applyFunction(getStretchTransformation(dp), pointArray);
  }
  inline       void     mirror( bool  horizontal , Point2DRefArray &pointArray) {
    applyFunction(getMirrorTransformation(horizontal), pointArray);
  }
  void                  rotate( const Point2D &dp, Point2DRefArray &pointArray);
  void                  scale(double factor);

  inline       Point2D  getSelectedMarkPoint() const {
    return getMarkPoint(getSelectedMark());
  }
  void                  repaint(Viewport2D &vp, ProfileEditorState state);
  bool                  isPointOnMarkRect(const CPoint &p) const;
  inline MarkId         getSelectedMark() const {
    return (m_selectedMarkIndex >= 0)
         ? m_marks[m_selectedMarkIndex].m_markId
         : UNKNOWN;
  }
};
