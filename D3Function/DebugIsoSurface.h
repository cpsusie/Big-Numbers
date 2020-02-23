#pragma once

#ifdef DEBUG_POLYGONIZER

#include <Math/Expression/ExpressionWrapper.h>
#include <D3DGraphics/IsoSurface.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>
#include <D3DGraphics/MeshBuilder.h>

class Debugger;

#define MESH_VISIBLE   0x01
#define OCTA_VISIBLE   0x02
#define TETRA_VISIBLE  0x04
#define FACES_VISIBLE  0x08
#define VERTEX_VISIBLE 0x10

class DebugSceneobject : public D3SceneObjectVisual {
private:
  D3SceneObjectVisual  *m_meshObject, *m_octaObject, *m_tetraObject, *m_facesObject, *m_vertexObject;
  D3DFILLMODE           m_fillMode;
  D3DSHADEMODE          m_shadeMode;
  BYTE                  m_visibleParts;
  void deleteMeshObject();
  void deleteOctaObject();
  void deleteTetraObject();
  void deleteFacesObject();
  void deleteVertexObject();
public:
  DebugSceneobject(D3Scene &scene);
  ~DebugSceneobject();
  void initOctaTetraVertex(D3SceneObjectVisual *octaObject, D3SceneObjectVisual *tetraObject, D3SceneObjectVisual *vertexObject);
  void setMeshObject(              D3SceneObjectVisual *obj);
  void setFacesObject(             D3SceneObjectVisual *obj);
  void draw();
  LPD3DXMESH getMesh() const {
    return m_meshObject ? m_meshObject->getMesh() : NULL;
  }
  inline D3SceneObjectVisual *getOctaObject() const {
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
  NEW_LEVEL
 ,NEW_OCTA
 ,NEW_TETRA
 ,NEW_FACE
 ,NEW_VERTEX
} StepType;

#define HAS_OCTA   0x01
#define HAS_TETRA  0x02
#define HAS_FACE   0x04
#define HAS_VERTEX 0x08

class DebugIsoSurface : public IsoSurfaceEvaluator {
private:
  Debugger                             &m_debugger;
  D3SceneContainer                     &m_sc;
  IsoSurfaceParameters                  m_param;
  ExpressionWrapper                     m_exprWrapper;
  IsoSurfacePolygonizer                *m_polygonizer;
  PolygonizerStatistics                 m_statistics;
  bool                                  m_reverseSign;
  Real                                 *m_xp, *m_yp, *m_zp;
  DWORD                                 m_faceCount, m_lastFaceCount, m_lastVertexCount;

  BYTE                                  m_flags;
  BYTE                                  m_currentLevel;
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

  void           updateMeshObject();
  void           updateOctaObject();
  void           updateTetraObject();
  void           updateFacesObject();
  void           updateVertexObject();
  inline void    clearCurrentFaceArray() {
    m_currentFaceArray.clear(-1);
  }
  inline void    clearVisibleVertexArray() {
    m_visibleVertexArray.clear(-1);
  }
  inline DebugIsoSurface &setFlags(BYTE f) {
    m_flags |= f;
    return *this;
  }
  inline DebugIsoSurface &clrFlags(BYTE f) {
    m_flags &= ~f;
    return *this;
  }
public:
  DebugIsoSurface(Debugger *debugger, D3SceneContainer &sc, const IsoSurfaceParameters &param);
  ~DebugIsoSurface();
  void   createData();
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);

  void   markCurrentOcta(  const Octagon          &octa  );
  void   markCurrentTetra( const Tetrahedron      &tetra );
  void   markCurrentFace(  const Face3            &fave  );
  void   markCurrentVertex(const IsoSurfaceVertex &vertex);
  String getInfoMessage() const;

  inline D3Scene &getScene() {
    return m_sc.getScene();
  }
  inline float getCellSize() const {
    return (float)m_param.m_cellSize;
  }
  inline BYTE getCurrentLevel() const {
    return m_currentLevel;
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
  D3SceneObjectWithMesh *createMeshObject() const;
  const Octagon &getCurrentOcta() const {
    return m_currentOcta;
  }
  inline bool hasCurrentOcta() const {
    return (m_flags & HAS_OCTA) != 0;
  }
  inline bool hasVisibleVertexArray() const {
    return m_visibleVertexArray.size() > 0;
  }
  const IsoSurfacePolygonizer *getPolygonizer() const {
    return m_polygonizer;
  }
  String toString() const;
};

#endif // DEBUG_POLYGONIZER
