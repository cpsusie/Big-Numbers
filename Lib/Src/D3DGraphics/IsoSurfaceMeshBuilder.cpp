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
  InterruptableRunnable                *m_interruptable;
  void checkUserAction() const {
    if(m_interruptable) {
      m_interruptable->checkInterruptAndSuspendFlags();
    }
  }
public:
  IsoSurface(const IsoSurfaceParameters &param, FILE *listFile = NULL);
  void   createData(InterruptableRunnable *ir);
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

IsoSurface::IsoSurface(const IsoSurfaceParameters &param, FILE *listFile)
: m_param(param)
, m_exprWrapper(param.m_expr,param.m_machineCode, listFile)
, m_interruptable(NULL)
{
  m_xp = m_exprWrapper.getVariableByName(_T("x"));
  m_yp = m_exprWrapper.getVariableByName(_T("y"));
  m_zp = m_exprWrapper.getVariableByName(_T("z"));
  m_tp = m_exprWrapper.getVariableByName(_T("t"));
}

void IsoSurface::createData(InterruptableRunnable *ir) {
  m_interruptable   = ir;
  try {
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
  } catch (...) {
    m_interruptable = NULL;
    throw;
  }
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
    checkUserAction();
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

LPD3DXMESH createMesh(AbstractMeshFactory &amf, IsoSurface &surface, InterruptableRunnable *ir) {
  try {
    surface.createData(ir);
    return surface.getMeshbuilder().createMesh(amf, surface.getParam().m_doubleSided);
  } catch (...) {
    int fisk = 1;
    throw;
  }
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const IsoSurfaceParameters &param) {
  if(param.m_includeTime) {
    throwInvalidArgumentException(__TFUNCTION__, _T("param.includeTime=true"));
  }
  FileNameSplitter fs(ExpressionWrapper::getDefaultFileName());
  fs.setDir(_T("\\temp\\3Dplot\\ISO"));
  fs.setFileName(fs.getFileName() + param.getName());
  FILE *listFile = MKFOPEN(fs.getFullPath(), _T("w"));
  LPD3DXMESH result = createMesh(amf, IsoSurface(param, listFile), NULL);
  fclose(listFile);
  return result;
}

class VariableIsoSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory &m_amf;
  mutable IsoSurface   m_surface;
public:
  VariableIsoSurfaceMeshCreator(AbstractMeshFactory &amf, const IsoSurfaceParameters &param, FILE *listFile)
  : m_amf(amf)
  , m_surface(param, listFile)
  {
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const;
};

LPD3DXMESH VariableIsoSurfaceMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  try {
    m_surface.setT(time);
    return ::createMesh(m_amf, m_surface, ir);
  } catch (...) {
    int fisk = 1;
    throw;
  }
}

class IsoSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory        &m_amf;
  const IsoSurfaceParameters &m_param;
  String                      m_fileName;
public:
  IsoSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const IsoSurfaceParameters &param)
    : m_amf(amf)
    , m_param(param) {
    m_fileName = format(_T("c:\\temp\\3DPlot\\ISO\\%s.lst"), param.getDisplayName().cstr());
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_frameCount;
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    FILE *listFile = MKFOPEN(m_fileName, _T("w"));

    VariableIsoSurfaceMeshCreator *result = new VariableIsoSurfaceMeshCreator(m_amf, m_param, listFile); TRACE_NEW(result);
    fclose(listFile);
    return result;
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
