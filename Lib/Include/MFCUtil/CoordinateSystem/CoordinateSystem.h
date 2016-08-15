#pragma once

#include <Math/MathLib.h>
#include <MFCUtil/Viewport2D.h>
#include "AxisType.h"
#include "DataRange.h"

class CCoordinateSystem;

class CoordinateSystemObject {
public:
  virtual void paint(Viewport2D &vp) = 0;
  virtual ~CoordinateSystemObject() {
  }
};

class CCoordinateSystem : public CStatic {
private:
  DECLARECLASSNAME;
  Viewport2D                            m_vp;
  COLORREF                              m_backgroundColor;
  COLORREF                              m_axisColor;
  AxisType                              m_xAxisType, m_yAxisType;
  bool                                  m_grid;
  bool                                  m_retainAspectRatio;
  CompactArray<CoordinateSystemObject*> m_objectArray;
  int    findObject(const CoordinateSystemObject *object) const;
public:

public:
  CCoordinateSystem();
  virtual ~CCoordinateSystem();

  void   substituteControl(CWnd *parent, int id);

  static DoubleInterval getDefaultInterval(AxisType type);
  static DataRange getDefaultDataRange(AxisType xType, AxisType yType);

  void   paint(       CDC &dc);
  void   paintCurve(  CDC &dc, const Point2DArray &a, COLORREF color = RGB(0,0,0));
  void   plotfunction(CDC &dc, Function &f, const DoubleInterval *range = NULL, COLORREF color = RGB(0,0,0));

  void   addObject(   CoordinateSystemObject *object);
  void   removeObject(CoordinateSystemObject *object);
  void   removeAllObjects() {
    m_objectArray.clear();
  }
  int    getObjectCount() const {
    return (int)m_objectArray.size();
  }
  CoordinateSystemObject *getObject(unsigned int index) {
    return m_objectArray[index];
  }

  inline const RectangleTransformation &getTransformation() const {
    return m_vp.getTransformation();;
  }
  
  inline RectangleTransformation &getTransformation() {
    return m_vp.getTransformation();;
  }

  Viewport2D *getViewport(CDC &dc);
  void  setFromRectangle(const Rectangle2D &rectangle, bool makeSpace);

  inline Rectangle2D getFromRectangle() const {
    return getTransformation().getFromRectangle();
  }

  void setDataRange(const DataRange &dataRange, bool makeSpace);

  inline DataRange getDataRange() const {
    return getTransformation().getFromRectangle();
  }
  
  void zoom(const CPoint &p, bool in, int flags) {
    getTransformation().zoom(Point2DP(p), in, flags);
  }

  void setGrid(bool grid) {
    m_grid = grid;
  }

  bool hasGrid() const {
    return m_grid;
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

