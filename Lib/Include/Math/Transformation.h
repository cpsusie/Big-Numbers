#pragma once

#include <NumberInterval.h>
#include "Point2D.h"
#include "Rectangle2D.h"
#include "PragmaLib.h"

typedef enum {
  LINEAR
 ,LOGARITHMIC
 ,NORMAL_DISTRIBUTION
} IntervalScale;

class IntervalTransformation {
private:
  DoubleInterval m_fromInterval;
  DoubleInterval m_toInterval;
  double m_a, m_b;

protected:
  virtual double translate(double x)        const = 0;
  virtual double inverseTranslate(double x) const = 0;
  void computeTransformation();
  void checkFromInterval(const DoubleInterval &interval);
public:
  IntervalTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval);
  static DoubleInterval getDefaultInterval(IntervalScale);

          const DoubleInterval &getFromInterval() const { return m_fromInterval; }
          const DoubleInterval &getToInterval()   const { return m_toInterval;   }
  virtual const DoubleInterval &setFromInterval(  const DoubleInterval &interval);
  virtual const DoubleInterval &setToInterval(    const DoubleInterval &interval);

  double         forwardTransform( double x) const;
  double         backwardTransform(double x) const;
  DoubleInterval forwardTransform( const DoubleInterval &interval) const;
  DoubleInterval backwardTransform(const DoubleInterval &interval) const;

  virtual bool isLinear() const = 0;
  const DoubleInterval &zoom(double x, double factor, bool xInToInterval=true); // Returns new fromInterval.

  virtual IntervalScale getScale() const = 0;
  virtual IntervalTransformation *clone() const = 0;
  virtual ~IntervalTransformation() {
  }
};

class LinearTransformation : public IntervalTransformation {
protected:
  double translate(       double x) const;
  double inverseTranslate(double x) const;
public:
  bool isLinear() const {
    return true;
  }
  LinearTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval);
  IntervalScale getScale() const;
  IntervalTransformation *clone() const;
};

class LogarithmicTransformation : public IntervalTransformation {
protected:
  double translate(       double x) const;
  double inverseTranslate(double x) const;
public:
  bool isLinear() const {
    return false;
  }
  LogarithmicTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval);
  IntervalScale getScale() const;
  IntervalTransformation *clone() const;
};

class NormalDistributionTransformation : public IntervalTransformation {
private:
  static DoubleInterval maxInterval;

protected:
  double translate(       double x) const;
  double inverseTranslate(double x) const;
  const DoubleInterval &setFromInterval(const DoubleInterval &i);
public:
  NormalDistributionTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval);
  bool isLinear() const {
    return false;
  }
  IntervalScale getScale() const;
  IntervalTransformation *clone() const;
};

#define X_AXIS 1
#define Y_AXIS 2

class RectangleTransformation {
private:
  IntervalTransformation *m_xtransform;
  IntervalTransformation *m_ytransform;
  void computeTransformation(const Rectangle2D &from, const Rectangle2D &to, IntervalScale xScale, IntervalScale yScale);
  IntervalTransformation *allocateTransformation(const DoubleInterval &from, const DoubleInterval &to, IntervalScale scale);
  RectangleTransformation(const IntervalTransformation &tx, const IntervalTransformation &ty);

  static Rectangle2D getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale);
  static Rectangle2D getDefaultToRectangle();

public:
  RectangleTransformation(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR);
  RectangleTransformation(const RectangleTransformation &src);
  RectangleTransformation &operator=(const RectangleTransformation &src);
  RectangleTransformation(const Rectangle2D &from, const Rectangle2D &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR);
  ~RectangleTransformation();
  const IntervalTransformation &getXTransformation() const { return *m_xtransform; }
  const IntervalTransformation &getYTransformation() const { return *m_ytransform; }

  void   setFromRectangle(  const Rectangle2D &rect);
  void   setToRectangle(    const Rectangle2D &rect);
  Rectangle2D  getFromRectangle() const;
  Rectangle2D  getToRectangle()   const;
  Point2D      forwardTransform(  const Point2D &p)   const;
  Point2D      backwardTransform( const Point2D &p)   const;
  Point2D      forwardTransform(  double x, double y) const;
  Point2D      backwardTransform( double x, double y) const;
  Rectangle2D  forwardTransform(  const Rectangle2D &rect)  const;
  Rectangle2D  backwardTransform( const Rectangle2D &rect)  const;

  void setScale(IntervalScale newScale, int flags);
  Rectangle2D zoom(const Point2D &p, double factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true); // Returns new fromRectangle.
  bool adjustAspectRatio(); // returns true if transformation is changed

  static const RectangleTransformation id;
};
