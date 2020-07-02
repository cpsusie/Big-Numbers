#pragma once

#include <BitMatrix.h>
#include <Math/MathLib.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/Viewport2D.h>
#include "AxisType.h"
#include "DataRange.h"

class CCoordinateSystem;

class CoordinateSystemObject {
private:
  CCoordinateSystem &m_system;
protected:
  const Viewport2D &getViewport() const;
public:
  CoordinateSystemObject(CCoordinateSystem &system) : m_system(system) {
  }
  virtual ~CoordinateSystemObject() {
  }
  inline const CCoordinateSystem &getSystem() const {
    return m_system;
  }
  inline CCoordinateSystem &getSystem() {
    return m_system;
  }
  virtual void paint(CDC &dc) = 0;
  virtual const DataRange &getDataRange() const = 0;
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
  friend class SystemPainter;

  Viewport2D                            m_vp;
  OccupationMap                         m_occupationMap;
  COLORREF                              m_backgroundColor;
  AxisAttribute                         m_axisAttr[2];
  bool                                  m_retainAspectRatio;
  bool                                  m_autoScale, m_autoSpace;
  SystemPainter                        *m_systemPainter;

  CompactArray<CoordinateSystemObject*> m_objectArray;
  DataRange   findSmallestDataRange() const;
  int         findObject(const CoordinateSystemObject *object) const;
  PointArray &transformPoint2DArray(PointArray &dst, const Point2DArray &src) const;
  void        createSystemPainter();
  void        destroySystemPainter();

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
  inline AxisAttribute &getAxisAttr(AxisIndex axis) {
    return m_axisAttr[axis];
  }
  // remove AND delete all objects
  void deleteAllObjects();
  inline int getObjectCount() const {
    return (int)m_objectArray.size();
  }
  inline CoordinateSystemObject &getObject(UINT index) {
    return *m_objectArray[index];
  }

  inline const RectangleTransformation &getTransformation() const {
    return m_vp.getTransformation();;
  }
  inline RectangleTransformation &getTransformation() {
    return m_vp.getTransformation();;
  }
  void setOccupiedPoint(          const Point2D      &p );
  void setOccupiedLine(           const Point2D      &p1, const Point2D &p2);
  void setOccupiedPoints(         const Point2DArray &pa);
  void setOccupiedConnectedPoints(const Point2DArray &pa);

  inline CDC *setDC(CDC *dc) const {
    return m_vp.setDC(dc);
  }
  inline Viewport2D &getViewport() {
    return m_vp;
  }
  inline const Viewport2D &getViewport() const {
    return m_vp;
  }
  inline OccupationMap &getOccupationMap() {
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
    getTransformation().zoom(p, in, flags);
  }

  void setAutoScale(bool autoScale, bool makeSpace) {
    m_autoScale = autoScale;
    m_autoSpace = makeSpace;
  }
  inline bool getAutoScale() const {
    return m_autoScale;
  }

  inline bool canRetainAspectRatio() const {
    return getAxisType(XAXIS_INDEX) == getAxisType(YAXIS_INDEX);
  }

  void setRetainAspectRatio(bool retainAspectRatio);

  inline bool isRetainingAspectRatio() const {
    return canRetainAspectRatio() && m_retainAspectRatio;
  }

  inline void setBackGroundColor(COLORREF color) {
    m_backgroundColor = color;
  }
  inline COLORREF getBackGroundColor() const {
    return m_backgroundColor;
  }
  inline const AxisAttribute &getAxisAttr(AxisIndex axis) const {
    return m_axisAttr[axis];
  }
  inline void setAxisType(AxisIndex axis, AxisType type) {
    getAxisAttr(axis).setType(type);
  }
  inline AxisType getAxisType(AxisIndex axis) const {
    return getAxisAttr(axis).getType();
  }
  inline void setAxisColor(AxisIndex axis, COLORREF color) {
    getAxisAttr(axis).setColor(color);
  }
  inline COLORREF getAxisColor(AxisIndex axis) const {
    return getAxisAttr(axis).getColor();
  }
  inline void setAxisFlags(AxisIndex axis, AxisFlags flags, bool set) {
    getAxisAttr(axis).setFlags(set?flags:0, set?0:flags);
  }
  inline bool getAxisFlags(AxisIndex axis, AxisFlags flags) const {
    return (getAxisAttr(axis).getFlags() & flags) != 0;
  }

  inline void setShowAxisValues(AxisIndex axis, bool show) {
    setAxisFlags(axis, AXIS_SHOW_VALUES, show);
  }
  inline bool hasAxisValues(AxisIndex axis) const {
    return getAxisFlags(axis, AXIS_SHOW_VALUES);
  }

  inline void showAxisValueMarks(AxisIndex axis, bool show) {
    setAxisFlags(axis, AXIS_SHOW_VALUEMARKS, show);
  }
  inline bool hasAxisValueMarks(AxisIndex axis) const {
    return getAxisFlags(axis, AXIS_SHOW_VALUEMARKS);
  }
  inline void showAxisGridLines(AxisIndex axis, bool show) {
    setAxisFlags(axis, AXIS_SHOW_GRIDLINES, show);
  }
  inline bool hasAxisGridLines(AxisIndex axis) const {
    return getAxisFlags(axis, AXIS_SHOW_GRIDLINES);
  }
  Point2D getMouseToSystem(const CPoint &p) {
    return getTransformation().backwardTransform(p.x, p.y);
  }

  String getPointText(const Point2D &p) const;
  String getValueText(AxisIndex axis, double value) const;

  afx_msg void OnPaint();

  DECLARE_MESSAGE_MAP()
};
