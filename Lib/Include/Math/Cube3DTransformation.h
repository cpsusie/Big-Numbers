#pragma once

#include "CubeTransformationTemplate.h"
#include "Cube3D.h"

template<typename T> class Cube3DTransformationTemplate : public CubeTransformationTemplate<T, 3> {
private:
  Cube3DTransformationTemplate(const IntervalTransformationTemplate<T> &tx
                              ,const IntervalTransformationTemplate<T> &ty
                              ,const IntervalTransformationTemplate<T> &tz
                              ) {
    setTransformation(0,tx);
    setTransformation(1,ty);
    setTransformation(2,tz);
  }

  static Cube3DTemplate<T> getDefaultFromCube(IntervalScale xScale, IntervalScale yScale, IntervalScale zScale) {
    const NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(xScale);
    const NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(yScale);
    const NumberInterval<T> zInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(zScale);
    return Cube3DTemplate<T>(Point3DTemplate<T>(xInterval.getFrom()  , yInterval.getFrom()  , zInterval.getFrom())
                            ,Size3DTemplate<T>( xInterval.getLength(), yInterval.getLength(), zInterval.getLength())
                            );
  }
  static Cube3DTemplate<T> getDefaultToCube() {
    return Cube3DTemplate<T>(0, 0, 0, 10, 10, 10);
  }

public:
  Cube3DTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR) {
    setFromCube(getDefaultFromCube(xScale, yScale, zScale));
    setToCube(getDefaultToCube());
    setScaleType(0, xScale);
    setScaleType(1, yScale);
    setScaleType(2, zScale);
  }
  template<typename S> Cube3DTransformationTemplate(const CubeTransformationTemplate<S, 3> &src) 
    : CubeTransformationTemplate<T, 3>(src)
  {
  }
  template<typename T1, typename T2> Cube3DTransformationTemplate(const CubeTemplate<T1, 3> &from, const const CubeTemplate<T2, 3> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR) {
    setFromCube(from).setToCube(to).setScaleType(0, xScale).setScaleType(1, yScale).setScaleType(2, zScale);
  }

  void setScale(IntervalScale newScale, int flags) {
    if((flags & X_AXIS) != 0) {
      setScaleType(0, newScale);
    }
    if((flags & Y_AXIS) != 0) {
      setScaleType(1, newScale);
    }
    if((flags & Z_AXIS) != 0) {
      setScaleType(2, newScale);
    }
  }
  // Returns new fromCube.
  template<typename T1, typename T2> Cube3DTemplate<T> zoom(const FixedSizeVectorTemplate<T1, 2> &v, const T2 &factor, int flags = X_AXIS | Y_AXIS | Z_AXIS, bool pInToCube=true) {
    if(flags & X_AXIS) {
      (*this)[0].zoom(v[0], factor, pInToCube);
    }
    if(flags & Y_AXIS) {
      (*this)[1].zoom(v[1], factor, pInToCube);
    }
    if(flags & Z_AXIS) {
      (*this)[2].zoom(v[2], factor, pInToCube);
    }
    return getFromCube();
  }
  static Cube3DTransformationTemplate<T> getId() {
    return Cube3DTransformationTemplate(Cube3DTemplate<T>::getUnit(), Cube3DTemplate<T>::getUnit());
  }
};

typedef Cube3DTransformationTemplate<float   > FloatCube3DTransformation;
typedef Cube3DTransformationTemplate<double  > Cube3DTransformation;
typedef Cube3DTransformationTemplate<Real    > RealCube3DTransformation;
