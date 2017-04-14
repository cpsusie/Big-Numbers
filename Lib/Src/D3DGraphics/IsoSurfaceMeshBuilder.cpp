#include "pch.h"
#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/IsoSurface.h>
#include <D3DGraphics/IsoSurfacePolygonizer.h>
#include "D3DGraphics/MeshArrayJobMonitor.h"

/* ---------------- Implicit surface polygonizer supportfunctions ------------------- */

class IsoSurface : public IsoSurfaceEvaluator {
private:
  IsoSurfaceParameters                  m_param;
  ExpressionWrapper                     m_exprWrapper;
  bool                                  m_reverseSign;
  Real                                 *m_xp,*m_yp,*m_zp,*m_tp;
  int                                   m_lastVertexCount;
  MeshBuilder                           m_mb;
  PolygonizerStatistics                 m_statistics;
  const CompactArray<IsoSurfaceVertex> *m_vertexArray;
  CompactArray<Point3D>                 m_debugPoints;

public:
  IsoSurface(const IsoSurfaceParameters &param);
  void   createData();
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);
  void   receiveDebugVertices(int id,...);
  const  IsoSurfaceParameters &getParam() const {
    return m_param;
  }
  String getInfoMessage() const;
  const MeshBuilder &getMeshbuilder() const {
    return m_mb;
  }

  void setT(double t) {
    *m_tp = t;
  }
  const CompactArray<Point3D> &getDebugPoints() const {
    return m_debugPoints;
  }
};

IsoSurface::IsoSurface(const IsoSurfaceParameters &param) 
: m_param(param)
, m_exprWrapper(param.m_expr,param.m_machineCode)
{
  m_xp = m_exprWrapper.getVariableByName("x");
  m_yp = m_exprWrapper.getVariableByName("y");
  m_zp = m_exprWrapper.getVariableByName("z");
  m_tp = m_exprWrapper.getVariableByName("t");
}

void IsoSurface::createData() {
  Point3D origin(0,0,0);

  m_reverseSign     = false; // dont delete this. Used in evaluate !!
  m_reverseSign     = m_param.m_originOutside == (evaluate(origin) < 0);
  m_lastVertexCount = 0;
  m_mb.clear(30000);

  IsoSurfacePolygonizer polygonizer(*this);
  m_vertexArray = &polygonizer.getVertexArray();

  polygonizer.polygonize(Point3D(0,0,0)
                        ,m_param.m_cellSize
                        ,m_param.m_boundingBox
                        ,m_param.m_tetrahedral);
  if(m_mb.isEmpty()) {
    throwException(_T("No polygons generated. Cannot create object"));
  }

  m_statistics = polygonizer.getStatistics();
  m_mb.validate();
//  m_mb.dump();
//  m_mb.optimize();
}

String IsoSurface::getInfoMessage() const {
  return m_statistics.toString();
}

double IsoSurface::evaluate(const Point3D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  *m_zp = p.z;
  if(m_reverseSign) {
    return -getDouble(m_exprWrapper.evaluate());
  } else {
    return getDouble(m_exprWrapper.evaluate());
  }
}

void IsoSurface::receiveFace(const Face3 &face) {
  const int size = (int)m_vertexArray->size();
  if(size > m_lastVertexCount) {
    for(const IsoSurfaceVertex *sv = &(*m_vertexArray)[m_lastVertexCount], *last = &m_vertexArray->last(); sv <= last; sv++) {
      m_mb.addVertex(sv->m_position);
      m_mb.addNormal(sv->m_normal  );
    }
    m_lastVertexCount = size;
  }
  Face &f = m_mb.addFace();
  f.addVertexNormalIndex(face.m_i1, face.m_i1);
  f.addVertexNormalIndex(face.m_i2, face.m_i2);
  f.addVertexNormalIndex(face.m_i3, face.m_i3);
}

void IsoSurface::receiveDebugVertices(int id,...) {
  va_list argptr;
  va_start(argptr, id);
  m_debugPoints.add((*m_vertexArray)[id].m_position);
  while((id = va_arg(argptr,int)) != -1) {
    m_debugPoints.add((*m_vertexArray)[id].m_position);
  }
  va_end(argptr);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, IsoSurface &surface) {
  surface.createData();
  return surface.getMeshbuilder().createMesh(amf, surface.getParam().m_doubleSided);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const IsoSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=true"));
  }
  return createMesh(amf, IsoSurface(param));
}

class VariableIsoSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory &m_amf;
  mutable IsoSurface   m_surface;
public:
  VariableIsoSurfaceMeshCreator(AbstractMeshFactory &amf, const IsoSurfaceParameters &param)
  : m_amf(amf)
  , m_surface(param)
  {
  }
  LPD3DXMESH createMesh(double time) const;
};

LPD3DXMESH VariableIsoSurfaceMeshCreator::createMesh(double time) const {
  m_surface.setT(time);
  return ::createMesh(m_amf, m_surface);
}

class IsoSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory        &m_amf;
  const IsoSurfaceParameters &m_param;
public:
  IsoSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const IsoSurfaceParameters &param)
    : m_amf(amf)
    , m_param(param) {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_frameCount;
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    return new VariableIsoSurfaceMeshCreator(m_amf, m_param);
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const IsoSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=false"));
  }
  return IsoSurfaceMeshArrayJobParameter(amf, param).createMeshArray(wnd);
}

LPD3DXMESH createSphereMesh(AbstractMeshFactory &amf, double radius) {
  IsoSurfaceParameters param;
  const double bb       = ceil(radius + 1);
  param.setName(format(_T("sphere radius %.2lf"), radius));
  param.m_boundingBox   = Cube3D(Point3D(-bb,-bb,-bb), Point3D( bb, bb, bb));
  param.m_expr          = format(_T("x*x+y*y+z*z-%lf"), radius*radius);
  param.m_machineCode   = true;
  param.m_originOutside = false;
  param.m_cellSize      = radius / 5;
  param.m_tetrahedral   = false;
  param.m_doubleSided   = false;
  return createMesh(amf, param);
}
