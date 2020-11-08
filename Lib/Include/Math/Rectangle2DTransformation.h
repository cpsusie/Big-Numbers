#pragma once

#include "CubeTransformationTemplate.h"
#include "Rectangle2D.h"

template<typename CubeType, typename PointType, typename SizeType, typename T> class Rectangle2DTransformationTemplate : public CubeTransformationTemplate<CubeType, PointType, SizeType, T, 2> {
private:
  Rectangle2DTransformationTemplate(const IntervalTransformationTemplate<T> &tx
                                   ,const IntervalTransformationTemplate<T> &ty
                                   ) {
    setTransformation(0,tx);
    setTransformation(1,ty);
  }

  static CubeType getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    const NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(xScale);
    const NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(yScale);
    return CubeType(PointType(xInterval.getFrom()  , yInterval.getFrom())
                   ,SizeType( xInterval.getLength(), yInterval.getLength())
                   );
  }
  static CubeType getDefaultToRectangle() {
    return CubeType(0, 0, 10, 10);
  }

public:
  Rectangle2DTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    setFromCube(getDefaultFromRectangle(xScale, yScale));
    setToCube(getDefaultToRectangle());
    setScaleType(0, xScale);
    setScaleType(1, yScale);
  }
  template<typename CT, typename PT, typename ST, typename S> Rectangle2DTransformationTemplate(const CubeTransformationTemplate<CT,PT,ST,S, 2> &src)
    : CubeTransformationTemplate(src)
  {
  }

  template<typename PT1, typename ST1, typename S1
          ,typename PT2, typename ST2, typename S2>
     Rectangle2DTransformationTemplate(const CubeTemplate<PT1,ST1,S1, 2> &from
                                      ,const CubeTemplate<PT2,ST2,S2, 2> &to
                                      ,IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR)
  {
    setFromCube(from).setToCube(to).setScaleType(0, xScale).setScaleType(1, yScale);
  }

  // Return *this
  template<typename PT, typename ST, typename S> Rectangle2DTransformationTemplate &setFromRectangle(const CubeTemplate<PT,ST,S, 2> &rect) {
    setFromCube(rect);
    return *this;
  }
  // Return *this
  template<typename PT, typename ST, typename S> Rectangle2DTransformationTemplate &setToRectangle(const CubeTemplate<PT,ST,S, 2> &rect) {
    setToCube(rect);
    return *this;
  }
  CubeType getFromRectangle() const {
    return getFromCube();
  }
  CubeType getToRectangle()   const {
    return getToCube();
  }

  // Return *this
  Rectangle2DTransformationTemplate &setScale(IntervalScale newScale, int flags) {
    if((flags & X_AXIS) != 0) {
      setScaleType(0, newScale);
    }
    if((flags & Y_AXIS) != 0) {
      setScaleType(1, newScale);
    }
    return *this;
  }

  // Returns *this
  template<typename T1, typename T2> Rectangle2DTransformationTemplate &zoom(const FixedDimensionVector<T1, 2> &v, const T2 &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) {
    if(flags & X_AXIS) {
      (*this)[0].zoom(v[0], factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      (*this)[1].zoom(v[1], factor, pInToRectangle);
    }
    return *this;
  }

  // Returns true if transformation is changed
  bool adjustAspectRatio() {
    if(!(*this)[0].isLinear() || !(*this)[1].isLinear()) {
      return false;
    }
    CubeType fr = getFromRectangle(), tr = getToRectangle();
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

  template<typename CT, typename PT, typename ST, typename S>
  static Rectangle2DTransformationTemplate<CT,PT,ST,S> &getId(Rectangle2DTransformationTemplate<CT,PT,ST,S> &tr)
  { return Rectangle2DTransformationTemplate<CT,PT,ST,S>(CT::getUnit(), CT::getUnit());
  }
};

typedef Rectangle2DTransformationTemplate<FloatRectangle2D, FloatPoint2D, FloatSize2D, float > FloatRectangle2DTransformation;
typedef Rectangle2DTransformationTemplate<Rectangle2D     , Point2D     , Size2D     , double> Rectangle2DTransformation;
typedef Rectangle2DTransformationTemplate<RealRectangle2D , RealPoint2D , RealSize2D , Real  > RealRectangle2DTransformation;
