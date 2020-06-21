#include "pch.h"
#include <Math/Point2D3D.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/ParametricR2R3SurfaceParameters.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>
#include <D3DGraphics/MeshCreators.h>

class ParametricSurfacePoint {
public:
  Vertex m_p;
  Vertex m_n;
  ParametricSurfacePoint(FunctionR2R3 &f, const Point2D &p);
};

ParametricSurfacePoint::ParametricSurfacePoint(FunctionR2R3 &f, const Point2D &p) {
  const Point3D v = f(p);
  m_p = Vertex(v.x, v.y, v.z);
#define EPS 1e-5
  Point2D  pt = p, ps = p;

  pt.x = (fabs(p.x) < 2) ? (p.x+EPS) : p.x * (1+EPS);
  ps.y = (fabs(p.y) < 2) ? (p.y+EPS) : p.y * (1+EPS);
  const Point3DP vt = (f(pt) - v) / (pt.x - p.x);
  const Point3DP vs = (f(ps) - v) / (ps.y - p.y);
  m_n = Vertex(unitVector((Point3DP)cross(vt, vs)));
}

LPD3DXMESH createMeshFromParametricSurface(AbstractMeshFactory &amf, FunctionR2R3 &f, const DoubleInterval &tInterval, const DoubleInterval &sInterval, UINT nt, UINT ns, bool doubleSided) {
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
      ParametricSurfacePoint sp(f,p);
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

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionR2R3 &f) {
  checkIsAnimation(__TFUNCTION__, param, false);
  return createMeshFromParametricSurface(amf
                                        ,f
                                        ,param.getTInterval()
                                        ,param.getSInterval()
                                        ,param.m_tStepCount
                                        ,param.m_sStepCount
                                        ,param.m_doubleSided);
}

class VariableParametricSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory                    &m_amf;
  const ParametricR2R3SurfaceParameters  &m_param;
  FunctionWithTimeTemplate<FunctionR2R3> *m_f;
public:
  VariableParametricSurfaceMeshCreator(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f);
  ~VariableParametricSurfaceMeshCreator() {
    SAFEDELETE(m_f);
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const;
};

VariableParametricSurfaceMeshCreator::VariableParametricSurfaceMeshCreator(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f)
: m_amf(amf)
, m_param(param)
, m_f(f.clone())
{
}

LPD3DXMESH VariableParametricSurfaceMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  m_f->setTime(time);
  return createMeshFromParametricSurface(m_amf
                                        ,*m_f
                                        ,m_param.getTInterval()
                                        ,m_param.getSInterval()
                                        ,m_param.m_tStepCount
                                        ,m_param.m_sStepCount
                                        ,m_param.m_doubleSided
                                        );
}

class ParametricSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory                    &m_amf;
  const ParametricR2R3SurfaceParameters  &m_param;
  FunctionWithTimeTemplate<FunctionR2R3> &m_f;
public:
  ParametricSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f)
    : m_amf(  amf  )
    , m_param(param)
    , m_f(    f    )
  {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.m_animation.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_animation.m_frameCount;
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    VariableParametricSurfaceMeshCreator *result = new VariableParametricSurfaceMeshCreator(m_amf, m_param, m_f); TRACE_NEW(result);
    return result;
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f) {
  checkIsAnimation(__TFUNCTION__, param, true);
  return ParametricSurfaceMeshArrayJobParameter(amf, param, f).createMeshArray(wnd);
}
