#pragma once

#include "CubeTransformationTemplate.h"
#include "Cube3D.h"

template<typename CubeType, typename PointType, typename SizeType, typename T> class Cube3DTransformationTemplate : public CubeTransformationTemplate<CubeType, PointType, SizeType, T, 3> {
private:
  Cube3DTransformationTemplate(const IntervalTransformationTemplate<T> &tx
                              ,const IntervalTransformationTemplate<T> &ty
                              ,const IntervalTransformationTemplate<T> &tz
                              ) {
    setTransformation(0,tx);
    setTransformation(1,ty);
    setTransformation(2,tz);
  }

  static CubeType getDefaultFromCube(IntervalScale xScale, IntervalScale yScale, IntervalScale zScale) {
    const NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(xScale);
    const NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(yScale);
    const NumberInterval<T> zInterval = IntervalTransformationTemplate<T>::getDefaultFromInterval(zScale);
    return CubeType(PointType(xInterval.getFrom()  , yInterval.getFrom()  , zInterval.getFrom())
                   ,SizeType( xInterval.getLength(), yInterval.getLength(), zInterval.getLength())
                   );
  }
  static CubeType getDefaultToCube() {
    return CubeType(0, 0, 0, 10, 10, 10);
  }

public:
  Cube3DTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR) {
    setFromCube(getDefaultFromCube(xScale, yScale, zScale));
    setToCube(getDefaultToCube());
    setScaleType(0, xScale);
    setScaleType(1, yScale);
    setScaleType(2, zScale);
  }
  template<typename CT, typename PT, typename ST, typename S> Cube3DTransformationTemplate(const CubeTransformationTemplate<CT,PT,ST,S,3> &src)
    : CubeTransformationTemplate(src)
  {
  }
  template<typename PT1, typename ST1, typename S1
          ,typename PT2, typename ST2, typename S2>
     Cube3DTransformationTemplate(const CubeTemplate<PT1,ST1,S1,3> &from
                                 ,const CubeTemplate<PT2,ST2,S2,3> &to
                                 ,IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR)
  {
    setFromCube(from).setToCube(to).setScaleType(0, xScale).setScaleType(1, yScale).setScaleType(2, zScale);
  }

  // Return *this
  Cube3DTransformationTemplate &setScale(IntervalScale newScale, int flags) {
    if((flags & X_AXIS) != 0) {
      setScaleType(0, newScale);
    }
    if((flags & Y_AXIS) != 0) {
      setScaleType(1, newScale);
    }
    if((flags & Z_AXIS) != 0) {
      setScaleType(2, newScale);
    }
    return *this;
  }
  // Returns *this
  template<typename T1, typename T2> Cube3DTransformationTemplate &zoom(const FixedSizeVectorTemplate<T1, 3> &v, const T2 &factor, int flags = X_AXIS | Y_AXIS | Z_AXIS, bool pInToCube=true) {
    if(flags & X_AXIS) {
      (*this)[0].zoom(v[0], factor, pInToCube);
    }
    if(flags & Y_AXIS) {
      (*this)[1].zoom(v[1], factor, pInToCube);
    }
    if(flags & Z_AXIS) {
      (*this)[2].zoom(v[2], factor, pInToCube);
    }
    return *this;
  }
  template<typename CT, typename PT, typename ST, typename S>
  static Cube3DTransformationTemplate<CT,PT,ST,S> &getId(Cube3DTransformationTemplate<CT,PT,ST,S> &tr)
  { return Cube3DTransformationTemplate<CT,PT,ST,S>(CT::getUnit(), CT::getUnit());
  }
};

typedef Cube3DTransformationTemplate<FloatCube3D, FloatPoint3D, FloatSize3D, float   > FloatCube3DTransformation;
typedef Cube3DTransformationTemplate<Cube3D     , Point3D     , Size3D     , double  > Cube3DTransformation;
typedef Cube3DTransformationTemplate<RealCube3D , RealPoint3D , RealSize3D , Real    > RealCube3DTransformation;
