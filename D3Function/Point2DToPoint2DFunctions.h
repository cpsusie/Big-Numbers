#pragma once

#include <Math/Point2D.h>

class MoveTransformation : public FunctionR2R2 {
private:
  const Point2D m_dp;
public:
  MoveTransformation(const Point2D &dp);
  Point2D operator()(const Point2D &p) override;
};

class RectAreaTransformation : public FunctionR2R2 {
private:
  Point2D m_origin, m_u1, m_u2;
  double m_det;
protected:
  const Point2D &getOrigin() const { return m_origin; }
  const Point2D &getU1() const { return m_u1; }
  const Point2D &getU2() const { return m_u2; }
  Point2D toBaseU(const Point2D &p);
  Point2D toView(const Point2D &k);
public:
  RectAreaTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2);
};

class StretchTransformation : public RectAreaTransformation {
private:
  Point2D m_k0, m_step;
public:
  StretchTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2, const Point2D &p0, const Point2D &step);
  Point2D operator()(const Point2D &p) override;
};

class SkewTransformation : public RectAreaTransformation {
private:
  Point2D m_k0, m_step;
public:
  SkewTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2, const Point2D &p0, const Point2D &step);
  Point2D operator()(const Point2D &p) override;
};

class RotateTransformation : public FunctionR2R2 {
private:
  const Point2D m_center;
  const double  m_theta;
public:
  RotateTransformation(const Point2D &center, double theta);
  Point2D operator()(const Point2D &p) override;
};

class MirrorTransformation : public FunctionR2R2 {
private:
  const Point2D m_p1, m_p2, m_u;
public:
  MirrorTransformation(const Point2D &p1, const Point2D &p2);
  Point2D operator()(const Point2D &p) override;
};
