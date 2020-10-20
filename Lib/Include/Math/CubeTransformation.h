#pragma once

#include "IntervalTransformation.h"
#include "Cube3D.h"

template<typename T> class CubeTransformationTemplate {
private:
  IntervalTransformationTemplate<T> *m_transform[3];
  void cleanup() {
    for(int i = 0; i < ARRAYSIZE(m_transform); i++) {
      SAFEDELETE(m_transform[i]);
    }
  }
  void computeTransformation(const Cube3DTemplate<T> &from, const Cube3DTemplate<T> &to, IntervalScale xScale, IntervalScale yScale, IntervalScale zScale) {
    IntervalTransformationTemplate<T> *newXtransform = nullptr, *newYtransform = nullptr, *newZtransform = nullptr;
    try {
      newXtransform = allocateIntervalTransformation(NumberInterval<T>(from.getMinX(), from.getMaxX())
                                                    ,NumberInterval<T>(to.getMinX()  , to.getMaxX()  )
                                                    ,xScale); TRACE_NEW(newXtransform)

      newYtransform = allocateIntervalTransformation(NumberInterval<T>(from.getMinY(), from.getMaxY())
                                                    ,NumberInterval<T>(to.getMinY()  , to.getMaxY()  )
                                                    ,yScale); TRACE_NEW(newYtransform)
      newZtransform = allocateIntervalTransformation(NumberInterval<T>(from.getMinZ(), from.getMaxZ())
                                                    ,NumberInterval<T>(to.getMinZ()  , to.getMaxZ()  )
                                                    ,zScale); TRACE_NEW(newZtransform)
      cleanup();
      m_transform[0] = newXtransform;
      m_transform[1] = newYtransform;
      m_transform[2] = newZtransform;
    } catch(Exception e) {
      SAFEDELETE(newXtransform);
      SAFEDELETE(newYtransform);
      SAFEDELETE(newZtransform);
      throw;
    }
  }

  CubeTransformationTemplate(const IntervalTransformationTemplate<T> &tx, const IntervalTransformationTemplate<T> &ty, , const IntervalTransformationTemplate<T> &tz) {
    m_transform[0] = tx.clone();
    m_transform[1] = ty.clone();
    m_transform[2] = tz.clone();
  }

  static Cube3DTemplate<T> getDefaultFromCube(IntervalScale xScale, IntervalScale yScale, IntervalScale zScale) {
    NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultInterval(xScale);
    NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultInterval(yScale);
    NumberInterval<T> zInterval = IntervalTransformationTemplate<T>::getDefaultInterval(zScale);
    return Cube3DTemplate<T>(xInterval.getFrom(), yInterval.getFrom(), zInterval.getFrom(), xInterval.getLength(), yInterval.getLength(), zInterval.getLength());
  }
  static Cube3DTemplate<T> getDefaultToCube() {
    return Cube3DTemplate<T>(0, 0, 0, 10, 10, 10);
  }

public:
  CubeTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR) {
    m_transform[0] = nullptr; m_transform[1] = nullptr; m_transform[2] = nullptr;
    computeTransformation(getDefaultFromCube(xScale,yScale, zScale), getDefaultToCube(), xScale, yScale);
  }
  CubeTransformationTemplate(const CubeTransformationTemplate &src) {
    m_transform[0] = src.getXTransformation().clone();
    m_transform[1] = src.getYTransformation().clone();
    m_transform[2] = src.getZTransformation().clone();
  }
  CubeTransformationTemplate &operator=(const CubeTransformationTemplate &src) {
    cleanup();
    m_transform[0] = src.getXTransformation().clone();
    m_transform[1] = src.getYTransformation().clone();
    m_transform[2] = src.getZTransformation().clone();
    return *this;
  }
  CubeTransformationTemplate(const Cube3DTemplate<T> &from, const Cube3DTemplate<T> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, IntervalScale zScale = LINEAR) {
    m_transform[0] = nullptr; m_transform[1] = nullptr; m_transform[2] = nullptr;
    computeTransformation(from, to, xScale, yScale, zScale);
  }
  virtual ~CubeTransformationTemplate() {
    cleanup();
  }
  const IntervalTransformationTemplate<T> &getXTransformation() const {
    return *m_transform[0];
  }
  const IntervalTransformationTemplate<T> &getYTransformation() const {
    return *m_transform[1];
  }
  const IntervalTransformationTemplate<T> &getZTransformation() const {
    return *m_transform[2];
  }

  void   setFromCube(const Cube3DTemplate<T> &cube) {
    computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
  }
  void   setToCube(const Cube3DTemplate<T> &cube) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
  }
  Cube3DTemplate<T>  getFromCube() const {
    const Point3DTemplate<T> lbn = Point3DTemplate<T>(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom(), getZTransformation().getFromInterval().getFrom());
    const Point3DTemplate<T> rtf = Point3DTemplate<T>(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo()  , getZTransformation().getFromInterval().getTo()  );
    return Cube3DTemplate<T>(lbn,rtf);
  }
  Cube3DTemplate<T>  getToCube()   const {
    const Point3DTemplate<T> lbn = Point3DTemplate<T>(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom(), getZTransformation().getToInterval().getFrom());
    const Point3DTemplate<T> rtf = Point3DTemplate<T>(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo()  , getZTransformation().getToInterval().getTo()  );
    return Cube3DTemplate<T>(lbn,rtf);
  }
  Point3DTemplate<T>      forwardTransform(const Point3DTemplate<T> &p)   const {
    return Point3DTemplate<T>(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y),getZTransformation().forwardTransform(p.z));
  }
  Point3DTemplate<T>      backwardTransform(const Point3DTemplate<T> &p)   const {
    return Point3DTemplate<T>(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y),getZTransformation().backwardTransform(p.z));
  }
  Point3DTemplate<T>      forwardTransform(const T &x, const T &y, const T &z) const {
    return Point3DTemplate<T>(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y),getZTransformation().forwardTransform(z));
  }
  Point3DTemplate<T>      backwardTransform(const T &x, const T &y, const T &z) const {
    return Point3DTemplate<T>(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y),getZTransformation().backwardTransform(z));
  }
  Cube3DTemplate<T>  forwardTransform(const Cube2DTemplate<T> &cube)  const {
    const Point3DTemplate<T> lbn = forwardTransform(cube.LBN());
    const Point3DTemplate<T> rtf = forwardTransform(rect.RTF());
    return Cube3DTemplate<T>(lbn,rtf);
  }
  Cube3DTemplate<T>  backwardTransform( const Cube3DTemplate<T> &cube)  const {
    const Point3DTemplate<T> lbn = backwardTransform(cube.LBN());
    const Point3DTemplate<T> rtf = backwardTransform(rect.RTF());
    return Cube3DTemplate<T>(lbn,rtf);
  }

  void setScale(IntervalScale newScale, int flags) {
    IntervalScale xScale = getXTransformation().getScale();
    IntervalScale yScale = getYTransformation().getScale();
    IntervalScale zScale = getZTransformation().getScale();
    if((flags & X_AXIS) != 0) {
      xScale = newScale;
    }
    if((flags & Y_AXIS) != 0) {
      yScale = newScale;
    }
    if((flags & Z_AXIS) != 0) {
      zScale = newScale;
    }
    computeTransformation(getFromCube(),getToCube(),xScale,yScale,zScale);
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
