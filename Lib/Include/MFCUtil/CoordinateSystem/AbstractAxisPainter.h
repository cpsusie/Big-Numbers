#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/Coordinatesystem/AxisType.h>

class SystemPainter;

class AbstractAxisPainter {
private:
  SystemPainter                &m_systemPainter;
  const AxisIndex               m_axisIndex;     // is this X- or Y-axis
  const AxisType                m_type;
  bool                          m_isPainting;    // if false, all paint operations are invisible. Used to find maxTextOffset in constructor
  DoubleInterval                m_dataRange;     // X- or Y-interval in fromRectangle
  Viewport2D                   *m_vp;            // Not allocated locally
  mutable String                m_doubleFormatPaint, m_doubleFormatMouse;
  CPen                          m_solidPen, m_gridPen;
  DoubleInterval                m_xRectInterval; // X-interval in toRectangle
  DoubleInterval                m_yRectInterval; // Y-interval in toRectangle
  double                        m_step;
  double                        m_min;
  double                        m_max;
  int                           m_maxTextOffset;
  int                           m_lastTextPos;

  AbstractAxisPainter(           const AbstractAxisPainter &src); // not implemented
  AbstractAxisPainter &operator=(const AbstractAxisPainter &src); // not implemented

  void verticalGridLine(  double x);
  void horizontalGridLine(double y);
  void line(double x1, double y1, double x2, double y2, bool setOccupied);
  void line(double x1, double y1, double x2, double y2, CPen &pen, bool setOccupied);
  void textOut(int x, int y, const String &txt, const CSize &textExtent);
  void paintXAxis();
  void paintYAxis();

protected:
  virtual void         init();
  virtual const TCHAR *getDoubleFormat() const;
  virtual double       next(double x)    const;
  inline AxisType      getType()         const {
    return m_type;
  }
  inline  AxisIndex    getAxisIndex() const {
    return m_axisIndex;
  }
  const  AxisAttribute &getAxisAttr() const;
  inline AxisFlags      getAxisFlags() const {
    return getAxisAttr().getFlags();
  }
  inline bool hasGridLines() const {
    return (getAxisFlags() & AXIS_SHOW_GRIDLINES) != 0;
  }
  const  DoubleInterval &getDataRange() const {
    return m_dataRange;
  }

  double getVisibleDataRangeLength() const {
    return m_max - m_min;
  }
  double getMin()  const {
    return m_min;
  }
  double getMax()  const {
    return m_max;
  }
  double getStep() const {
    return m_step;
  }

  bool isMouseMode() const;
  const  IntervalTransformation &getTransformation() const;
  virtual void paintXData() = 0;
  virtual void paintYData() = 0;
  void   paintXDataSingleDecade();
  void   paintYDataSingleDecade();
  void   doInvisiblePaint();
  void   setMinMaxStep(double min, double max, double step);
  double transform(    double x) const;
  void   xTextOut(     double x, const String &s, int yOffset);
  void   yTextOut(     double y, const String &s, int xOffset);
  bool   xTextPossible(double x, const String &s);
  bool   yTextPossible(double y, const String &s);

  bool   isPainting() const {
    return m_isPainting;
  }

  void   paintHorizontalPin(double y, bool big);
  void   paintVerticalPin(  double x, bool big);
  const  CPoint &getOrigin() const;
  CSize  getTextExtent(const String &str);

public:
  static const int PIN_LENGTH;
  static const int ARROW_SIZE;

  AbstractAxisPainter(SystemPainter &systemPainter, AxisIndex axisIndex, AxisType type);
  virtual ~AbstractAxisPainter();
  void paintAxisData();
  void paintAxis();
  virtual double getAxisPoint() const = 0;
  virtual String getValueText(double v) const;

  inline String getMaxValueText() const {
    return getValueText(m_max);
  };

  inline String getMinValueText() const {
    return getValueText(m_min);
  }

  int getMaxTextOffset() const {
    return m_maxTextOffset;
  }

  static double  findNiceDecimalStep(double range);
};
