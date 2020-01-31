#pragma once

#include <NumberInterval.h>
#include <Math/Point3D.h>
#include "Cube3D.h"
#include "PragmaLib.h"

template<typename T> class CubeTransformationTemplate {
private:
  IntervalTransformationTemplate<T> *m_transform[3];
  IntervalTransformationTemplate<T> *allocateTransformation(const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
    switch(scale) {
    case LINEAR             : return new LinearTransformationTemplate<T>(            from, to);
    case LOGARITHMIC        : return new LogarithmicTransformationTemplate<T>(       from, to);
    case NORMAL_DISTRIBUTION: return new NormalDistributionTransformationTemplate<T>(from, to);
    default                 : throwInvalidArgumentException(__TFUNCTION__, _T("scale=%d"), scale);
                              return NULL;
    }
  }
  void cleanup() {
    for(int i = 0; i < ARRAYSIZE(m_transform); i++) {
      SAFEDELETE(m_transform[i]);
    }
  }
  void computeTransformation(const Cube3D<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale, IntervalScale yScale) {
    IntervalTransformationTemplate<T> *newXtransform = NULL, *newYtransform = NULL;
    try {
      newXtransform = allocateTransformation(NumberInterval<T>(from.getBottomLeft().x,from.getBottomRight().x)
                                            ,NumberInterval<T>(to.getBottomLeft().x  ,to.getBottomRight().x)
                                            ,xScale); TRACE_NEW(newXtransform)

      newYtransform = allocateTransformation(NumberInterval<T>(from.getTopLeft().y, from.getBottomLeft().y)
                                            ,NumberInterval<T>(to.getTopLeft().y  , to.getBottomLeft().y  )
                                            ,yScale); TRACE_NEW(newYtransform)
      cleanup();
      m_transform[0] = newXtransform;
      m_ytransform = newYtransform;
    } catch(Exception e) {
      SAFEDELETE(newXtransform);
      SAFEDELETE(newYtransform);
      throw;
    }
  }

  RectangleTransformationTemplate(const IntervalTransformationTemplate<T> &tx, const IntervalTransformationTemplate<T> &ty) {
    m_transform[0] = tx.clone();
    m_ytransform = ty.clone();
  }

  static Rectangle2DTemplate<T> getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultInterval(xScale);
    NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultInterval(yScale);
    return Rectangle2DTemplate<T>(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
  }
  static Rectangle2DTemplate<T> getDefaultToRectangle() {
    return Rectangle2DTemplate<T>(0, 100, 100, -100);
  }

public:
  RectangleTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_transform[0] = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
  }
  RectangleTransformationTemplate(const RectangleTransformationTemplate &src) {
    m_transform[0] = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
  }
  RectangleTransformationTemplate &operator=(const RectangleTransformationTemplate &src) {
    cleanup();
    m_transform[0] = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
    return *this;
  }
  RectangleTransformationTemplate(const Rectangle2DTemplate<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_transform[0] = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }
  virtual ~RectangleTransformationTemplate() {
    cleanup();
  }
  const IntervalTransformationTemplate<T> &getXTransformation() const {
    return *m_transform[0];
  }
  const IntervalTransformationTemplate<T> &getYTransformation() const {
    return *m_ytransform;
  }

  void   setFromRectangle(const Rectangle2DTemplate<T> &rect) {
    computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
  }
  void   setToRectangle(const Rectangle2DTemplate<T> &rect) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
  }
  Rectangle2DTemplate<T>  getFromRectangle() const {
    Point2DTemplate<T> p1 = Point2DTemplate<T>(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
    Point2DTemplate<T> p2 = Point2DTemplate<T>(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
    return Rectangle2DTemplate<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  Rectangle2DTemplate<T>  getToRectangle()   const {
    Point2DTemplate<T> p1 = Point2DTemplate<T>(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
    Point2DTemplate<T> p2 = Point2DTemplate<T>(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
    return Rectangle2DTemplate<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  Point2DTemplate<T>      forwardTransform(const Point2DTemplate<T> &p)   const {
    return Point2DTemplate<T>(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
  }
  Point2DTemplate<T>      backwardTransform(const Point2DTemplate<T> &p)   const {
    return Point2DTemplate<T>(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y));
  }
  Point2DTemplate<T>      forwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
  }
  Point2DTemplate<T>      backwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
  }
  Rectangle2DTemplate<T>  forwardTransform(const Rectangle2DTemplate<T> &rect)  const {
    Point2DTemplate<T> p1 = forwardTransform(rect.getTopLeft());
    Point2DTemplate<T> p2 = forwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }
  Rectangle2DTemplate<T>  backwardTransform( const Rectangle2DTemplate<T> &rect)  const {
    Point2DTemplate<T> p1 = backwardTransform(rect.getTopLeft());
    Point2DTemplate<T> p2 = backwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }

  void setScale(IntervalScale newScale, int flags) {
    IntervalScale xScale = getXTransformation().getScale();
    IntervalScale yScale = getYTransformation().getScale();
    if((flags & X_AXIS) != 0) {
      xScale = newScale;
    }
    if((flags & Y_AXIS) != 0) {
      yScale = newScale;
    }
    computeTransformation(getFromRectangle(),getToRectangle(),xScale,yScale);
  }
  // Returns new fromRectangle.
  Rectangle2DTemplate<T> zoom(const Point2DTemplate<T> &p, const T &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) {
    if(flags & X_AXIS) {
      m_transform[0]->zoom(p.x, factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      m_ytransform->zoom(p.y, factor, pInToRectangle);
    }
    return getFromRectangle();
  }
  // returns true if transformation is changed
  bool adjustAspectRatio() {
    if(!getXTransformation().isLinear() || !getYTransformation().isLinear()) {
      return false;
    }
    Rectangle2DTemplate<T> fr        = getFromRectangle();
    Rectangle2DTemplate<T> tr        = getToRectangle();
    const T                fromRatio = fabs(fr.getWidth() / fr.getHeight());
    const T                toRatio   = fabs(tr.getWidth() / tr.getHeight());
    bool                   changed   = false;
    if(fromRatio > toRatio) {
      const T dh = dsign(fr.getHeight())*(fabs(fr.getWidth()/toRatio) - fabs(fr.getHeight()));
      fr.m_y -= dh / 2;
      fr.m_h += dh;
      changed = dh != 0;
    } else if(fromRatio < toRatio) {
      const T dw = dsign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth()));
      fr.m_x -= dw / 2;
      fr.m_w += dw;
      changed = dw != 0;
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }

  bool operator==(const RectangleTransformationTemplate<T> &rhs) const {
    return (getXTransformation() == rhs.getXTransformation())
        && (getYTransformation() == rhs.getYTransformation());
  }
  bool operator!=(const RectangleTransformationTemplate<T> &rhs) const {
    return !(*this == rhs);
  }
  static RectangleTransformationTemplate<T> getId() {
    return RectangleTransformationTemplate(Rectangle2DTemplate<T>(0,0,1,1)
                                          ,Rectangle2DTemplate<T>(0,0,1,1));
  }
};

typedef RectangleTransformationTemplate<float   > FloatRectangleTransformation;
typedef RectangleTransformationTemplate<double  > RectangleTransformation;
typedef RectangleTransformationTemplate<Double80> D80RectangleTransformation;
typedef RectangleTransformationTemplate<Real    > RealRectangleTransformation;
