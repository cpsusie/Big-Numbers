#pragma once

#ifdef ISODEBUGGER

#include <FlagTraits.h>
#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>
#include <D3DGraphics/D3SceneObjectWireFrameBox.h>
#include <D3DGraphics/MeshBuilder.h>

class Debugger;
class D3SceneContainer;
class D3SceneEditor;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

#define MESH_VISIBLE   0x01
#define OCTA_VISIBLE   0x02
#define TETRA_VISIBLE  0x04
#define FACES_VISIBLE  0x08
#define VERTEX_VISIBLE 0x10

class CornerMarkObject;

class OctaObject : public D3SceneObjectWireFrameBox {
private:
  CornerMarkObject *m_cornerMark;
  D3DXVECTOR3       m_cornerCenterArray[8]; // relative to m_center
  const float       m_cellSize;
  int               m_materialId;
  StackedCube       m_cube;
  D3DXVECTOR3       m_center;
  GridLabel         m_cornerLabel[8];
  static D3Cube createCube(float cellSize);
public:
  OctaObject(D3SceneObjectVisual *parent, float cellSize);
  ~OctaObject();
  void setOctagon(const Octagon &octa);
  inline const D3DXVECTOR3 &getCornerCenter(UINT index) const {
    return m_cornerCenterArray[index];
  }
  inline const D3DXVECTOR3 *getCornerCenterArray() const {
    return m_cornerCenterArray;
  }
  inline GridLabel getCornerLabel(UINT index) const {
    return m_cornerLabel[index];
  }
  inline const D3DXVECTOR3 &getCenter() const {
    return m_center;
  }
  int getMaterialId() const {
    return m_materialId;
  }
  inline float getCellSize() const {
    return m_cellSize;
  }
  D3DXMATRIX &getWorld();
  void draw();
  String getInfoString() const;
};

class DebugMeshObject : public D3SceneObjectWithMesh {
private:
  int m_materialId;
  void initMaterial();
public:
  DebugMeshObject(D3SceneObjectVisual *parent, LPD3DXMESH m);
  DebugMeshObject(D3Scene &scene, LPD3DXMESH m);
  ~DebugMeshObject();
  int getMaterialId() const {
    return m_materialId;
  }
  D3DXMATRIX &getWorld();
};

class DebugSceneobject : public D3SceneObjectVisual {
private:
  Debugger             &m_debugger;
  DebugMeshObject      *m_meshObject;
  OctaObject           *m_octaObject;
  D3SceneObjectVisual  *m_tetraObject, *m_facesObject, *m_vertexObject;
  D3DFILLMODE           m_fillMode;
  D3DSHADEMODE          m_shadeMode;
  BYTE                  m_visibleParts;
  float                 m_currentCamDistance;
  int                   m_debugLightIndex;

  void deleteMeshObject();
  void deleteOctaObject();
  void deleteTetraObject();
  void deleteFacesObject();
  void deleteVertexObject();

  void resetCameraFocus(bool resetViewAngleAndDistance);
  void debugRotateFocusCam(const D3DXVECTOR3 &axis, float rad);
  void createDebugLight();
  void destroyDebugLight();
  bool hasDebugLight() const;
  void adjustDebugLightDir();
  inline bool isSet(BYTE part) const {
    return (m_visibleParts & part) != 0;
  }
  D3DXVECTOR3 getCubeCamVector() const;
  void        moveCamToNewCubeCenter();
public:
  DebugSceneobject(D3Scene &scene, Debugger &debugger);
  ~DebugSceneobject();
  void initOctaTetraVertex(OctaObject *octaObject, D3SceneObjectVisual *tetraObject, D3SceneObjectVisual *vertexObject);
  void setMeshObject(      DebugMeshObject     *obj);
  void setFacesObject(     D3SceneObjectVisual *obj);
  bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  void draw();
  D3Camera   *dbgCAM() const;
  bool        hasCubeCenter() const;
  D3DXVECTOR3 getCubeCenter() const;
  void        updateCamDistance();
  void        handleDebuggerPaused();
  inline float getCamDistance() const {
    return m_currentCamDistance;
  }
  LPD3DXMESH getMesh() const {
    return m_meshObject ? m_meshObject->getMesh() : NULL;
  }
  inline OctaObject          *getOctaObject() const {
    return m_octaObject;
  }
  inline D3SceneObjectVisual *getTetraObject() const {
    return m_tetraObject;
  }
  inline D3SceneObjectVisual *getVertexObject() const {
    return m_vertexObject;
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
  inline void setVisibleParts(BYTE visibleParts) {
    m_visibleParts = visibleParts;
  }
};

typedef enum {
  NEW_OCTA
 ,NEW_TETRA
 ,NEW_FACE
 ,NEW_VERTEX
} StepType;

#define HAS_OCTA   0x01
#define HAS_TETRA  0x02
#define HAS_FACE   0x04
#define HAS_VERTEX 0x08
#define HAS_NORMAL 0x10

class FinalDebugIsoSurface;

class DebugIsoSurface : public IsoSurfaceEvaluator, public PropertyChangeListener {
private:
  Debugger                             &m_debugger;
  D3SceneContainer                     &m_sc;
  ExprIsoSurfaceParameters              m_param;
  ExpressionWrapper                     m_exprWrapper;
  IsoSurfacePolygonizer                *m_polygonizer;
  PolygonizerStatistics                 m_statistics;
  bool                                  m_reverseSign;
  Real                                 *m_xp, *m_yp, *m_zp;
  DWORD                                 m_faceCount, m_lastFaceCount, m_lastVertexCount;
  D3DXVECTOR3                           m_currentFaceNormal;

  FLAGTRAITS(DebugIsoSurface, BYTE,m_flags);
  mutable DWORD                         m_octaCount  , m_octaCountObj;
  mutable DWORD                         m_tetraCount , m_tetraCountObj;
  mutable DWORD                         m_visibleFaceCount, m_visibleFaceCountObj;
  mutable DWORD                         m_vertexCount, m_vertexCountObj;
  mutable size_t                        m_visibleVertexArraySizeObj;

  MeshBuilder                           m_mb;
  DebugSceneobject                      m_sceneObject;
  Octagon                               m_currentOcta;
  Tetrahedron                           m_currentTetra;
  CompactArray<Face3>                   m_currentFaceArray;
  IsoSurfaceVertexArray                 m_visibleVertexArray;
  D3SceneObjectVisual *createFacesObject();
  void             updateMeshObject();
  void             updateOctaObject();
  void             updateTetraObject();
  void             updateFacesObject();
  void             updateVertexObject();

  inline DebugIsoSurface &clearCurrentFaceArray() {
    m_currentFaceArray.clear(-1);
    return *this;
  }
  inline DebugIsoSurface &clearVisibleVertexArray() {
    m_visibleVertexArray.clear(-1);
    return *this;
  }
  inline D3Scene  &getScene() const {
    return m_sceneObject.getScene();
  }
  inline D3Camera *dbgCAM() const {
    return m_sceneObject.dbgCAM();
  }
  void debuggerStateChanged(DebuggerState oldState, DebuggerState newState);
public:
  DebugIsoSurface(Debugger *debugger, D3SceneContainer &sc, const ExprIsoSurfaceParameters &param);
  ~DebugIsoSurface();
  void   createData();
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);

  void   markCurrentOcta(  const Octagon          &octa  );
  void   markCurrentTetra( const Tetrahedron      &tetra );
  void   markCurrentFace(  const Face3            &fave  );
  void   markCurrentVertex(const IsoSurfaceVertex &vertex);
  void   asyncKillDebugger();
  void   handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline float getCellSize() const {
    return (float)m_param.m_cellSize;
  }
  inline const IsoSurfaceVertexArray &getVertexArray() const {
    return m_polygonizer->getVertexArray();
  }
  inline DebugSceneobject *getSceneObject() {
    return &m_sceneObject;
  }
  void updateSceneObject(BYTE visibleParts);
  inline int getFaceCount() const {
    return m_faceCount;
  }
  DebugMeshObject      *createMeshObject();
  FinalDebugIsoSurface *createFinalDebugIsoSurface(D3SceneEditor &editor) const;
  inline const Octagon &getCurrentOcta() const {
    return m_currentOcta;
  }
  inline bool hasCurrentOcta() const {
    return isSet(HAS_OCTA);
  }
  inline bool hasVisibleVertexArray() const {
    return m_visibleVertexArray.size() > 0;
  }
  inline const IsoSurfacePolygonizer *getPolygonizer() const {
    return m_polygonizer;
  }
  inline const D3DXVECTOR3 &getCurrentFaceNormal() const {
    return m_currentFaceNormal;
  }
  inline bool hasFaceNormal() const {
    return isSet(HAS_NORMAL);
  }
  String toString() const;
  String getInfoString() const;
};

typedef enum {
  FDIS_CUBEINDEX
} FinalDebugIsoSurfaceProperties;

class FinalDebugIsoSurface : public DebugMeshObject, public PropertyContainer {
private:
  OctaObject           *m_octaObject;
  int                   m_cubeIndex;
  D3SceneEditor        &m_editor;
  const PolygonizerBase m_polygonizer;
  int findCubeIndex(CPoint point) const;
public:
  FinalDebugIsoSurface(D3SceneEditor &editor, LPD3DXMESH m, const PolygonizerBase &polygonizer);
  ~FinalDebugIsoSurface();
  bool OnLButtonDown(UINT nFlags, CPoint point);
  bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

  inline const PolygonizerBase &getPolygonizer() const {
    return m_polygonizer;
  }
  inline int getSelectedCubeIndex() const {
    return m_cubeIndex;
  }
  inline bool hasSelectedCube() const {
    return getSelectedCubeIndex() >= 0;
  }
  void draw();
  String getInfoString() const;
};

#endif // ISODEBUGGER
