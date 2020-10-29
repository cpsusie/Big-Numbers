#pragma once

#include "CubeTransformationTemplate.h"
#include "Rectangle2D.h"

template<typename T> class Rectangle2DTransformationTemplate : public CubeTransformationTemplate<T, 2> {
private:
  Rectangle2DTransformationTemplate(const IntervalTransformationTemplate<T> &tx
                                   ,const IntervalTransformationTemplate<T> &ty
                                   ) {
    setTransformation(0,tx);
    setTransformation(1,ty);
  }

  static Rectangle2DTemplate<T> getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    const NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(xScale);
    const NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(yScale);
    return Rectangle2DTemplate<T>(Point2DTemplate<T>(xInterval.getFrom()  , yInterval.getFrom())
                                 ,Size2DTemplate<T>( xInterval.getLength(), yInterval.getLength())
                                 );
  }
  static Rectangle2DTemplate<T> getDefaultToRectangle() {
    return Rectangle2DTemplate<T>(0, 0, 10, 10);
  }

public:
  Rectangle2DTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    setFromCube(getDefaultFromRectangle(xScale, yScale));
    setToCube(getDefaultToRectangle());
    setScaleType(0, xScale);
    setScaleType(1, yScale);
  }
  template<typename S> Rectangle2DTransformationTemplate(const CubeTransformationTemplate<S, 2> &src)
    : CubeTransformationTemplate<T,2>(src)
  {
  }
  template<typename T1, typename T2> Rectangle2DTransformationTemplate(const CubeTemplate<T1, 2> &from, const CubeTemplate<T2, 2> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    setFromCube(from).setToCube(to).setScaleType(0, xScale).setScaleType(1, yScale);
  }

  template<typename S> Rectangle2DTransformationTemplate &setFromRectangle(const CubeTemplate<S, 2> &rect) {
    setFromCube(rect);
    return *this;
  }
  template<typename S> Rectangle2DTransformationTemplate &setToRectangle(const CubeTemplate<S, 2> &rect) {
    setToCube(rect);
    return *this;
  }
  Rectangle2DTemplate<T>    getFromRectangle() const {
    return getFromCube();
  }
  Rectangle2DTemplate<T>    getToRectangle()   const {
    return getToCube();
  }

  void setScale(IntervalScale newScale, int flags) {
    if((flags & X_AXIS) != 0) {
      setScaleType(0, newScale);
    }
    if((flags & Y_AXIS) != 0) {
      setScaleType(1, newScale);
    }
  }
  // Returns new fromRectangle.
  template<typename T1, typename T2> Rectangle2DTemplate<T> zoom(const FixedSizeVectorTemplate<T1, 2> &v, const T2 &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) {
    if(flags & X_AXIS) {
      (*this)[0].zoom(v[0], factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      (*this)[1].zoom(v[1], factor, pInToRectangle);
    }
    return getFromRectangle();
  }

  // Returns true if transformation is changed
  bool adjustAspectRatio() {
    if(!(*this)[0].isLinear() || !(*this)[1].isLinear()) {
      return false;
    }
    Rectangle2DTemplate<T> fr        = getFromRectangle();
    Rectangle2DTemplate<T> tr        = getToRectangle();
    const T                fromRatio = fabs(fr.getWidth() / fr.getHeight());
    const T                toRatio   = fabs(tr.getWidth() / tr.getHeight());
    bool                   changed   = false;
    if(fromRatio > toRatio) {
      const T dh    = dsign(fr.getHeight())*(fabs(fr.getWidth()/toRatio) - fabs(fr.getHeight()));
      fr.p0()[1]   -= dh / 2;
      fr.size()[1] += dh;
      changed       =  (dh != 0);
    } else if(fromRatio < toRatio) {
      const T dw    = dsign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth()));
      fr.p0()[0]   -= dw / 2;
      fr.size()[0] += dw;
      changed       = (dw != 0);
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }

  static Rectangle2DTransformationTemplate<T> getId() {
    return Rectangle2DTransformationTemplate(Rectangle2DTemplate<T>::getUnit(), Rectangle2DTemplate<T>::getUnit());
  }
};

typedef Rectangle2DTransformationTemplate<double  > Rectangle2DTransformation;
