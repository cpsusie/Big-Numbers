#pragma once

#include <MFCUtil/Viewport2D.h>
#include <CompactHashMap.h>
#include <Math/Polygon2D.h>
#include <MFCUtil/AbstractMouseHandler.h>
#include <D3DGraphics/Profile.h>

class ViewportContainer {
public:
  virtual Viewport2D &getViewport()      = 0;
  virtual Profile    &getProfile()       = 0;
  virtual void        repaintScreen()    = 0;
  virtual int         showNormals()      = 0;
  virtual bool        showPoints()       = 0;
  virtual void        setMousePosition(const Point2D &p) = 0;
  virtual HWND        getWindow()        = 0;
};

typedef enum {
  IDLE
 ,DRAGGING
 ,MOVING
 ,STRETCHING
 ,ROTATING
} ProfileDialogState;

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

void              createBitmapCache(HWND hwnd);
void              destroyBitmapCache();
LPDIRECT3DDEVICE  getBitmapCacheDevice();
CBitmap          *getBitmap(int id, int degree);
HBITMAP           rotateBitmap(LPDIRECT3DDEVICE device, HBITMAP bm, double degree);

class RectMark : public CRect { // screen rect
public:
  CBitmap          *m_image;
  MarkId            m_markId;
  RectMark() : m_image(NULL), m_markId(UNKNOWN) {
  }
  RectMark(MarkId markId, const CPoint &p, int imageId, int degree);
};

class TurnableRect : public Polygon2D {
private:
  CompactArray<RectMark> m_marks;
  int                    m_selectedMarkIndex;
  void addMarkRect(Viewport2D &vp, MarkId id, int imageId, int degree);
  Point2D getMarkPoint(MarkId id);
  Point2DFunction *getMoveTransformation(   const Point2D &dp);
  Point2DFunction *getStretchTransformation(const Point2D &dp);
  Point2DFunction *getRotateTransformation( const Point2D &dp);
  Point2DFunction *getSkewTransformation(   const Point2D &dp);
  Point2DFunction *getMirrorTransformation( bool horizontal );

  Point2D getStretchOrigin();
  Point2D getStretchDir();
  Point2D getSkewDir();
  Point2D getRotateDir();
  Point2D getU1() { return getTopRight()   - getTopLeft(); }
  Point2D getU2() { return getBottomLeft() - getTopLeft(); }
  void applyFunction(Point2DFunction *f, Point2DRefArray &pointArray);
public:
  Point2D m_rotationCenter;
  Point2D &getTopLeft()     { return (*this)[0]; }
  Point2D &getTopRight()    { return (*this)[1]; }
  Point2D &getBottomRight() { return (*this)[2]; }
  Point2D &getBottomLeft()  { return (*this)[3]; }
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
  void repaint(Viewport2D &vp, ProfileDialogState state);
  bool pointOnMarkRect(const CPoint &p);
  MarkId getSelectedMark() const { return m_selectedMarkIndex >= 0 ? m_marks[m_selectedMarkIndex].m_markId : UNKNOWN; }
};

class PolygonSet : public CompactArray<ProfilePolygon*> {
private:
  TurnableRect m_boundingBox; // world rect
  Point2DRefArray getPointRefArray();
public:
  void evaluateBox();
  void repaint(Viewport2D &vp, ProfileDialogState state);
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

class DrawTool : public AbstractMouseHandler {
private:
  CBrush m_blackBrush, m_redBrush;
protected:
  ViewportContainer     &m_container;
  PolygonSet             m_polygonSet;
  CompactArray<Point2D*> m_selectedPoints;
  ProfileDialogState     m_state;

  void drawState();
  void paintBox(const Point2D &p, bool selected);
  void paintPoints(const ProfilePolygon &p, bool selected);
  void repaintPolygon(const ProfilePolygon &p, bool selected);
  void repaintScreen() { m_container.repaintScreen(); }
  void selectPolygonsInRect(const Rectangle2D &r);
  void selectPointsInRect(const Rectangle2D &r);
  void select(ProfilePolygon *p);
  void select(Point2D *p);
  void unselect(ProfilePolygon *p);
  void unselect(Point2D *p);
  bool isSelected(ProfilePolygon *p) const;
  bool isSelected(const Point2D *p) const;
public:
  DrawTool(ViewportContainer *container);
  virtual ~DrawTool();
  BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  bool canConnect() const;
  void connect();
  bool canInvertNormals() const;
  void invertNormals();
  bool canMirror() const;
  void mirror(bool horizontal);
  bool canDelete();
  void deleteSelected();
  bool canCopy();
  void copySelected();
  bool canCut();
  void cutSelected();
  void paste();
  void initState() { m_state = IDLE; }
  void unselectAll();
  void repaintProfile();
};

class NullTool : public DrawTool {
public:
  NullTool() : DrawTool(NULL) {}
};

class LineTool : public DrawTool {
private:
  Point2D *m_p0,*m_p1;
public:
  LineTool(ViewportContainer *container) : DrawTool(container) { m_p0 = m_p1 = NULL; }
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
};

class BezierCurveTool : public DrawTool {
public:
  BezierCurveTool(ViewportContainer *container) : DrawTool(container) {}
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
};

class RectangleTool : public DrawTool {
private:
  Point2D *m_ul,*m_ur,*m_ll,*m_lr;
public:
  RectangleTool(ViewportContainer *container) : DrawTool(container) {}
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
};

class PolygonTool : public DrawTool {
private:
  ProfilePolygon *m_pp;
public:
  PolygonTool(ViewportContainer *container) : DrawTool(container) { m_pp = NULL; }
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnLButtonDblClk(UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
};

class EllipseTool : public DrawTool {
private:
  CPoint m_p0,m_p1;
public:
  EllipseTool(ViewportContainer *container) : DrawTool(container) {}
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
};

class SelectTool : public DrawTool {
private:
  void startDragRect();
  void redrawDragRect(const CPoint &point);
  void endDragRect();
  ProfilePolygon *findNearestPolygon(const CPoint  &p);
  Point2D        *findNearestPoint(  const CPoint  &p);
  void            moveSelectedPoints(const Point2D &dp);
  CPoint               m_lastMousePoint;
  CPoint               m_mouseDownPoint;
  CRect                m_dragRect;
  void adjustMousePosition();
public:
  SelectTool(ViewportContainer *container);
  void OnLButtonDown(  UINT nFlags, CPoint point);
  void OnLButtonDblClk(UINT nFlags, CPoint point);
  void OnLButtonUp(    UINT nFlags, CPoint point);
  void OnRButtonDown(  UINT nFlags, CPoint point);
  void OnRButtonUp(    UINT nFlags, CPoint point);
  void OnMouseMove(    UINT nFlags, CPoint point);
};
