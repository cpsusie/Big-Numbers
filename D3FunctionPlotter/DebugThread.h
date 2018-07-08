#pragma once

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <Thread.h>
#include <PropertyContainer.h>
#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/IsoSurface.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>

typedef enum {
  THREAD_RUNNING
} DebugThreadProperties;

class CD3FunctionPlotterDlg;
class DebugIsoSurface;

#define BREAKONNEXTFACE  0x01
#define BREAKONNEXTCUBE  0x02
#define BREAKONNEXTLEVEL 0x04
#define THREADKILLED     0x08

typedef enum {
  NEW_FACE
 ,NEW_CUBE
 ,NEW_LEVEL
} StepType;

class DebugThread : public Thread, public PropertyContainer {
private:
  CD3FunctionPlotterDlg             &m_dlg;
  DebugIsoSurface                   *m_surface;
  String                             m_resultMsg;
  BYTE                               m_breakPoints;
  bool                               m_running;
  bool                               m_finished;
  bool                               m_ok;
  inline void setPropRunning(bool value) {
    setProperty(THREAD_RUNNING, m_running, value);
  }
  void initThread(bool singleStep);
  void suspendThread();
public:
  DebugThread(CD3FunctionPlotterDlg *dlg);
  ~DebugThread();
  UINT run();
  void singleStep();
  void go();
  void goUntilNextCube();
  void breakOnNextLevel(bool on);
  void kill();
  void handleStep(StepType type);
  inline bool isFinished() const {
    return m_finished;
  }
  inline bool isRunning() const {
    return m_running;
  }
  inline bool isOK() const {
    return m_ok;
  }
  D3SceneObject *getSceneObject();
  const DebugIsoSurface &getDebugSurface() const {
    return *m_surface;
  }
};

class DebugSceneobject : public D3SceneObject {
private:
  D3SceneObject *m_meshObject, *m_cubeObject;
  D3DFILLMODE    m_fillMode;
  D3DSHADEMODE   m_shadeMode;
  void deleteMeshObject();
  void deleteCubeObject();
public:
  DebugSceneobject(D3Scene &scene)
    : D3SceneObject(scene, _T("Debug Polygonizer"))
    , m_meshObject(NULL)
    , m_cubeObject(NULL)
    , m_fillMode(  D3DFILL_WIREFRAME)
    , m_shadeMode( D3DSHADE_FLAT)
  {
  }
  ~DebugSceneobject();
  void setMeshObject(D3SceneObject *obj);
  void setCubeObject(D3SceneObject *obj);
  void draw() {
    if(m_meshObject) m_meshObject->draw();
    if(m_cubeObject) m_cubeObject->draw();
  }
  LPD3DXMESH getMesh() const {
    return m_meshObject ? m_meshObject->getMesh() : NULL;
  }
  bool hasFillMode() const {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode) {
    m_fillMode = fillMode;
    if(m_meshObject) m_meshObject->setFillMode(fillMode);
  }
  D3DFILLMODE getFillMode() const {
    return m_meshObject ? m_meshObject->getFillMode() : m_fillMode;
  }
  bool hasShadeMode() const {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode) {
    m_shadeMode = shadeMode;
    if(m_meshObject) m_meshObject->setShadeMode(shadeMode);
  }
  D3DSHADEMODE getShadeMode() const {
    return m_meshObject ? m_meshObject->getShadeMode() : m_shadeMode;
  }
};

class DebugIsoSurface : public IsoSurfaceEvaluator {
private:
  DebugThread                          &m_thread;
  D3SceneContainer                     &m_sc;
  IsoSurfaceParameters                  m_param;
  ExpressionWrapper                     m_exprWrapper;
  bool                                  m_reverseSign;
  Real                                 *m_xp,*m_yp,*m_zp;
  DWORD                                 m_lastVertexCount;
  DWORD                                 m_faceCount;
  DWORD                                 m_cubeCount;
  mutable DWORD                         m_lastCalculatedFaceCount;
  mutable DWORD                         m_lastCalculatedCubeCount;
  BYTE                                  m_currentLevel;
  const Array<IsoSurfaceVertex>        *m_vertexArray;
  MeshBuilder                           m_mb;
  StackedCube                           m_currentCube;
  DebugSceneobject                      m_sceneObject;
  PolygonizerStatistics                 m_statistics;
  D3WireFrameBox *createCubeObject();
  inline void updateMeshObject() {
    if(m_faceCount > m_lastCalculatedFaceCount) {
      m_sceneObject.setMeshObject(createMeshObject());
    }
  }
  inline void updateCubeObject() {
    if(m_cubeCount > m_lastCalculatedCubeCount) {
      m_sceneObject.setCubeObject(createCubeObject());
    }
  }
public:
  DebugIsoSurface(DebugThread *thread, D3SceneContainer &sc, const IsoSurfaceParameters &param);
  void   createData();
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);
  void   markCurrentCube(const StackedCube &cube);
  String getInfoMessage() const;
  inline D3SceneObject *getSceneObject() {
    return &m_sceneObject;
  }
  inline void updateSceneObject() {
    updateMeshObject();
    updateCubeObject();
  }
  inline int getFaceCount() const {
    return m_faceCount;
  }
  SceneObjectWithMesh *createMeshObject() const;
};
