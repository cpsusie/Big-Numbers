#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/FunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3TexturePointArray.h>
#include <D3DGraphics/MeshCreators.h>

class Function2DPoint {
public:
  Vertex m_p;
  Vertex m_n;
  Function2DPoint(FunctionR2R1 &f, const Point2D &p, bool calculateNormal);
};

Function2DPoint::Function2DPoint(FunctionR2R1 &f, const Point2D &p, bool calculateNormal) {
  const double z = f(p);
  m_p = Vertex(p[0], p[1], z);
  if(!calculateNormal) return;
#define EPS 1e-5
  Point2D px,py;

  px[0] = (fabs(p[0]) < 2) ? (p[0]+EPS) : p[0] * (1+EPS);
  px[1] = p[1];
  py[0] = p[0];
  py[1] = (fabs(p[1]) < 2) ? (p[1]+EPS) : p[1] * (1+EPS);
  const double dfx = (f(px) - z)/(px[0]-p[0]);
  const double dfy = (f(py) - z)/(py[1]-p[1]);
  m_n = Vertex(unitVector(D3DXVECTOR3((float)-dfx, (float)-dfy, 1.0f)));
}

#define MF_DOUBLESIDED 0x01

LPD3DXMESH createMeshFrom2DFunction(AbstractMeshFactory &amf, FunctionR2R1 &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided, DWORD fvf) {
  nx = max(nx, 2);
  ny = max(ny, 2);

  MeshBuilder   mb;
  mb.clear((nx+1)*(ny+1));
  const double  stepx            = xInterval.getLength() / (nx-1);
  const double  stepy            = yInterval.getLength() / (ny-1);
  const bool    calculateNormals = fvf & D3DFVF_NORMAL;
  const bool    calculateTexture = fvf & D3DFVF_TEX1;
  const D3TexturePointArray uPoints(nx), vPoints(ny);
  const float *uValues = calculateTexture ? uPoints.getBuffer() : nullptr, *vValues = calculateTexture ? vPoints.getBuffer() : nullptr;
  // (u ~ x, v ~ y)
  Point2D p(xInterval.getFrom(), yInterval.getFrom());
  for(UINT i = 0; i < ny; i++, p[1] += stepy) {
    p[0] = xInterval.getFrom();
    for(UINT j = 0; j < nx; j++, p[0] += stepx) {
      Function2DPoint fp(f, p, calculateNormals);
      mb.addVertex(fp.m_p);
      if(calculateNormals) {
        mb.addNormal(fp.m_n);
      }
      if(calculateTexture) {
        mb.addTextureVertex(uValues[j],vValues[i]);
      }
    }
  }
  for(UINT i = 1; i < ny; i++) {
    ULONG inx = (i-1)*nx, inxnx = inx+nx;
    for(const ULONG lastinx = inxnx-1; inx<lastinx; inx++, inxnx++) {
      Face &face = mb.addFace((size_t)4);
      switch(fvf & (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)) {
      case (D3DFVF_XYZ | D3DFVF_NORMAL):
        face.addVertexNormalIndex(inx    , inx    );
        face.addVertexNormalIndex(inxnx  , inxnx  );
        face.addVertexNormalIndex(inxnx+1, inxnx+1);
        face.addVertexNormalIndex(inx  +1, inx  +1);
        break;
      case (D3DFVF_XYZ | D3DFVF_TEX1):
        face.addVertexTextureIndex(inx    , inx    );
        face.addVertexTextureIndex(inxnx  , inxnx  );
        face.addVertexTextureIndex(inxnx+1, inxnx+1);
        face.addVertexTextureIndex(inx  +1, inx  +1);
        break;
      case (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1):
        face.addVertexNormalTextureIndex(inx    , inx    , inx   );
        face.addVertexNormalTextureIndex(inxnx  , inxnx  , inxnx );
        face.addVertexNormalTextureIndex(inxnx+1, inxnx+1,inxnx+1);
        face.addVertexNormalTextureIndex(inx  +1, inx  +1,inxnx+1);
        break;
      default:
        throwInvalidArgumentException(__TFUNCTION__, _T("fvf=%s"), FVFToString(fvf).cstr());
        break;
      }
    }
  }
//  debugLog(_T("%s\n"), mb.toString().cstr());
  return mb.createMesh(amf, doubleSided);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionR2R1 &f) {
  return createMeshFrom2DFunction(amf
                                 ,f
                                 ,param.getXInterval()
                                 ,param.getYInterval()
                                 ,param.m_pointCount
                                 ,param.m_pointCount
                                 ,param.m_doubleSided
                                 ,param.getFVF()
                                 );
}

class VariableFunction2DMeshCreator : public AbstractVariableMeshCreator {
private:
  const FunctionR2R1SurfaceParameters    &m_param;
  FunctionWithTimeTemplate<FunctionR2R1> *m_f;
public:
  VariableFunction2DMeshCreator(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f)
    : AbstractVariableMeshCreator(amf      )
    , m_param(                    param    )
    , m_f(                        f.clone())
  {
  }
  ~VariableFunction2DMeshCreator() override {
    SAFEDELETE(m_f);
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const override;
};

LPD3DXMESH VariableFunction2DMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  m_f->setTime(time);
  return ::createMesh(m_amf, m_param, *m_f);
}

class Function2DMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  const FunctionR2R1SurfaceParameters    &m_param;
  FunctionWithTimeTemplate<FunctionR2R1> &m_f;
public:
  Function2DMeshArrayJobParameter(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f)
    : AbstractMeshArrayJobParameter(amf, param.m_animation)
    , m_param(param)
    , m_f(    f    )
  {
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const override {
    VariableFunction2DMeshCreator *result = new VariableFunction2DMeshCreator(m_amf, m_param, m_f); TRACE_NEW(result);
    return result;
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f) {
  checkIsAnimation(__TFUNCTION__, param, true);
  return Function2DMeshArrayJobParameter(amf, param, f).createMeshArray(wnd);
}
