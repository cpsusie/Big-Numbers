#include "pch.h"
#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/ParametricSurface.h>
#include "D3DGraphics/MeshArrayJobMonitor.h"

class ParametricSurfacePoint {
public:
  Vertex m_p;
  Vertex m_n;
  ParametricSurfacePoint(ParametricSurface &ps, const Point2D &p);
};

ParametricSurfacePoint::ParametricSurfacePoint(ParametricSurface &f, const Point2D &p) {
  const Point3D v = f(p);
  m_p = Vertex(v.x, v.y, v.z);
#define EPS 1e-5
  Point2D  pt = p, ps = p;

  pt.x = (fabs(p.x) < 2) ? (p.x+EPS) : p.x * (1+EPS);
  ps.y = (fabs(p.y) < 2) ? (p.y+EPS) : p.y * (1+EPS);
  const Point3DP vt = (f(pt) - v) / (pt.x - p.x);
  const Point3DP vs = (f(ps) - v) / (ps.y - p.y);
  m_n = Vertex(unitVector(crossProduct(vt, vs)));
}

class ExprParametricSurface : public ParametricSurface {
private:
  ExpressionWrapper m_exprX, m_exprY, m_exprZ;

  Real *m_tXp, *m_sXp, *m_timeXp;
  Real *m_tYp, *m_sYp, *m_timeYp;
  Real *m_tZp, *m_sZp, *m_timeZp;

public:
  ExprParametricSurface(const ParametricSurfaceParameters &param);
  void setTime(double time) {
    *m_timeXp = *m_timeYp = *m_timeZp = time;
  }
  Point3D operator()(const Point2D &ts);
};

ExprParametricSurface::ExprParametricSurface(const ParametricSurfaceParameters &param)
: m_exprX(param.m_exprX, param.m_machineCode)
, m_exprY(param.m_exprY, param.m_machineCode)
, m_exprZ(param.m_exprZ, param.m_machineCode)
{
  m_tXp    = m_exprX.getVariableByName(_T("t"   ));
  m_sXp    = m_exprX.getVariableByName(_T("s"   ));
  m_timeXp = m_exprX.getVariableByName(_T("time"));

  m_tYp    = m_exprY.getVariableByName(_T("t"   ));
  m_sYp    = m_exprY.getVariableByName(_T("s"   ));
  m_timeYp = m_exprY.getVariableByName(_T("time"));

  m_tZp    = m_exprZ.getVariableByName(_T("t"   ));
  m_sZp    = m_exprZ.getVariableByName(_T("s"   ));
  m_timeZp = m_exprZ.getVariableByName(_T("time"));
}

Point3D ExprParametricSurface::operator()(const Point2D &ts) {
  *m_tXp = *m_tYp = *m_tZp = ts.x;
  *m_sXp = *m_sYp = *m_sZp = ts.y;
  return Point3D(m_exprX.evaluate(), m_exprY.evaluate(), m_exprZ.evaluate());
}

LPD3DXMESH createMeshFromParametricSurface(AbstractMeshFactory &amf, ParametricSurface &sf, const DoubleInterval &tInterval, const DoubleInterval &sInterval, UINT nt, UINT ns, bool doubleSided) {
  nt = max(nt, 2);
  ns = max(ns, 2);

  MeshBuilder mb;
  mb.clear((nt+1)*(ns+1));
  const double  stept = tInterval.getLength() / (nt-1);
  const double  steps = sInterval.getLength() / (ns-1);
  Point2D p;
  p.x = tInterval.getFrom();
  for(UINT i = 0; i < nt; i++, p.x += stept) {
    p.y = sInterval.getFrom();
    for(UINT j = 0; j < ns; j++, p.y += steps) {
      ParametricSurfacePoint sp(sf,p);
      mb.addVertex(sp.m_p);
      mb.addNormal(sp.m_n);
    }
  }
  for(UINT i = 1; i < nt; i++) {
    UINT index = (i-1)*ns;
    for(UINT j = 1; j < ns; j++, index++) {
      Face &face = mb.addFace();
      face.addVertexNormalIndex(index     , index     );
      face.addVertexNormalIndex(index+1   , index+1   );
      face.addVertexNormalIndex(index+1+ns, index+1+ns);
      face.addVertexNormalIndex(index  +ns, index  +ns);
    }
  }
  return mb.createMesh(amf, doubleSided);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ParametricSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=true"));
  }
  ExprParametricSurface ps(param);
  return createMeshFromParametricSurface(amf, ps, param.getTInterval(), param.getSInterval(), param.m_tStepCount, param.m_sStepCount, param.m_doubleSided);
}

class VariableParametricSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory               &m_amf;
  const ParametricSurfaceParameters &m_param;
  mutable ExprParametricSurface      m_ps;
public:
  VariableParametricSurfaceMeshCreator(AbstractMeshFactory &amf, const ParametricSurfaceParameters &param);
  LPD3DXMESH createMesh(double time) const;
};

VariableParametricSurfaceMeshCreator::VariableParametricSurfaceMeshCreator(AbstractMeshFactory &amf, const ParametricSurfaceParameters &param)
: m_amf(amf)
, m_param(param)
, m_ps(param)
{
}

LPD3DXMESH VariableParametricSurfaceMeshCreator::createMesh(double time) const {
  m_ps.setTime(time);
  return createMeshFromParametricSurface(m_amf
                                        ,m_ps
                                        ,m_param.getTInterval()
                                        ,m_param.getSInterval()
                                        ,m_param.m_tStepCount
                                        ,m_param.m_sStepCount
                                        ,m_param.m_doubleSided
                                        );
}

class ParametricSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory               &m_amf;
  const ParametricSurfaceParameters &m_param;
public:
  ParametricSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const ParametricSurfaceParameters &param)
    : m_amf(amf)
    , m_param(param)
  {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_frameCount;
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    return new VariableParametricSurfaceMeshCreator(m_amf, m_param);
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ParametricSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  return ParametricSurfaceMeshArrayJobParameter(amf, param).createMeshArray(wnd);
}
