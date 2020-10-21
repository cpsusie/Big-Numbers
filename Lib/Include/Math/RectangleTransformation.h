#pragma once

#include "IntervalTransformationArray.h"
#include "Rectangle2D.h"

template<typename T> class RectangleTransformationTemplate {
private:
  IntervalTransformationArray<T, 2> m_tr;
  void computeTransformation(const Rectangle2DTemplate<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale, IntervalScale yScale) {
    IntervalTransformationArray<T, 2> newTr;
    newTr.setTransformation(0, from.getXInterval(), to.getXInterval(), xScale);
    newTr.setTransformation(1, from.getYInterval(), to.getYInterval(), yScale);
    m_tr = newTr;
  }

  RectangleTransformationTemplate(const IntervalTransformationTemplate<T> &tx, const IntervalTransformationTemplate<T> &ty) {
    m_tr.setTransformation(0,tx);
    m_tr.setTransformation(1,ty);
  }

  static Rectangle2DTemplate<T> getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    const NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(xScale);
    const NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(yScale);
    return Rectangle2DTemplate<T>(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
  }
  static Rectangle2DTemplate<T> getDefaultToRectangle() {
    return Rectangle2DTemplate<T>(0, 100, 100, -100);
  }

public:
  inline RectangleTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
  }
  inline RectangleTransformationTemplate(const RectangleTransformationTemplate &src) : m_tr(src.m_tr) {
  }
  RectangleTransformationTemplate(const Rectangle2DTemplate<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    computeTransformation(from, to, xScale, yScale);
  }
  const IntervalTransformationTemplate<T> &getXTransformation() const {
    return m_tr[0];
  }
  const IntervalTransformationTemplate<T> &getYTransformation() const {
    return m_tr[1];
  }

  void   setFromRectangle(const Rectangle2DTemplate<T> &rect) {
    m_tr.setFromCube((CubeN<T>)rect);
  }
  void   setToRectangle(const Rectangle2DTemplate<T> &rect) {
    m_tr.setToCube((CubeN<T>)rect);
  }
  Rectangle2DTemplate<T>    getFromRectangle() const {
    return Rectangle2DTemplate<T>(m_tr.getFromCube());
  }
  Rectangle2DTemplate<T>    getToRectangle()   const {
    return Rectangle2DTemplate<T>(m_tr.getToCube());
  }
  inline Point2DTemplate<T> forwardTransform(const Point2DTemplate<T> &p)   const {
    return forwardTransform(p.x, p.y);
  }
  inline Point2DTemplate<T> backwardTransform(const Point2DTemplate<T> &p)   const {
    return backwardTransform(p.x, p.y);
  }
  inline Point2DTemplate<T> forwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(m_tr[0].forwardTransform(x),m_tr[1].forwardTransform(y));
  }
  inline Point2DTemplate<T>  backwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(m_tr[0].backwardTransform(x),m_tr[1].backwardTransform(y));
  }
  inline Rectangle2DTemplate<T>  forwardTransform(const Rectangle2DTemplate<T> &rect)  const {
    const Point2DTemplate<T> tl = forwardTransform(rect.getTopLeft()    );
    const Point2DTemplate<T> br = forwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(tl,br);
  }
  Rectangle2DTemplate<T>  backwardTransform( const Rectangle2DTemplate<T> &rect)  const {
    const Point2DTemplate<T> tl = backwardTransform(rect.getTopLeft());
    const Point2DTemplate<T> br = backwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(tl,br);
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
      m_tr[0].zoom(p.x, factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      m_tr[1].zoom(p.y, factor, pInToRectangle);
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
      fr.m_p.y     -= dh / 2;
      fr.m_size.cy += dh;
      changed = dh != 0;
    } else if(fromRatio < toRatio) {
      const T dw = dsign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth()));
      fr.m_p.x     -= dw / 2;
      fr.m_size.cx += dw;
      changed = dw != 0;
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }

  inline bool operator==(const RectangleTransformationTemplate<T> &rhs) const {
    return m_tr == rhs.m_tr;
  }
  inline bool operator!=(const RectangleTransformationTemplate<T> &rhs) const {
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
