#include "pch.h"
#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/Function2DSurface.h>
#include "D3DGraphics/MeshArrayJobMonitor.h"

class Function2DPoint {
public:
  Vertex m_p;
  Vertex m_n;
  Function2DPoint(Function2D &f, const Point2D &p);
};

Function2DPoint::Function2DPoint(Function2D &f, const Point2D &p) {
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
static void findMax16BitMeshVertexCount(DIRECT3DDEVICE device) {
  unsigned int l = 100;
  unsigned int h = 0xfffff;
  while(l < h-1) {
    int vertexCount = (l+h)/2;
    LPD3DXMESH mesh = NULL;
    HRESULT hr = D3DXCreateMeshFVF(10000, vertexCount, D3DXMESH_SYSTEMMEM, VertexNormal::FVF_Flags, device, &mesh);
    if(hr != D3D_OK) {
      h = vertexCount - 1;
    } else {
      l = vertexCount;
      mesh->Release();
    }
  }
  AfxMessageBox(format(_T("l:%d, h:%d"), l, h).cstr());
}
#endif

LPD3DXMESH createMeshFrom2DFunction(DIRECT3DDEVICE device, Function2D &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, unsigned int nx, unsigned int ny, bool doubleSided) {
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
      face.addVertexAndNormalIndex(index     , index     ,-1);
      face.addVertexAndNormalIndex(index+1   , index+1   ,-1);
      face.addVertexAndNormalIndex(index+1+ny, index+1+ny,-1);
      face.addVertexAndNormalIndex(index  +ny, index  +ny,-1);
    }
  }
  return mb.createMesh(device, doubleSided);
}

LPD3DXMESH createMesh(DIRECT3DDEVICE device, const Function2DSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=true"));
  }
  ExpressionWrapper f(param.m_expr, param.m_machineCode);
  return createMeshFrom2DFunction(device, f, param.getXInterval(), param.getYInterval(), param.m_pointCount, param.m_pointCount, param.m_doubleSided);
}

class VariableFunction2DMeshCreator : public VariableMeshCreator {
private:
  DIRECT3DDEVICE                    m_device;
  const Function2DSurfaceParameters &m_param;
  mutable ExpressionWrapper         m_expr;
public:
  VariableFunction2DMeshCreator(DIRECT3DDEVICE device, const Function2DSurfaceParameters &param);
  LPD3DXMESH createMesh(double time) const;
};

VariableFunction2DMeshCreator::VariableFunction2DMeshCreator(DIRECT3DDEVICE device, const Function2DSurfaceParameters &param)
: m_device(device)
, m_param(param)
{
  m_expr.compile(m_param.m_expr, m_param.m_machineCode);
  if(!m_expr.ok()) {
    throwException(_T("%s"), m_expr.getErrorMessage().cstr());
  }
}

LPD3DXMESH VariableFunction2DMeshCreator::createMesh(double time) const {
  m_expr.setT(time);
  return createMeshFrom2DFunction(m_device
                                 ,m_expr
                                 ,m_param.getXInterval()
                                 ,m_param.getYInterval()
                                 ,m_param.m_pointCount
                                 ,m_param.m_pointCount
                                 ,m_param.m_doubleSided
                                 );
}

class Function2DMeshArrayJobParameter : public MeshArrayJobParameter {
private:
  DIRECT3DDEVICE                    m_device;
  const Function2DSurfaceParameters &m_param;
public:
  Function2DMeshArrayJobParameter(DIRECT3DDEVICE device, const Function2DSurfaceParameters &param)
    : m_device(device)
    , m_param(param)
  {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_frameCount;
  }
  VariableMeshCreator *fetchMeshCreator() const {
    return new VariableFunction2DMeshCreator(m_device, m_param);
  }
};

MeshArray createMeshArray(CWnd *wnd, DIRECT3DDEVICE device, const Function2DSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  return Function2DMeshArrayJobParameter(device, param).createMeshArray(wnd);
}
