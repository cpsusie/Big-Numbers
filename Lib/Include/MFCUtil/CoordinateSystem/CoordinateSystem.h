#pragma once

#include <BitSet.h>
#include <Math/MathLib.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/Viewport2D.h>
#include "AxisType.h"
#include "DataRange.h"

class CCoordinateSystem;

class CoordinateSystemObject {
public:
  virtual void paint(CCoordinateSystem &cs) = 0;
  virtual const DataRange &getDataRange() const = 0;
  virtual ~CoordinateSystemObject() {
  }
};

class OccupationMap : public BitMatrix {
private:
  CSize m_cellSize; // in pixels
  CSize m_winSize;  // in pixels
  inline CRect getRect() const {
    return CRect(0,0,m_winSize.cx,m_winSize.cy);
  }
  PointArray &createDistinctPointArray(PointArray &dst, const PointArray &src) const;
public:
  OccupationMap() : m_cellSize(10,10), m_winSize(80,80), BitMatrix(8,8) {
  }
  void setCellSize(int cx, int cy);
  void setWindowSize(const CSize &size);
  void setOccupiedPoint(          const CPoint     &p );
  void setOccupiedLine(           const CPoint     &p1, const CPoint &p2);
  void setOccupiedRect(           const CRect      &r );
  void setOccupiedPoints(         const PointArray &pa);
  void setOccupiedConnectedPoints(const PointArray &pa);
};


class CCoordinateSystem : public CStatic {
private:
  DECLARECLASSNAME;
  Viewport2D                            m_vp;
  OccupationMap                         m_occupationMap;
  COLORREF                              m_backgroundColor;
  COLORREF                              m_axisColor;
  AxisType                              m_xAxisType, m_yAxisType;
  bool                                  m_grid;
  bool                                  m_retainAspectRatio;
  bool                                  m_autoScale, m_autoSpace;
  friend class SystemPainter;

  CompactArray<CoordinateSystemObject*> m_objectArray;
  DataRange   findSmallestDataRange() const;
  int         findObject(const CoordinateSystemObject *object) const;
  PointArray &transformPoint2DArray(PointArray &dst, const Point2DArray &src) const;
public:

public:
  CCoordinateSystem();
  virtual ~CCoordinateSystem();

  void   substituteControl(CWnd *parent, int id);
  void   paint(            CDC &dc);

  static DoubleInterval getDefaultInterval(AxisType type);
  static DataRange      getDefaultDataRange(AxisType xType, AxisType yType);

  void   addPointObject(   const Point2DArray &a, COLORREF color = RGB(0,0,0));
  void   addFunctionObject(Function &f, const DoubleInterval *range = NULL, COLORREF color = RGB(0,0,0));

  void   addObject(   CoordinateSystemObject *object);
  void   removeObject(CoordinateSystemObject *object);
  // will not delete objects
  void   removeAllObjects() {
    m_objectArray.clear();
  }
  // remove AND delete all objects
  void deleteAllObjects();
  int    getObjectCount() const {
    return (int)m_objectArray.size();
  }
  CoordinateSystemObject &getObject(UINT index) {
    return *m_objectArray[index];
  }

  inline const RectangleTransformation &getTransformation() const {
    return m_vp.getTransformation();;
  }

  inline RectangleTransformation &getTransformation() {
    return m_vp.getTransformation();
  }

  void setOccupiedPoint(          const Point2D      &p );
  void setOccupiedLine(           const Point2D      &p1, const Point2D &p2);
  void setOccupiedPoints(         const Point2DArray &pa);
  void setOccupiedConnectedPoints(const Point2DArray &pa);

  void setDC(CDC &dc);
  Viewport2D &getViewport() {
    return m_vp;
  }
  OccupationMap &getOccupationMap() {
    return m_occupationMap;
  }

  // flags any combination of X_AXIS and Y_AXIS
  void  setFromRectangle(const Rectangle2D &rectangle, int makeSpaceFlags);

  inline Rectangle2D getFromRectangle() const {
    return getTransformation().getFromRectangle();
  }

  void setDataRange(const DataRange &dataRange, bool makeSpace);

  inline DataRange getDataRange() const {
    return getTransformation().getFromRectangle();
  }

  // flags any combination of X_AXIS and Y_AXIS
  void zoom(const CPoint &p, bool in, int flags) {
    getTransformation().zoom(Point2DP(p), in, flags);
  }

  void setGrid(bool grid) {
    m_grid = grid;
  }

  bool hasGrid() const {
    return m_grid;
  }

  void setAutoScale(bool autoScale, bool makeSpace) {
    m_autoScale = autoScale;
    m_autoSpace = makeSpace;
  }

  bool getAutoScale() const {
    return m_autoScale;
  }
  bool canRetainAspectRatio() const {
    return m_xAxisType == m_yAxisType;
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  bool isRetainingAspectRatio() const {
    return canRetainAspectRatio() && m_retainAspectRatio;
  }

  void setBackGroundColor(COLORREF color) {
    m_backgroundColor = color;
  }

  void setAxisColor(COLORREF color) {
    m_axisColor = color;
  }

  void setXAxisType(AxisType axisType) {
    m_xAxisType = axisType;
  }

  AxisType getXAxisType() const {
    return m_xAxisType;
  }

  void setYAxisType(AxisType axisType) {
    m_yAxisType = axisType;
  }

  AxisType getYAxisType() const {
    return m_yAxisType;
  }

  Point2D getMouseToSystem(const CPoint &p) {
    return getTransformation().backwardTransform(p.x, p.y);
  }

	afx_msg void OnPaint();

  DECLARE_MESSAGE_MAP()
};
