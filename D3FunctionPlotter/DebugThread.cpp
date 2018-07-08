#include "stdafx.h"
#include "DebugThread.h"
#include "D3FunctionPlotterDlg.h"

DebugThread::DebugThread(CD3FunctionPlotterDlg *dlg)
: m_dlg(*dlg)
, m_ok(true)
{
  initThread(true);
  m_surface = new DebugIsoSurface(this,*dlg,dlg->getIsoSurfaceParameters()); TRACE_NEW(m_surface);
}

void DebugThread::initThread(bool singleStep) {
  m_running     = m_finished = false;
  m_breakPoints = 0;
  if(singleStep) m_breakPoints |= BREAKONNEXTFACE;
  breakOnNextLevel(m_dlg.isBreakOnNextLevelChecked());
}

#define CHECKKILLED() { if(m_breakPoints & THREADKILLED) throwException(_T("Killed")); }

DebugThread::~DebugThread() {
  kill();
  for(int i = 0; i < 500; i++) {
    if(!stillActive()) {
      break;
    }
    Sleep(1000);
  }
  if(stillActive()) {
    showWarning(_T("DebugThread still active in destructor"));
  } else {
    SAFEDELETE(m_surface);
  }
}

void DebugThread::singleStep() {
  if(isFinished()) {
    throwException(_T("%s:Thread has exited"),__TFUNCTION__);
  }
  m_breakPoints |= BREAKONNEXTFACE;
  resume();
}

void DebugThread::go() {
  if(isFinished()) {
    throwException(_T("%s:Thread has exited"),__TFUNCTION__);
  }
  m_breakPoints &= ~(BREAKONNEXTFACE|BREAKONNEXTCUBE);
  resume();
}

void DebugThread::goUntilNextCube() {
  if(isFinished()) {
    throwException(_T("%s:Thread has exited"),__TFUNCTION__);
  }
  m_breakPoints &= ~BREAKONNEXTFACE;
  m_breakPoints |= BREAKONNEXTCUBE;
  resume();
}

void DebugThread::breakOnNextLevel(bool on) {
  if(on) {
    m_breakPoints |= BREAKONNEXTLEVEL;
  } else {
    m_breakPoints &= ~BREAKONNEXTLEVEL;
  }
}

void DebugThread::kill() {
  m_breakPoints |= THREADKILLED;
  if(stillActive() && !isRunning()) {
    resume();
  }
}

UINT DebugThread::run() {
  setPropRunning(true);
  try {
    m_surface->createData();
  } catch(Exception e) {
    m_resultMsg = e.what();
    m_ok = false;
  } catch(...) {
    m_resultMsg  = _T("Unknown exception");
    m_ok = false;
  }
  m_finished = true;
  setPropRunning(false);
  return 0;
}

void DebugThread::suspendThread() {
  m_surface->updateSceneObject();
  setPropRunning(false);
  suspend();
  setPropRunning(true);
  CHECKKILLED();
}

void DebugThread::handleStep(StepType type) {
  if(m_breakPoints) {
    CHECKKILLED();
    switch(type) {
    case NEW_FACE :
      if(m_breakPoints & BREAKONNEXTFACE) {
        suspendThread();
      }
      break;
    case NEW_CUBE :
      if(m_breakPoints & (BREAKONNEXTCUBE|BREAKONNEXTFACE)) {
        suspendThread();
      }
      break;
    case NEW_LEVEL:
      if(m_breakPoints & (BREAKONNEXTLEVEL|BREAKONNEXTCUBE|BREAKONNEXTFACE)) {
        suspendThread();
      }
      break;
    }
  }
}

D3SceneObject *DebugThread::getSceneObject() {
  return m_surface->getSceneObject();
}

DebugIsoSurface::DebugIsoSurface(DebugThread *thread, D3SceneContainer &sc, const IsoSurfaceParameters &param)
: m_thread(                *thread)
, m_sc(                     sc)
, m_param(                  param)
, m_exprWrapper(            param.m_expr,param.m_machineCode)
, m_lastVertexCount(        0)
, m_faceCount(              0)
, m_cubeCount(              0)
, m_lastCalculatedFaceCount(0)
, m_lastCalculatedCubeCount(0)
, m_currentLevel(           0)
, m_currentCube(            0,0,0,0)
, m_sceneObject(            sc.getScene())
{
  m_xp = m_exprWrapper.getVariableByName(_T("x"));
  m_yp = m_exprWrapper.getVariableByName(_T("y"));
  m_zp = m_exprWrapper.getVariableByName(_T("z"));
}

SceneObjectWithMesh *DebugIsoSurface::createMeshObject() const {
  D3Scene             &scene = m_sc.getScene();
  SceneObjectWithMesh *obj   = new SceneObjectWithMesh(scene, m_mb.createMesh(scene, m_param.m_doubleSided)); TRACE_NEW(m_sceneObject);
  obj->setFillMode( m_sceneObject.getFillMode());
  obj->setShadeMode(m_sceneObject.getShadeMode());
  m_lastCalculatedFaceCount = m_faceCount;
  return obj;
}

D3WireFrameBox *DebugIsoSurface::createCubeObject() {
  D3Scene &scene = m_sc.getScene();
  D3WireFrameBox *cube = new D3WireFrameBox(scene, *m_currentCube.m_corners[LBN], *m_currentCube.m_corners[RTF]); TRACE_NEW(newCube);
  m_lastCalculatedCubeCount = m_cubeCount;
  return cube;
}

void DebugIsoSurface::createData() {
  Point3D origin(0,0,0);

  m_reverseSign = false; // dont delete this. Used in evaluate !!
  m_reverseSign = m_param.m_originOutside == (evaluate(origin) < 0);

  IsoSurfacePolygonizer polygonizer(*this);
  m_vertexArray = &polygonizer.getVertexArray();

  polygonizer.polygonize(Point3D(0,0,0)
                        ,m_param.m_cellSize
                        ,m_param.m_boundingBox
                        ,m_param.m_tetrahedral
                        ,m_param.m_tetraOptimize4
                        ,m_param.m_adaptiveCellSize
                        );
  if(m_faceCount == 0) {
    throwException(_T("No polygons generated. Cannot create object"));
  }
  m_statistics = polygonizer.getStatistics();
}

String DebugIsoSurface::getInfoMessage() const {
  return m_statistics.toString();
}

double DebugIsoSurface::evaluate(const Point3D &p) {
  *m_xp = p.x;
  *m_yp = p.y;
  *m_zp = p.z;
  if(m_reverseSign) {
    return -getDouble(m_exprWrapper.evaluate());
  } else {
    return getDouble(m_exprWrapper.evaluate());
  }
}

void DebugIsoSurface::receiveFace(const Face3 &face) {
  m_faceCount++;
  const size_t size = m_vertexArray->size();
  if(size > m_lastVertexCount) {
    for(size_t i = m_lastVertexCount; i < size; i++) {
      const IsoSurfaceVertex &sv = (*m_vertexArray)[i];
      m_mb.addVertex(sv.m_position);
      m_mb.addNormal(sv.m_normal  );
    }
    m_lastVertexCount = (DWORD)size;
  }
  Face &f = m_mb.addFace(/*face.m_color*/);
  f.addVertexNormalIndex(face.m_i1, face.m_i1);
  f.addVertexNormalIndex(face.m_i2, face.m_i2);
  f.addVertexNormalIndex(face.m_i3, face.m_i3);

  m_thread.handleStep(NEW_FACE);
}

void DebugIsoSurface::markCurrentCube(const StackedCube &cube) {
  m_cubeCount++;
  m_currentCube = cube;
  if(cube.getLevel() == m_currentLevel) {
    m_thread.handleStep(NEW_CUBE);
  } else {
    m_currentLevel = cube.getLevel();
    m_thread.handleStep(NEW_LEVEL);
  }
}

DebugSceneobject::~DebugSceneobject() {
  deleteMeshObject();
  deleteCubeObject();
}

void DebugSceneobject::setMeshObject(D3SceneObject *obj) {
  deleteMeshObject();
  m_meshObject = obj;
}

void DebugSceneobject::setCubeObject(D3SceneObject *obj) {
  deleteCubeObject();
  m_cubeObject = obj;
}

void DebugSceneobject::deleteMeshObject() {
  if(m_meshObject) {
    m_fillMode  = m_meshObject->getFillMode();
    m_shadeMode = m_meshObject->getShadeMode();
    SAFEDELETE(m_meshObject);
  }
}

void DebugSceneobject::deleteCubeObject() {
  if(m_cubeObject) SAFEDELETE(m_cubeObject);
}
