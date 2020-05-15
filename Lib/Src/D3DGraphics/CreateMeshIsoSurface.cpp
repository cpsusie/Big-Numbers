#include "pch.h"
#include <InterruptableRunnable.h>
#include <D3DGraphics/IsoSurfaceParameters.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/MeshArrayJobMonitor.h>
#include <D3DGraphics/MeshCreators.h>

class InterruptableIsoSurfaceEvaluator : public IsoSurfaceEvaluator {
private:
  IsoSurfaceParameters          m_param;
  FunctionR3R1                 *m_f;
  void checkUserAction() const {
    if(m_interruptable) {
      m_interruptable->checkInterruptAndSuspendFlags();
    }
  }
protected:
  bool                          m_reverseSign;
  size_t                        m_lastVertexCount;
  MeshBuilder                   m_mb;
  PolygonizerStatistics         m_statistics;
  const IsoSurfaceVertexArray  *m_vertexArray;
  InterruptableRunnable        *m_interruptable;
public:
  InterruptableIsoSurfaceEvaluator(const IsoSurfaceParameters &param);
  InterruptableIsoSurfaceEvaluator &createData(FunctionR3R1 &f, InterruptableRunnable *ir = NULL);
  InterruptableIsoSurfaceEvaluator &createData(FunctionWithTimeTemplate<FunctionR3R1> &f, const Real &time, InterruptableRunnable *ir = NULL);
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);
  String getInfoMessage() const {
    return m_statistics.toString();
  }
  const IsoSurfaceParameters &getParam() const {
    return m_param;
  }
  const MeshBuilder &getMeshbuilder() const {
    return m_mb;
  }
};

InterruptableIsoSurfaceEvaluator::InterruptableIsoSurfaceEvaluator(const IsoSurfaceParameters &param)
: m_param(        param)
, m_f(            NULL )
, m_interruptable(NULL )
, m_vertexArray(  NULL )
{
}

InterruptableIsoSurfaceEvaluator &InterruptableIsoSurfaceEvaluator::createData(FunctionR3R1 &f, InterruptableRunnable *ir) {
  m_f             = &f;
  m_interruptable = ir;
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
                          ,m_param.m_lambda
                          ,m_param.m_boundingBox
                          ,m_param.m_tetrahedral
                          ,m_param.m_tetraOptimize4
                          );
    if(m_mb.isEmpty()) {
      throwException(_T("No polygons generated. Cannot create object"));
    }

    m_statistics = polygonizer.getStatistics();
    m_mb.validate();
    m_mb.optimize();
    m_vertexArray = NULL;
  //  m_mb.dump();
  //  m_mb.optimize();
  } catch(...) {
    m_vertexArray   = NULL;
    m_interruptable = NULL;
    throw;
  }
  return *this;
}

InterruptableIsoSurfaceEvaluator &InterruptableIsoSurfaceEvaluator::createData(FunctionWithTimeTemplate<FunctionR3R1> &f, const Real &time, InterruptableRunnable *ir) {
  f.setTime(time);
  return createData(f, ir);
}

double InterruptableIsoSurfaceEvaluator::evaluate(const Point3D &p) {
  return m_reverseSign ? -getDouble((*m_f)(p)) : getDouble((*m_f)(p));
}

void InterruptableIsoSurfaceEvaluator::receiveFace(const Face3 &face) {
  const size_t size = m_vertexArray->size();
  if(size > m_lastVertexCount) {
    for(size_t i = m_lastVertexCount; i < size; i++) {
      const IsoSurfaceVertex &sv = (*m_vertexArray)[i];
      m_mb.addVertex(sv.m_position);
      m_mb.addNormal(sv.m_normal  );
    }
    m_lastVertexCount = size;
    checkUserAction();
  }
  Face &f = m_mb.addFace(/*face.m_color*/);
  f.addVertexNormalIndex(face.m_i1, face.m_i1);
  f.addVertexNormalIndex(face.m_i2, face.m_i2);
  f.addVertexNormalIndex(face.m_i3, face.m_i3);
}

LPD3DXMESH createMesh(AbstractMeshFactory &amf, const IsoSurfaceParameters &param, FunctionR3R1 &f) {
  checkIsAnimation(__TFUNCTION__, param, false);
  return InterruptableIsoSurfaceEvaluator(param).createData(f, NULL).getMeshbuilder().createMesh(amf, param.m_doubleSided);
}

class VariableIsoSurfaceMeshCreator : public AbstractVariableMeshCreator {
private:
  AbstractMeshFactory                    &m_amf;
  const IsoSurfaceParameters             &m_param;
  FunctionWithTimeTemplate<FunctionR3R1> *m_f;
public:
  VariableIsoSurfaceMeshCreator(AbstractMeshFactory &amf, const IsoSurfaceParameters &param, FunctionWithTimeTemplate<FunctionR3R1> &f)
  : m_amf(  amf      )
  , m_param(param    )
  , m_f(    f.clone())
  {
  }
  ~VariableIsoSurfaceMeshCreator() {
    SAFEDELETE(m_f);
  }
  LPD3DXMESH createMesh(double time, InterruptableRunnable *ir) const;
};

LPD3DXMESH VariableIsoSurfaceMeshCreator::createMesh(double time, InterruptableRunnable *ir) const {
  try {
    return InterruptableIsoSurfaceEvaluator(m_param).createData(*m_f, time, ir).getMeshbuilder().createMesh(m_amf, m_param.m_doubleSided);
  } catch(...) {
    int fisk = 1;
    throw;
  }
}

class IsoSurfaceMeshArrayJobParameter : public AbstractMeshArrayJobParameter {
private:
  AbstractMeshFactory                    &m_amf;
  const IsoSurfaceParameters             &m_param;
  FunctionWithTimeTemplate<FunctionR3R1> &m_f;
public:
  IsoSurfaceMeshArrayJobParameter(AbstractMeshFactory &amf, const IsoSurfaceParameters &param, FunctionWithTimeTemplate<FunctionR3R1> &f)
    : m_amf(amf)
    , m_param(param)
    , m_f(f)
  {
  }
  const DoubleInterval &getTimeInterval() const {
    return m_param.m_animation.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_param.m_animation.getFrameCount();
  }
  AbstractVariableMeshCreator *fetchMeshCreator() const {
    VariableIsoSurfaceMeshCreator *result = new VariableIsoSurfaceMeshCreator(m_amf, m_param, m_f); TRACE_NEW(result);
    return result;
  }
};

MeshArray createMeshArray(CWnd *wnd, AbstractMeshFactory &amf, const IsoSurfaceParameters &param, FunctionWithTimeTemplate<FunctionR3R1> &f) {
  checkIsAnimation(__TFUNCTION__, param, true);
  return IsoSurfaceMeshArrayJobParameter(amf, param, f).createMeshArray(wnd);
}
