#include "stdafx.h"
#include <Math/MathFunctions.h>
#include "MeshArrayJobMonitor.h"
#include "IsoSurfacePolygonizer.h"

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

IsoSurface::IsoSurface(const IsoSurfaceParameters &param) {
  m_param = param;
  m_exprWrapper.compile(m_param.m_expr,m_param.m_machineCode);
  if(!m_exprWrapper.ok()) {
    throwException(_T("%s"), m_exprWrapper.getErrorMessage().cstr());
  }
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
                        ,m_param.m_size
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
      m_mb.addNormal(sv->m_normal);
    }
    m_lastVertexCount = size;
  }
  Face &f = m_mb.addFace();
  f.addVertexAndNormalIndex(face.m_i1, face.m_i1);
  f.addVertexAndNormalIndex(face.m_i2, face.m_i2);
  f.addVertexAndNormalIndex(face.m_i3, face.m_i3);
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

class IsoSurfaceDebugObject : public SceneObjectWithMesh {
private:
  D3LineArray *m_debugLines;
public:
  IsoSurfaceDebugObject(D3Scene &scene, LPD3DXMESH mesh) : SceneObjectWithMesh(scene, mesh) {
    m_debugLines = NULL;
  }
  ~IsoSurfaceDebugObject() {
    delete m_debugLines;
  }
  void createDebugLines(IsoSurface &surface);
  void draw();
};

void IsoSurfaceDebugObject::draw() {
  SceneObjectWithMesh::draw();
  if(m_debugLines) {
    m_debugLines->draw();
  }
}

void IsoSurfaceDebugObject::createDebugLines(IsoSurface &surface) {
  const CompactArray<Point3D> &pointArray = surface.getDebugPoints();
  if(pointArray.size() > 0) {
    const double  u = surface.getParam().m_size / 8;
    const Point3D e1(u,0,0),e2(0,u,0),e3(0,0,u);
    CompactArray<Line> lines;
    for(size_t i = 0; i < pointArray.size(); i++) {
      const Point3D &p = pointArray[i];
      lines.add(Line(p-e1,p+e1));
      lines.add(Line(p-e2,p+e2));
      lines.add(Line(p-e3,p+e3));
    }
    m_debugLines = new D3LineArray(m_scene, lines.getBuffer(), (int)lines.size());
  }
}

D3SceneObject *createIsoSurfaceDebugObject(D3Scene &scene, const IsoSurfaceParameters &param) {
  IsoSurface surface(param);
  surface.createData();
  LPD3DXMESH mesh = surface.getMeshbuilder().createMesh(scene.getDevice(), param.m_doubleSided);
  IsoSurfaceDebugObject *object = new IsoSurfaceDebugObject(scene, mesh);
  object->createDebugLines(surface);
  return object;
}

LPD3DXMESH createMesh(DIRECT3DDEVICE device, IsoSurface &surface) {
  surface.createData();
  return surface.getMeshbuilder().createMesh(device, surface.getParam().m_doubleSided);
}

LPD3DXMESH createMesh(DIRECT3DDEVICE device, const IsoSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(_T("createMesh"), _T("param.includeTime=true"));
  }
  return createMesh(device, IsoSurface(param));
}

class VariableIsoSurfaceMeshCreator : public VariableMeshCreator {
private:
  DIRECT3DDEVICE     m_device;
  mutable IsoSurface m_surface;
public:
  VariableIsoSurfaceMeshCreator(DIRECT3DDEVICE device, const IsoSurfaceParameters &param)
  : m_device(device)
  , m_surface(param)
  {
  }
  LPD3DXMESH createMesh(double t) const;
};

LPD3DXMESH VariableIsoSurfaceMeshCreator::createMesh(double t) const {
  m_surface.setT(t);
  return ::createMesh(m_device, m_surface);
}

class IsoSurfaceMeshArrayJobParameter : public MeshArrayJobParameter {
private:
  DIRECT3DDEVICE              m_device;
  const IsoSurfaceParameters &m_param;
public:
  IsoSurfaceMeshArrayJobParameter(DIRECT3DDEVICE device, const IsoSurfaceParameters &param)
    : m_device(device)
    , m_param(param) {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.getTInterval();
  }
  int getTimeCount() const {
    return m_param.m_timeCount;
  }
  VariableMeshCreator *fetchMeshCreator() const {
    return new VariableIsoSurfaceMeshCreator(m_device, m_param);
  }
};

MeshArray createMeshArray(CWnd *wnd, DIRECT3DDEVICE device, const IsoSurfaceParameters &param) {
  if(!param.m_includeTime) {
    throwInvalidArgumentException(_T("createMeshArray"), _T("param.includeTime=false"));
  }
  return IsoSurfaceMeshArrayJobParameter(device, param).createMeshArray(wnd);
}

LPD3DXMESH createSphereMesh(DIRECT3DDEVICE device, double radius) {
  IsoSurfaceParameters param;
  const double bb       = ceil(radius + 1);
  param.setName(format(_T("sphere radius %.2lf"), radius));
  param.m_boundingBox   = Cube3D(Point3D(-bb,-bb,-bb), Point3D( bb, bb, bb));
  param.m_expr          = format(_T("x*x+y*y+z*z-%lf"), radius*radius);
  param.m_machineCode   = true;
  param.m_originOutside = false;
  param.m_size          = radius / 5;
  param.m_tetrahedral   = false;
  param.m_doubleSided   = false;
  return createMesh(device, param);
}
