#include "pch.h"
#include <Math/Point2D3D.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/ParametricR2R3SurfaceParameters.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>
#include <D3DGraphics/D3TexturePointArray.h>
#include <D3DGraphics/MeshCreators.h>

class ParametricSurfacePoint {
public:
  Vertex m_p;
  Vertex m_n;
  ParametricSurfacePoint(FunctionR2R3 &f, const Point2D &p, bool calculateNormal);
};

ParametricSurfacePoint::ParametricSurfacePoint(FunctionR2R3 &f, const Point2D &p, bool calculateNormal) {
  const Point3D v = f(p);
  m_p = v;
  if(calculateNormal) {
#define EPS 1e-5
    Point2D  pt = p, ps = p;

    pt.x = (fabs(p.x) < 2) ? (p.x+EPS) : p.x * (1+EPS);
    ps.y = (fabs(p.y) < 2) ? (p.y+EPS) : p.y * (1+EPS);
    const Point3D vt = (f(pt) - v) / (pt.x - p.x);
    const Point3D vs = (f(ps) - v) / (ps.y - p.y);
    m_n = unitVector(cross(vt, vs));
  }
}

LPD3DXMESH createMeshFromParametricSurface(AbstractMeshFactory &amf, FunctionR2R3 &f, const DoubleInterval &tInterval, const DoubleInterval &sInterval, UINT nt, UINT ns, bool doubleSided, DWORD fvf) {
  nt = max(nt, 2);
  ns = max(ns, 2);

  MeshBuilder mb;
  mb.clear((nt+1)*(ns+1));
  const double  stept            = tInterval.getLength() / (nt-1);
  const double  steps            = sInterval.getLength() / (ns-1);
  const bool    calculateNormals = fvf & D3DFVF_NORMAL;
  const bool    calculateTexture = fvf & D3DFVF_TEX1;
  const D3TexturePointArray uPoints(nt), vPoints(ns);
  const float *uValues = calculateTexture ? uPoints.getBuffer() : NULL, *vValues = calculateTexture ? vPoints.getBuffer() : NULL;
  // (u ~ t, v ~ s)
  Point2D p;
  p.x = tInterval.getFrom();
  for(UINT i = 0; i < nt; i++, p.x += stept) {
    p.y = sInterval.getFrom();
    for(UINT j = 0; j < ns; j++, p.y += steps) {
      ParametricSurfacePoint sp(f,p, calculateNormals);
      mb.addVertex(sp.m_p);
      if(calculateNormals) {
        mb.addNormal(sp.m_n);
      }
      if(calculateTexture) {
        mb.addTextureVertex(uValues[i],vValues[j]);
      }
    }
  }
  for(UINT i = 1; i < nt; i++) {
    UINT index = (i-1)*ns;
    for(UINT j = 1; j < ns; j++, index++) {
      Face &face = mb.addFace((size_t)4);
      switch(fvf & (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)) {
      case (D3DFVF_XYZ | D3DFVF_NORMAL):
        face.addVertexNormalIndex(       index     , index     );
        face.addVertexNormalIndex(       index+1   , index+1   );
        face.addVertexNormalIndex(       index+1+ns, index+1+ns);
        face.addVertexNormalIndex(       index  +ns, index  +ns);
        break;
      case (D3DFVF_XYZ | D3DFVF_TEX1):
        face.addVertexTextureIndex(      index     , index     );
        face.addVertexTextureIndex(      index+1   , index+1   );
        face.addVertexTextureIndex(      index+1+ns, index+1+ns);
        face.addVertexTextureIndex(      index  +ns, index  +ns);
        break;
      case (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1):
        face.addVertexNormalTextureIndex(index     , index     , index     );
        face.addVertexNormalTextureIndex(index+1   , index+1   , index+1   );
        face.addVertexNormalTextureIndex(index+1+ns, index+1+ns, index+1+ns);
        face.addVertexNormalTextureIndex(index  +ns, index  +ns, index  +ns);
        break;
      default:
        throwInvalidArgumentException(__TFUNCTION__, _T("fvf=%s"), FVFToString(fvf).cstr());
        break;
      }
    }
  }
  return mb.createMesh(amf, doubleSided);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionR2R3 &f) {
  return createMeshFromParametricSurface(amf
                                        ,f
                                        ,param.getTInterval()
                                        ,param.getSInterval()
                                        ,param.m_tStepCount
                                        ,param.m_sStepCount
                                        ,param.m_doubleSided
                                        ,param.getFVF()
                                        );
}

class VariableParametricSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  const ParametricR2R3SurfaceParameters  &m_param;
  FunctionWithTimeTemplate<FunctionR2R3> *m_f;
public:
  VariableParametricSurfaceMeshCreator(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f)
    : AbstractVariableMeshCreator(amf      )
    , m_param(                    param    )
    , m_f(                        f.clone())
  {
  }
  ~VariableParametricSurfaceMeshCreator() override {
    SAFEDELETE(m_f);
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const override;
};

LPD3DXMESH VariableParametricSurfaceMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  m_f->setTime(time);
  return ::createMesh(m_amf, m_param, *m_f);
}

class ParametricSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  const ParametricR2R3SurfaceParameters  &m_param;
  FunctionWithTimeTemplate<FunctionR2R3> &m_f;
public:
  ParametricSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f)
    : AbstractMeshArrayJobParameter(amf, param.m_animation)
    , m_param(param)
    , m_f(    f    )
  {
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const override {
    VariableParametricSurfaceMeshCreator *result = new VariableParametricSurfaceMeshCreator(m_amf, m_param, m_f); TRACE_NEW(result);
    return result;
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const ParametricR2R3SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R3> &f) {
  checkIsAnimation(__TFUNCTION__, param, true);
  return ParametricSurfaceMeshArrayJobParameter(amf, param, f).createMeshArray(wnd);
}
