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
  void setPropRunning(bool value);
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
  mutable DWORD                         m_lastCalculatedFaceCount;
  BYTE                                  m_currentLevel;
  const Array<IsoSurfaceVertex>        *m_vertexArray;
  MeshBuilder                           m_mb;
  SceneObjectWithMesh                  *m_sceneObject;
  D3WireFrameBox                       *m_currentCubeObject;
  PolygonizerStatistics                 m_statistics;
  D3DFILLMODE                           m_fillMode;
  D3DSHADEMODE                          m_shadeMode;
  void cleanup();
public:
  DebugIsoSurface(DebugThread *thread, D3SceneContainer &sc, const IsoSurfaceParameters &param);
  virtual ~DebugIsoSurface();
  void   createData();
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);
  void   markCurrentCube(const StackedCube &cube);
  void   updateSceneObject();

  String getInfoMessage() const;
  inline D3SceneObject *getSceneObject() {
    return m_sceneObject;
  }
  inline int getFaceCount() const {
    return m_faceCount;
  }
  SceneObjectWithMesh *createSceneObject() const;
};
