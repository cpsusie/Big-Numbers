#pragma once

#ifdef DEBUG_POLYGONIZER

#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/IsoSurface.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>

class Debugger;

class DebugSceneobject : public D3SceneObject {
private:
  D3SceneObject  *m_meshObject, *m_cubeObject;
  D3PosDirUpScale m_startPDUS, m_pdus;
  D3DFILLMODE     m_fillMode;
  D3DSHADEMODE    m_shadeMode;
  void deleteMeshObject();
  void deleteCubeObject();
public:
  DebugSceneobject(D3Scene &scene)
    : D3SceneObject(scene, _T("Debug Polygonizer"))
    , m_meshObject(NULL)
    , m_cubeObject(NULL)
    , m_fillMode(D3DFILL_WIREFRAME)
    , m_shadeMode(D3DSHADE_FLAT)
  {
    m_startPDUS = m_scene.getObjPDUS();
  }
  ~DebugSceneobject();
  void setMeshObject(D3SceneObject *obj);
  void setCubeObject(D3SceneObject *obj);
  void draw() {
    if(m_meshObject) m_meshObject->draw();
    if(m_cubeObject) m_cubeObject->draw();
  }
  D3PosDirUpScale &getPDUS() {
    m_pdus = m_startPDUS;
    return m_pdus;
  }
  LPD3DXMESH getMesh() const {
    return m_meshObject ? m_meshObject->getMesh() : NULL;
  }
  bool hasFillMode() const {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode) {
    m_fillMode = fillMode;
    if (m_meshObject) m_meshObject->setFillMode(fillMode);
  }
  D3DFILLMODE getFillMode() const {
    return m_meshObject ? m_meshObject->getFillMode() : m_fillMode;
  }
  bool hasShadeMode() const {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode) {
    m_shadeMode = shadeMode;
    if (m_meshObject) m_meshObject->setShadeMode(shadeMode);
  }
  D3DSHADEMODE getShadeMode() const {
    return m_meshObject ? m_meshObject->getShadeMode() : m_shadeMode;
  }
};

typedef enum {
  NEW_FACE
 ,NEW_CUBE
 ,NEW_LEVEL
} StepType;

class DebugIsoSurface : public IsoSurfaceEvaluator {
private:
  Debugger                      &m_debugger;
  D3SceneContainer              &m_sc;
  IsoSurfaceParameters           m_param;
  ExpressionWrapper              m_exprWrapper;
  IsoSurfacePolygonizer         *m_polygonizer;
  bool                           m_reverseSign;
  Real                          *m_xp, *m_yp, *m_zp;
  DWORD                          m_lastVertexCount;
  DWORD                          m_faceCount;
  DWORD                          m_cubeCount;
  mutable DWORD                  m_lastCalculatedFaceCount;
  mutable DWORD                  m_lastCalculatedCubeCount;
  BYTE                           m_currentLevel;
  const Array<IsoSurfaceVertex> *m_vertexArray;
  MeshBuilder                    m_mb;
  StackedCube                    m_currentCube;
  DebugSceneobject               m_sceneObject;
  PolygonizerStatistics          m_statistics;
  D3SceneObject *createCubeObject();
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
  DebugIsoSurface(Debugger *debugger, D3SceneContainer &sc, const IsoSurfaceParameters &param);
  ~DebugIsoSurface();
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
  const StackedCube &getCurrentCube() const {
    return m_currentCube;
  }
  inline bool hasCurrentCube() const {
    return m_cubeCount > 0;
  }
  const IsoSurfacePolygonizer *getPolygonizer() const {
    return m_polygonizer;
  }
};

#endif // DEBUG_POLYGONIZER
