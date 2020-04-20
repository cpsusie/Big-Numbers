#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include <D3DGraphics/D3Cube.h>
#include <D3DGraphics/D3SceneObjectWireFrameBox.h>
#include <D3DGraphics/D3SceneObjectLineArrow.h>
#include <D3DGraphics/D3SceneObjectSolidBox.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"

// ------------------------------------------------------------ Octa Object ----------------------------------------------
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

class CornerMarkObject : public D3SceneObjectSolidBox {
private:
  OctaObject &m_octaObject;
#ifdef USE_SNAPMC
  int         m_materialId[3];
#else
  int         m_materialId[2];
#endif // USE_SNAPMC
  int         m_cornerIndex;
  static D3Cube createCube(float cellSize);
public:
  CornerMarkObject(OctaObject *parent);
  ~CornerMarkObject();
  int getMaterialId() const;
  inline CornerMarkObject &setCornerIndex(int index) {
    m_cornerIndex = index;
    return *this;
  }
  D3DXMATRIX &getWorld();
};

OctaObject::OctaObject(D3SceneObjectVisual *parent, float cellSize)
: D3SceneObjectWireFrameBox(parent, createCube(cellSize), _T("OctaObject"))
, m_cellSize(cellSize)
{
  m_cube.clear();
  m_materialId = getScene().addMaterialWithColor(D3D_BLUE);

  const D3Cube      cube = createCube(cellSize);
  const D3DXVECTOR3 lbn  = cube.getMin(), rtf = cube.getMax();
#ifdef USE_SNAPMC // x={left,right}, y={near,far}, z={bottom,top}
  m_cornerCenterArray[LBN] = D3DXVECTOR3(lbn.x, lbn.y, lbn.z); // left  bottom near corner
  m_cornerCenterArray[LBF] = D3DXVECTOR3(lbn.x, rtf.y, lbn.z); // left  bottom far  corner
  m_cornerCenterArray[LTN] = D3DXVECTOR3(lbn.x, lbn.y, rtf.z); // left  top    near corner
  m_cornerCenterArray[LTF] = D3DXVECTOR3(lbn.x, rtf.y, rtf.z); // left  top    far  corner
  m_cornerCenterArray[RBN] = D3DXVECTOR3(rtf.x, lbn.y, lbn.z); // right bottom near corner
  m_cornerCenterArray[RBF] = D3DXVECTOR3(rtf.x, rtf.y, lbn.z); // right bottom far  corner
  m_cornerCenterArray[RTN] = D3DXVECTOR3(rtf.x, lbn.y, rtf.z); // right top    near corner
  m_cornerCenterArray[RTF] = D3DXVECTOR3(rtf.x, rtf.y, rtf.z); // right top    far  corner
#else
  m_cornerCenterArray[LBN] = D3DXVECTOR3(lbn.x, lbn.y, lbn.z); // left  bottom near corner
  m_cornerCenterArray[LBF] = D3DXVECTOR3(lbn.x, lbn.y, rtf.z); // left  bottom far  corner
  m_cornerCenterArray[LTN] = D3DXVECTOR3(lbn.x, rtf.y, lbn.z); // left  top    near corner
  m_cornerCenterArray[LTF] = D3DXVECTOR3(lbn.x, rtf.y, rtf.z); // left  top    far  corner
  m_cornerCenterArray[RBN] = D3DXVECTOR3(rtf.x, lbn.y, lbn.z); // right bottom near corner
  m_cornerCenterArray[RBF] = D3DXVECTOR3(rtf.x, lbn.y, rtf.z); // right bottom far  corner
  m_cornerCenterArray[RTN] = D3DXVECTOR3(rtf.x, rtf.y, lbn.z); // right top    near corner
  m_cornerCenterArray[RTF] = D3DXVECTOR3(rtf.x, rtf.y, rtf.z); // right top    far  corner
#endif // USE_SNAPMC

  m_cornerMark = new CornerMarkObject(this); TRACE_NEW(m_cornerMark);
}

OctaObject::~OctaObject() {
  SAFEDELETE(m_cornerMark);
  getScene().removeMaterial(m_materialId);
}

D3Cube OctaObject::createCube(float cellSize) {
  const float t = cellSize / 2;
  return D3Cube(D3DXVECTOR3(-t, -t, -t), D3DXVECTOR3(t, t, t));
}

void OctaObject::setOctagon(const Octagon &octa) {
  if(!octa.isEmpty()) {
    m_cube = *octa.getCube();
    m_center = m_cube.getCenter();
    const UINT n = octa.getCornerCount();
    for(UINT i = 0; i < n; i++) {
      m_cornerLabel[i] = m_cube.m_corners[i]->m_label;
    }
  }
}

D3DXMATRIX &OctaObject::getWorld() {
  D3World w(m_parent->getWorld());
  return m_world = w.setPos(w.getPos() + rotate(m_center, w.getOrientation()));
}

void OctaObject::draw() {
  __super::draw();
  for(UINT i = 0; i < ARRAYSIZE(m_cornerCenterArray); i++) {
    m_cornerMark->setCornerIndex(i).draw();
  }
}

String OctaObject::getInfoString() const {
  return m_cube.toString(3);
}

#define NEGATIVECOLOR D3DCOLOR_XRGB(120, 25, 30)
#define ZEROCOLOR     D3DCOLOR_XRGB(110,110,110)
#define POSITIVECOLOR D3DCOLOR_XRGB( 33,150, 28)

CornerMarkObject::CornerMarkObject(OctaObject *parent)
: D3SceneObjectSolidBox(parent, createCube(parent->getCellSize() / 10), _T("CornerMark"))
, m_octaObject(*parent)
{
#ifdef USE_SNAPMC
  m_materialId[V_NEGATIVE] = getScene().addMaterialWithColor(NEGATIVECOLOR);
  m_materialId[V_ZERO    ] = getScene().addMaterialWithColor(ZEROCOLOR    );
  m_materialId[V_POSITIVE] = getScene().addMaterialWithColor(POSITIVECOLOR);
#else
  m_materialId[false     ] = getScene().addMaterialWithColor(NEGATIVECOLOR);
  m_materialId[true      ] = getScene().addMaterialWithColor(POSITIVECOLOR);
#endif // USE_SNAPMC
}

CornerMarkObject::~CornerMarkObject() {
  for(size_t i = 0; i < ARRAYSIZE(m_materialId); i++) {
    getScene().removeMaterial(m_materialId[i]);
  }
}

D3Cube CornerMarkObject::createCube(float cellSize) {
  const float t = cellSize / 2;
  return D3Cube(D3DXVECTOR3(-t,-t,-t), D3DXVECTOR3(t,t,t));
}

int CornerMarkObject::getMaterialId() const {
  return m_materialId[m_octaObject.getCornerLabel(m_cornerIndex)];
}

D3DXMATRIX &CornerMarkObject::getWorld() {
  D3World w = m_parent->getWorld();
  return m_world = w.setPos(w.getPos()+rotate(m_octaObject.getCornerCenter(m_cornerIndex),w.getOrientation()));
}

// ----------------------------------------------------Tetra Object ----------------------------------------------

class SubTetraObject : public D3SceneObjectLineArray {
public:
  SubTetraObject(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lineArray, const String &name)
    : D3SceneObjectLineArray(parent, lineArray, name)
  {
  }
  int getMaterialId() const {
    return m_parent->getMaterialId();
  }
  D3DXMATRIX &getWorld() {
    return m_parent->getWorld();
  }
};

class TetraObject : public D3SceneObjectVisual {
private:
  OctaObject                       &m_octaObject;
  CompactHashMap<Tetrahedron, UINT> m_objectCache;
  int                               m_materialId;
  int                               m_currentVisible;
  CompactArray<Line3D> createLineArray(const Tetrahedron &tetra) const;
  UINT                 getLinesObject( const Tetrahedron &tetra);
public:
  TetraObject(OctaObject *octaObject);
  ~TetraObject();
  void setTetrahedron(const Tetrahedron &tetra);
  int getMaterialId() const {
    return m_materialId;
  }
  void selectVisibleChild(int index);
  D3DXMATRIX &getWorld() {
    return m_octaObject.getWorld();
  }
  void draw() {
    if(m_currentVisible >= 0) {
      getChild(m_currentVisible)->draw();
    }
  }
};

TetraObject::TetraObject(OctaObject *octaObject)
: D3SceneObjectVisual(octaObject, _T("Tetra"))
, m_octaObject(*octaObject)
, m_currentVisible(-1)
{
  m_materialId = getScene().addMaterialWithColor(D3D_YELLOW);
}

TetraObject::~TetraObject() {
  getScene().removeMaterial(m_materialId);
}

void TetraObject::setTetrahedron(const Tetrahedron &tetra) {
  const UINT index = getLinesObject(tetra);
  selectVisibleChild(index);
}

void TetraObject::selectVisibleChild(int index) {
  m_currentVisible = index;
}

UINT TetraObject::getLinesObject(const Tetrahedron &tetra) {
  UINT *index = m_objectCache.get(tetra);
  if(index != NULL) {
    return *index;
  }
  const UINT newIndex = addChild(new SubTetraObject(this, createLineArray(tetra), tetra.toString()));
  m_objectCache.put(tetra, newIndex);
  return newIndex;
}

CompactArray<Line3D> TetraObject::createLineArray(const Tetrahedron &tetra) const {
  const D3DXVECTOR3 *corners = m_octaObject.getCornerCenterArray();
  CompactArray<Line3D> result(6);
  const UINT n = tetra.getCornerCount();
  for(UINT i = 0; i < n; i++) {
    for(UINT j = i+1; j < n; j++) {
      result.add(Line3D(corners[tetra.getCorner(i)], corners[tetra.getCorner(j)]));
    }
  }
  return result;
}

// ------------------------------------------------------------Face Object ----------------------------------------------

class ColoredFaceObject : public D3SceneObjectLineArray {
  int m_materialId;
public:
  ColoredFaceObject(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lines, D3DCOLOR color);
  ~ColoredFaceObject() {
    getScene().removeMaterial(m_materialId);
  }
  int getMaterialId() const {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() {
    return m_parent->getWorld();
  }
};

ColoredFaceObject::ColoredFaceObject(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lines, D3DCOLOR color)
  : D3SceneObjectLineArray(parent, lines)
{
  m_materialId = getScene().addMaterialWithColor(color);
}

class FacesObject : public D3SceneObjectVisual {
private:
  CompactArray<Line3D> createLineArray(const Face3 &face,              const IsoSurfaceVertexArray &va) const;
  CompactArray<Line3D> createLineArray(CompactArray<Face3> &faceArray, const IsoSurfaceVertexArray &va) const;
public:
  FacesObject(DebugIsoSurface *dbgObject, CompactArray<Face3> &faceArray);
};

FacesObject::FacesObject(DebugIsoSurface *dbgObject, CompactArray<Face3> &faceArray)
: D3SceneObjectVisual(dbgObject->getScene(), _T("FaceObject"))
{
  if(faceArray.isEmpty()) return;
  const IsoSurfaceVertexArray &va = dbgObject->getVertexArray();
  addChild(new ColoredFaceObject(this, createLineArray(faceArray.last(), va), D3D_CYAN));
  if(faceArray.size() > 1) {
    addChild(new ColoredFaceObject(this, createLineArray(faceArray, va), D3D_BLACK));
  }
}

CompactArray<Line3D> FacesObject::createLineArray(const Face3 &face, const IsoSurfaceVertexArray &va) const {
  CompactArray<Line3D> result;
  result.add(Line3D(va[face.m_i1].m_position, va[face.m_i2].m_position));
  result.add(Line3D(va[face.m_i2].m_position, va[face.m_i3].m_position));
  result.add(Line3D(va[face.m_i3].m_position, va[face.m_i1].m_position));

  result.add(Line3D(va[face.m_i1].m_position, va[face.m_i3].m_position));
  result.add(Line3D(va[face.m_i3].m_position, va[face.m_i2].m_position));
  result.add(Line3D(va[face.m_i2].m_position, va[face.m_i1].m_position));
  return result;
}

CompactArray<Line3D> FacesObject::createLineArray(CompactArray<Face3> &faceArray, const IsoSurfaceVertexArray &va) const {
  CompactArray<Line3D> result;
  size_t n = faceArray.size();
  if(n <= 1) {
    return result;
  }
  n--;
  for(size_t i = 0; i < n; i++) {
    result.addAll(createLineArray(faceArray[i], va));
  }
  return result;
}

// ------------------------------------------------------------Vertex Object ----------------------------------------------

class VertexObject;
class NormalArrowObject : public D3SceneObjectLineArrow {
private:
  VertexObject *m_vertexObject;
  const float   m_scale;
  int           m_materialId;
public:
  NormalArrowObject(VertexObject *vertexObject, float scale);
  ~NormalArrowObject();
  int getMaterialId() const {
    return m_materialId;
  }
  D3DXMATRIX &getWorld();
};

class VertexObject : public D3SceneObjectLineArray {
  friend class NormalArrowObject;
private:
  IsoSurfaceVertexArray     m_vertexArray;
  size_t                    m_positionIndex;
  int                       m_materialId;
  D3World                   m_octaWorld;
  NormalArrowObject        *m_normalObject;
  CompactArray<Line3D> createLineArray(float lineLength) const;
public:
  VertexObject(OctaObject *octaObject);
  ~VertexObject();
  void setSurfaceVertexArray(const IsoSurfaceVertexArray &a);
  int getMaterialId() const {
    return m_materialId;
  }
  const IsoSurfaceVertex &getCurrentVertex() const {
    return m_vertexArray[m_positionIndex];
  }
  D3DXMATRIX &getWorld() {
    return m_world = D3World(m_octaWorld).setPos(rotate(getCurrentVertex().m_position, m_octaWorld.getOrientation()));
  }
  void draw();
};

VertexObject::VertexObject(OctaObject *octaObject) 
: D3SceneObjectLineArray(octaObject, createLineArray(octaObject->getCellSize() / 25))
{
  m_materialId   = getScene().addMaterialWithColor(D3D_WHITE);
  m_normalObject = new NormalArrowObject(this, octaObject->getCellSize() / 3); TRACE_NEW(m_normalObject);
}

VertexObject::~VertexObject() {
  SAFEDELETE(m_normalObject);
  getScene().removeMaterial(m_materialId);
}

void VertexObject::setSurfaceVertexArray(const IsoSurfaceVertexArray &a) {
  m_vertexArray = a;
}

CompactArray<Line3D> VertexObject::createLineArray(float lineLength) const {
  CompactArray<Line3D> result;
  float d = lineLength;
  const D3DXVECTOR3 v1( d, d, d);
  const D3DXVECTOR3 v2( d, d,-d);
  const D3DXVECTOR3 v3( d,-d, d);
  const D3DXVECTOR3 v4(-d, d, d);
  result.add(Line3D(-v1, +v1));
  result.add(Line3D(-v2, +v2));
  result.add(Line3D(-v3, +v3));
  result.add(Line3D(-v4, +v4));
  return result;
}

void VertexObject::draw() {
  const size_t n = m_vertexArray.size();
  if(n == 0) return;
  m_octaWorld = getParent()->getWorld();
  for(m_positionIndex = 0; m_positionIndex < n; m_positionIndex++) {
    __super::draw();
    m_normalObject->draw();
  }
}

// --------------------------------------- NormalObject -------------------------------------------

#define D3D_DARKCYAN       D3DCOLOR_XRGB(128,  0,128)

NormalArrowObject::NormalArrowObject(VertexObject *vertexObject, float scale)
: D3SceneObjectLineArrow(vertexObject->getScene(), D3DXORIGIN, Vertex(1, 0, 0))
, m_vertexObject(vertexObject)
, m_scale(scale)
{
  m_materialId = getScene().addMaterialWithColor(D3D_DARKCYAN);
}

NormalArrowObject::~NormalArrowObject() {
  getScene().removeMaterial(m_materialId);
}

D3DXMATRIX &NormalArrowObject::getWorld() {
  return m_world = D3World(m_vertexObject->getWorld())
    .setOrientation(createOrientation(m_vertexObject->getCurrentVertex().m_normal))
    .setScaleAll(m_scale);
}

// ---------------------------------------- DebugMeshObject -----------------------------------------
class DebugMeshObject : public D3SceneObjectWithMesh {
private:
  int m_materialId;
public:
  DebugMeshObject(D3Scene &scene, LPD3DXMESH m);
  ~DebugMeshObject();
  int getMaterialId() const {
    return m_materialId;
  }
};

DebugMeshObject::DebugMeshObject(D3Scene &scene, LPD3DXMESH m)
: D3SceneObjectWithMesh(scene, m)
{
  m_materialId = getScene().addMaterialWithColor(D3D_BLACK);
}

DebugMeshObject::~DebugMeshObject() {
  getScene().removeMaterial(m_materialId);
}

// ---------------------------------------- FinalMeshObject -----------------------------------------
class FinalMeshObject : public DebugMeshObject {
private:
  OctaObject           *m_octaObject;
  int                   m_cubeIndex;
  D3SceneEditor        &m_editor;
  const PolygonizerBase m_polygonizer;
  int findCubeIndex(CPoint point) const;
public:
  FinalMeshObject(D3SceneEditor &editor, LPD3DXMESH m, const PolygonizerBase &polygonizer);
  ~FinalMeshObject();
  bool OnLButtonDown(UINT nFlags, CPoint point);
  inline const PolygonizerBase &getPolygonizer() const {
    return m_polygonizer;
  }
  void draw();
  String getInfoString() const;

};

FinalMeshObject::FinalMeshObject(D3SceneEditor &editor, LPD3DXMESH m, const PolygonizerBase &polygonizer)
: DebugMeshObject(*editor.getScene(), m)
, m_octaObject(NULL)
, m_editor(editor)
, m_polygonizer(polygonizer)
, m_cubeIndex(-1)
{
}

FinalMeshObject::~FinalMeshObject() {
  SAFEDELETE(m_octaObject);
}

bool FinalMeshObject::OnLButtonDown(UINT nFlags, CPoint point) {
  if(nFlags != 1) return false;
  m_cubeIndex = findCubeIndex(point);
  if(m_cubeIndex >= 0) {
    if(m_octaObject == NULL) {
      m_octaObject = new OctaObject(this, (float)m_polygonizer.getCellSize());
    }
    m_octaObject->setOctagon(Octagon(&m_polygonizer, m_cubeIndex));
    m_editor.renderActiveCameras(SC_RENDERALL);
    return true;
  }
  return false;
}

int FinalMeshObject::findCubeIndex(CPoint point) const {
  if(m_editor.getCurrentVisual() == this) {
    const D3DXVECTOR3 mp = m_editor.getPickedInfo().m_info.getMeshPoint();
    Point3D mp3(mp.x,mp.y,mp.z);
    const CompactArray<StackedCube> &table = m_polygonizer.getCubeArray();
    const size_t n = table.size();
    for(size_t i = 0; i < n; i++) {
      const StackedCube &cube = table[i];
      if(cube.contains(mp3)) {
        return (int)i;
      }
    }
  }
  return -1;
}

void FinalMeshObject::draw() {
  __super::draw();
  if(m_octaObject) {
    m_octaObject->draw();
  }
}

String FinalMeshObject::getInfoString() const {
  String result = __super::getInfoString();
  if(m_octaObject) {
    result += _T("\n");
    result = format(_T("CubeIndex:%d, "), m_cubeIndex);
    result += m_octaObject->getInfoString();
  }
  return result;
}

// --------------------------------------- DebugIsoSurface -------------------------------------------

DebugIsoSurface::DebugIsoSurface(Debugger *debugger, D3SceneContainer &sc, const IsoSurfaceParameters &param)
  : m_debugger(*debugger)
  , m_sc(sc)
  , m_param(param)
  , m_polygonizer(NULL)
  , m_exprWrapper(param.m_expr, param.m_machineCode)
  , m_reverseSign(false)
  , m_lastVertexCount(          0)
  , m_faceCount(                0)
  , m_lastFaceCount(            0)
  , m_flags(                    0)
  , m_octaCount(                0)
  , m_octaCountObj(             0)
  , m_tetraCount(               0)
  , m_tetraCountObj(            0)
  , m_visibleFaceCount(         0)
  , m_visibleFaceCountObj(      0)
  , m_vertexCount(              0)
  , m_vertexCountObj(           0)
  , m_visibleVertexArraySizeObj(0)
  , m_sceneObject(sc.getScene())
{
  OctaObject   *octaObject   = new OctaObject(&m_sceneObject, (float)param.m_cellSize);
  TetraObject  *tetraObject  = new TetraObject( octaObject);
  VertexObject *vertexObject = new VertexObject(octaObject);
  m_sceneObject.initOctaTetraVertex(octaObject, tetraObject, vertexObject);

  m_xp = m_exprWrapper.getVariableByName(_T("x"));
  m_yp = m_exprWrapper.getVariableByName(_T("y"));
  m_zp = m_exprWrapper.getVariableByName(_T("z"));
}

DebugIsoSurface::~DebugIsoSurface() {
  SAFEDELETE(m_polygonizer)
}

void DebugIsoSurface::createData() {
  Point3D origin(0, 0, 0);

  m_reverseSign = false; // dont delete this. Used in evaluate !!
  m_reverseSign = m_param.m_originOutside == (evaluate(origin) < 0);

  m_polygonizer = new IsoSurfacePolygonizer(*this); TRACE_NEW(m_polygonizer);

  m_polygonizer->polygonize(Point3D(0, 0, 0)
                           ,m_param.m_cellSize
                           ,m_param.m_lambda
                           ,m_param.m_boundingBox
                           ,m_param.m_tetrahedral
                           ,m_param.m_tetraOptimize4
//                           ,m_param.m_adaptiveCellSize
                           );
  if(m_faceCount == 0) {
    throwException(_T("No polygons generated. Cannot create object"));
  }
  m_statistics = m_polygonizer->getStatistics();
}

String DebugIsoSurface::getInfoMessage() const {
  return m_statistics.toString();
}

D3Scene &DebugIsoSurface::getScene() {
  return m_sc.getScene();
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
  const IsoSurfaceVertexArray &va = getVertexArray();
  const size_t size = va.size();
  if(size > m_lastVertexCount) {
    for(size_t i = m_lastVertexCount; i < size; i++) {
      const IsoSurfaceVertex &sv = va[i];
      m_mb.addVertex(sv.m_position);
      m_mb.addNormal(sv.m_normal);
    }
    m_lastVertexCount = (DWORD)size;
  }
  Face &f = m_mb.addFace(/*face.m_color*/);
  f.addVertexNormalIndex(face.m_i1, face.m_i1);
  f.addVertexNormalIndex(face.m_i2, face.m_i2);
  f.addVertexNormalIndex(face.m_i3, face.m_i3);
}

void DebugIsoSurface::updateSceneObject(BYTE visibleParts) {
  updateMeshObject();
  updateOctaObject();
  updateTetraObject();
  updateFacesObject();
  updateVertexObject();
  m_sceneObject.setVisibleParts(visibleParts);
}

void DebugIsoSurface::markCurrentOcta(const Octagon &octa) {
  m_octaCount++;
  m_currentOcta = octa;
  m_flags = HAS_OCTA;
  clearVisibleVertexArray();
  clearCurrentFaceArray();
  m_debugger.handleStep(NEW_OCTA);
}

void DebugIsoSurface::markCurrentTetra(const Tetrahedron &tetra) {
  m_tetraCount++;
  m_currentTetra = tetra;
  setFlags(HAS_TETRA).clrFlags(HAS_VERTEX);
  clearVisibleVertexArray();
  m_debugger.handleStep(NEW_TETRA);
}

void DebugIsoSurface::markCurrentFace(const Face3 &f) {
  m_visibleFaceCount++;
  m_currentFaceArray.add(f);
  setFlags(HAS_FACE).clrFlags(HAS_VERTEX);
  m_debugger.handleStep(NEW_FACE);
}

void DebugIsoSurface::markCurrentVertex(const IsoSurfaceVertex &v) {
  m_vertexCount++;
  setFlags(HAS_VERTEX);
  if(m_visibleVertexArray.size() == 3) {
    m_visibleVertexArray.clear(-1);
  }
  m_visibleVertexArray.add(v);
  m_debugger.handleStep(NEW_VERTEX);
}

// ---------------------------------------- DebugIsoSurface -----------------------------------------
D3SceneObjectVisual *DebugIsoSurface::createMeshObject() const {
  D3Scene         &scene = m_sc.getScene();
  DebugMeshObject *obj = new DebugMeshObject(scene, m_mb.createMesh(scene, m_param.m_doubleSided)); TRACE_NEW(obj);
  obj->setFillMode(m_sceneObject.getFillMode());
  obj->setShadeMode(m_sceneObject.getShadeMode());
  return obj;
}

D3SceneObjectVisual *DebugIsoSurface::createFinalMeshObject(D3SceneEditor &editor) const {
  D3Scene         &scene = *editor.getScene();
  FinalMeshObject *obj = new FinalMeshObject(editor, m_mb.createMesh(scene, m_param.m_doubleSided), *m_polygonizer); TRACE_NEW(obj);
  obj->setFillMode( m_sceneObject.getFillMode());
  obj->setShadeMode(m_sceneObject.getShadeMode());
  return obj;
}

D3SceneObjectVisual *DebugIsoSurface::createFacesObject() {
  D3SceneObjectVisual *v = new FacesObject(this, m_currentFaceArray); TRACE_NEW(v);
  return v;
}

void DebugIsoSurface::updateMeshObject() {
  if(m_faceCount > m_lastFaceCount) {
    m_sceneObject.setMeshObject(createMeshObject());
    m_lastFaceCount = m_faceCount;
  }
}

void DebugIsoSurface::updateOctaObject() {
  if((m_flags & HAS_OCTA) == 0) {
    m_octaCountObj = 0;
  } else if(m_octaCount > m_octaCountObj) {
    ((OctaObject*)m_sceneObject.getOctaObject())->setOctagon(m_currentOcta);
    m_octaCountObj = m_octaCount;
  }
}

void DebugIsoSurface::updateTetraObject() {
  if(m_param.m_tetrahedral) {
    if((m_flags & HAS_TETRA) == 0) {
      m_tetraCountObj = 0;
    } else if(m_tetraCount > m_tetraCountObj) {
      ((TetraObject*)m_sceneObject.getTetraObject())->setTetrahedron(m_currentTetra);
      m_tetraCountObj = m_tetraCount;
    }
  }
}

void DebugIsoSurface::updateFacesObject() {
  if((m_flags & HAS_FACE) == 0) {
    m_sceneObject.setFacesObject(NULL);
    m_visibleFaceCountObj = 0;
  } else if(m_visibleFaceCount > m_visibleFaceCountObj) {
    m_sceneObject.setFacesObject(createFacesObject());
    m_visibleFaceCountObj = m_visibleFaceCount;
  }
}

void DebugIsoSurface::updateVertexObject() {
  if((m_flags & HAS_VERTEX) == 0) {
    m_visibleVertexArraySizeObj = 0;
  } else if(m_visibleVertexArray.size() != m_visibleVertexArraySizeObj) {
    ((VertexObject*)m_sceneObject.getVertexObject())->setSurfaceVertexArray(m_visibleVertexArray);
    m_visibleVertexArraySizeObj = m_visibleVertexArray.size();
  }
}

static String flagsToString(BYTE flags) {
  String result;
  const TCHAR *delim = NULL;
#define ADDFLAG(f) if(flags & HAS_##f) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(OCTA);
  ADDFLAG(TETRA);
  ADDFLAG(FACE);
  ADDFLAG(VERTEX);
  return result;
}

String DebugIsoSurface::toString() const {
  if(m_param.m_tetrahedral) {
    return format(_T("%s\nflags:%s\nVertices:%s\n")
                 ,m_currentTetra.toString().cstr()
                 ,flagsToString(m_flags).cstr()
                 ,m_visibleVertexArray.toString(_T("\n")).cstr()
                 );
  } else {
    return format(_T("%s\nFlags:%s\nVertices:%s\n")
                 ,m_currentOcta.toString(3).cstr()
                 ,flagsToString(m_flags).cstr()
                 ,m_visibleVertexArray.toString(_T("\n")).cstr()
                 );
  }
}

#endif // DEBUG_POLYGONIZER
