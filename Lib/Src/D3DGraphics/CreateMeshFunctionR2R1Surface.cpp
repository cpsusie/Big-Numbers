#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/FunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>
#include <D3DGraphics/MeshCreators.h>

class Function2DPoint {
public:
  Vertex m_p;
  Vertex m_n;
  Function2DPoint(FunctionR2R1 &f, const Point2D &p);
};

Function2DPoint::Function2DPoint(FunctionR2R1 &f, const Point2D &p) {
  const double z = f(p);
  m_p = Vertex(p.x, p.y, z);
#define EPS 1e-5
  Point2D px,py;

  px.x = (fabs(p.x) < 2) ? (p.x+EPS) : p.x * (1+EPS);
  px.y = p.y;
  py.x = p.x;
  py.y = (fabs(p.y) < 2) ? (p.y+EPS) : p.y * (1+EPS);
  const double dfx = (f(px) - z)/(px.x-p.x);
  const double dfy = (f(py) - z)/(py.y-p.y);
  m_n = Vertex(unitVector(D3DXVECTOR3((float)-dfx, (float)-dfy, 1.0f)));
}

#ifdef __NEVER__
static void findMax16BitMeshVertexCount(LPDIRECT3DDEVICE device) {
  unsigned int l = 100;
  unsigned int h = 0xfffff;
  while(l < h-1) {
    int vertexCount = (l+h)/2;
    LPD3DXMESH mesh = NULL;
    HRESULT hr = D3DXCreateMeshFVF(10000, vertexCount, D3DXMESH_SYSTEMMEM, VertexNormal::FVF_Flags, device, &mesh);
    if(hr != D3D_OK) {
      h = vertexCount - 1;
    } else {
      TRACE_CREATE(mesh);
      l = vertexCount;
      SAFERELEASE(mesh);
    }
  }
  showInformation(_T("l:%d, h:%d"), l, h);
}
#endif

LPD3DXMESH createMeshFrom2DFunction(AbstractMeshFactory &amf, FunctionR2R1 &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, unsigned int nx, unsigned int ny, bool doubleSided) {
  nx = max(nx, 2);
  ny = max(ny, 2);

  MeshBuilder mb;
  mb.clear((nx+1)*(ny+1));
  const double  stepx = xInterval.getLength() / (nx-1);
  const double  stepy = yInterval.getLength() / (ny-1);
  Point2D p;
  p.x = xInterval.getFrom();
  for(UINT i = 0; i < nx; i++, p.x += stepx) {
    p.y = yInterval.getFrom();
    for(UINT j = 0; j < ny; j++, p.y += stepy) {
      Function2DPoint fp(f,p);
      mb.addVertex(fp.m_p);
      mb.addNormal(fp.m_n);
    }
  }
  for(UINT i = 1; i < nx; i++) {
    ULONG index = (i-1)*ny;
    for(UINT j = 1; j < ny; j++, index++) {
      Face &face = mb.addFace();
      face.addVertexNormalIndex(index     , index     );
      face.addVertexNormalIndex(index+1   , index+1   );
      face.addVertexNormalIndex(index+1+ny, index+1+ny);
      face.addVertexNormalIndex(index  +ny, index  +ny);
    }
  }
//  debugLog(_T("%s\n"), mb.toString().cstr());
  return mb.createMesh(amf, doubleSided);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionR2R1 &f) {
  checkIsAnimation(__TFUNCTION__, param, false);
  return createMeshFrom2DFunction(amf, f, param.getXInterval(), param.getYInterval(), param.m_pointCount, param.m_pointCount, param.m_doubleSided);
}

class VariableFunction2DMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory                     &m_amf;
  const FunctionR2R1SurfaceParameters     &m_param;
  FunctionWithTimeTemplate<FunctionR2R1> *m_f;
public:
  VariableFunction2DMeshCreator(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f);
  ~VariableFunction2DMeshCreator() {
    SAFEDELETE(m_f);
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const;
};

VariableFunction2DMeshCreator::VariableFunction2DMeshCreator(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f)
: m_amf(amf)
, m_param(param)
, m_f(f.clone())
{
}

LPD3DXMESH VariableFunction2DMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  m_f->setTime(time);
  return createMeshFrom2DFunction(m_amf
                                 ,*m_f
                                 ,m_param.getXInterval()
                                 ,m_param.getYInterval()
                                 ,m_param.m_pointCount
                                 ,m_param.m_pointCount
                                 ,m_param.m_doubleSided
                                 );
}

class Function2DMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory                     &m_amf;
  const FunctionR2R1SurfaceParameters     &m_param;
  FunctionWithTimeTemplate<FunctionR2R1>  &m_f;
public:
  Function2DMeshArrayJobParameter(AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f)
    : m_amf(  amf  )
    , m_param(param)
    , m_f(    f    )
  {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.m_animation.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_animation.getFrameCount();
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    VariableFunction2DMeshCreator *result = new VariableFunction2DMeshCreator(m_amf, m_param, m_f); TRACE_NEW(result);
    return result;
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const FunctionR2R1SurfaceParameters &param, FunctionWithTimeTemplate<FunctionR2R1> &f) {
  checkIsAnimation(__TFUNCTION__, param, true);
  return Function2DMeshArrayJobParameter(amf, param, f).createMeshArray(wnd);
}
