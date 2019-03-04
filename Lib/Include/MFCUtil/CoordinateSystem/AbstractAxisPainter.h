#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/Coordinatesystem/AxisType.h>

class SystemPainter;

class AbstractAxisPainter {
private:
  SystemPainter                &m_systemPainter;
  bool                          m_isPainting;    // if false, all paint operations are invisible. Used to find maxTextOffset in constructor
  bool                          m_xAxis;         // is this X- or Y-axis
  DoubleInterval                m_dataRange;     // X- or Y-interval in fromRectangle
  Viewport2D                   *m_vp;            // Not allocated locally
  COLORREF                      m_axisColor;
  bool                          m_grid;
  String                        m_doubleFormat;
  CPen                          m_solidPen, m_gridPen;
  DoubleInterval                m_xRectInterval; // X-interval in toRectangle
  DoubleInterval                m_yRectInterval; // Y-interval in toRectangle
  double                        m_step;
  double                        m_min;
  double                        m_max;
  int                           m_maxTextOffset;
  int                           m_lastTextPos;

  void verticalGridLine(  double x);
  void horizontalGridLine(double y);
  void line(double x1, double y1, double x2, double y2, bool setOccupied);
  void line(double x1, double y1, double x2, double y2, CPen &pen, bool setOccupied);
  void textOut(int x, int y, const String &txt, const CSize &textExtent);
  void paintXAxis();
  void paintYAxis();
protected:

  virtual void init();
  virtual const TCHAR *getDoubleFormat();
  virtual double next(double x) const;
  virtual AxisType getType() const = 0;
  bool   isXAxis() const;
  const  DoubleInterval &getDataRange() const {
    return m_dataRange;
  }

  double getVisibleDataRangeLength() const {
    return m_max - m_min;
  }
;
  double getMin()  const {
    return m_min;
  }

  double getMax()  const {
    return m_max;
  }

  double getStep() const {
    return m_step;
  }

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

  AbstractAxisPainter(SystemPainter &systemPainter, bool xAxis);
  virtual ~AbstractAxisPainter();
  void paintAxisData();
  void paintAxis();
  virtual double getAxisPoint() const = 0;
  virtual String getValueText(double v);

  inline String getMaxValueText() {
    return getValueText(m_max);
  };

  inline String getMinValueText() {
    return getValueText(m_min);
  }

  int getMaxTextOffset() const {
    return m_maxTextOffset;
  }

  static double  findNiceDecimalStep(double range);
};
