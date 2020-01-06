#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include "Debugger.h"
#include "DebugIsoSurface.h"

class MaterialIndexWithColor {
private:
  mutable int m_matIndex;
  static int createMaterialWithColor(D3Scene &scene, D3DCOLOR color) {
    const D3DCOLORVALUE cv = colorToColorValue(color);
    D3DMATERIAL mat;
    memset(&mat, 0, sizeof(mat));
    mat.Diffuse = cv;
    mat.Emissive = cv;
    return scene.addMaterial(mat);
  }
public:
  MaterialIndexWithColor() : m_matIndex(-1) {
  }
  inline UINT getIndex(D3Scene &scene, D3DCOLOR color) const {
    if(m_matIndex < 0) {
      m_matIndex = createMaterialWithColor(scene, color);
    }
    return m_matIndex;
  }
};

static MaterialIndexWithColor blackMatIndex, whiteMatIndex, blueMatIndex, yellowMatIndex;

class CornerMarkObject : public SceneObjectSolidBox {
private:
  static D3DXCube3 createCube(const D3DXVECTOR3 &center, float sideLength) {
    return D3DXCube3(D3DXVECTOR3(center.x - sideLength / 2, center.y - sideLength / 2, center.z - sideLength / 2)
                    ,D3DXVECTOR3(center.x + sideLength / 2, center.y + sideLength / 2, center.z + sideLength / 2));
  }
public:
  CornerMarkObject(D3Scene &scene, const D3DXVECTOR3 &center, double sideLength, int materialIndex)
    : SceneObjectSolidBox(scene, createCube(center, (float)sideLength), materialIndex)
  {
  }
};

class TemporaryFace : public D3LineArray {
public:
  TemporaryFace(D3Scene &scene, const CompactArray<Line3D> &lineArray) : D3LineArray(scene, lineArray) {
  }
  void draw();
};

void TemporaryFace::draw() {
  getScene().selectMaterial(whiteMatIndex.getIndex(getScene(), D3D_WHITE));
  __super::draw();
}

static TemporaryFace *createTemporaryFace(D3Scene &scene, const CompactArray<const IsoSurfaceVertex*> &vertexArray) {
  if(vertexArray.size() < 2) {
    return NULL;
  }
  CompactArray<Line3D> lineArray;
  const IsoSurfaceVertex *c1 = vertexArray[0];
  for(size_t i = 1; i < vertexArray.size(); i++) {
    const IsoSurfaceVertex *c2 = vertexArray[i];
    lineArray.add(Line3D(Point3DP(c1->m_position), Point3DP(c2->m_position)));
    c1 = c2;
  }
  return new TemporaryFace(scene, lineArray);
}

// ------------------------------------------------------------Octa Object ----------------------------------------------

class OctaObject : public D3WireFrameBox {
private:
  static int s_matIndexPos, s_matIndexNeg;
  CompactArray<CornerMarkObject*> m_cornerMarkArray;
  int  createCornerMarkMaterial(bool positive);
  int  getCornerMarkMaterial(bool positive);
public:
  OctaObject(D3Scene &scene, const Octagon &octa, double cellSize);
  ~OctaObject();
  static CornerMarkObject *createCornerMarkObject(D3Scene &scene, const HashedCubeCorner &c, double sideLengh, int materialIndex);
  void draw();
};

int OctaObject::s_matIndexPos           = -1;
int OctaObject::s_matIndexNeg           = -1;

OctaObject::OctaObject(D3Scene &scene, const Octagon &octa, double cellSize)
  : D3WireFrameBox(scene, *octa.getCube().m_corners[LBN], *octa.getCube().m_corners[RTF])
{
  const double sideLength = cellSize/ 10 / (1 << octa.getLevel());
  for(UINT i = 0; i < octa.getCornerCount(); i++) {
    const HashedCubeCorner &hc = octa.getHashedCorner(i);
    const int               matIndex = getCornerMarkMaterial(hc.m_positive);
    m_cornerMarkArray.add(createCornerMarkObject(scene, hc, sideLength, matIndex));
  }
}

OctaObject::~OctaObject() {
  for(size_t i = 0; i < m_cornerMarkArray.size(); i++) {
    SAFEDELETE(m_cornerMarkArray[i]);
  }
  m_cornerMarkArray.clear();
}

CornerMarkObject *OctaObject::createCornerMarkObject(D3Scene &scene, const HashedCubeCorner &c, double sideLength, int materialIndex) { // static
  CornerMarkObject *mark = new CornerMarkObject(scene, Point3DP(c), sideLength, materialIndex); TRACE_NEW(mark);
  return mark;
}

int OctaObject::getCornerMarkMaterial(bool positive) {
  int &result = positive ? s_matIndexPos : s_matIndexNeg;
  if(result < 0) {
    result = createCornerMarkMaterial(positive);
  }
  return result;
}

#define CUBE_MATERIAL_DIFFUSE_POS D3DCOLOR_XRGB(33,150,28)
#define CUBE_MATERIAL_DIFFUSE_NEG D3DCOLOR_XRGB(120,25,30)

int OctaObject::createCornerMarkMaterial(bool positive) {
  MATERIAL mat = getScene().getMaterial(0);
  mat.Diffuse  = colorToColorValue(positive ? CUBE_MATERIAL_DIFFUSE_POS : CUBE_MATERIAL_DIFFUSE_NEG);
  mat.Specular = colorToColorValue(D3D_WHITE);
  mat.Ambient  = colorToColorValue(D3D_BLACK);
  mat.Emissive = mat.Ambient;
  mat.Power    = 200.0f;
  return getScene().addMaterial(mat);
}

void OctaObject::draw() {
  getScene().selectMaterial(blueMatIndex.getIndex(getScene(), D3D_BLUE));
  __super::draw();
  for(size_t i = 0; i < m_cornerMarkArray.size(); i++) {
    m_cornerMarkArray[i]->draw();
  }
}

// ----------------------------------------------------Tetra Object ----------------------------------------------

class TetraObject : public D3LineArray {
private:
  static int s_matIndexPos, s_matIndexNeg;
  CompactArray<CornerMarkObject*> m_cornerMarkArray;
  static CompactArray<Line3D> createLineArray(const Tetrahedron &tetra);
  int                         createCornerMarkMaterial(bool positive);
  int                         getCornerMarkMaterial(   bool positive);
public:
  TetraObject(D3Scene &scene, const Tetrahedron &tetra, double cellSize);
  ~TetraObject();
  void draw();
};

int TetraObject::s_matIndexPos             = -1;
int TetraObject::s_matIndexNeg             = -1;

TetraObject::TetraObject(D3Scene &scene, const Tetrahedron &tetra, double cellSize)
  : D3LineArray(scene, createLineArray(tetra))
{
  const StackedCube &cube       = tetra.getCube();
  const double       sideLength = cellSize / 10 / (1 << cube.getLevel());
  for(UINT i = 0; i < tetra.getCornerCount(); i++) {
    const HashedCubeCorner &hc       = tetra.getHashedCorner(i);
    const int               matIndex = getCornerMarkMaterial(hc.m_positive);
    m_cornerMarkArray.add(OctaObject::createCornerMarkObject(scene, hc, sideLength, matIndex));
  }
}

CompactArray<Line3D> TetraObject::createLineArray(const Tetrahedron &tetra) { // static
  CompactArray<Line3D> result(6);
  const UINT n = tetra.getCornerCount();
  for(UINT i = 0; i < n; i++) {
    for(UINT j = i+1; j < n; j++) {
      result.add(Line3D(tetra.getHashedCorner(i), tetra.getHashedCorner(j)));
    }
  }
  return result;
}

TetraObject::~TetraObject() {
  for(size_t i = 0; i < m_cornerMarkArray.size(); i++) {
    SAFEDELETE(m_cornerMarkArray[i]);
  }
  m_cornerMarkArray.clear();
}

#define TETRA_MATERIAL_DIFFUSE_POS CUBE_MATERIAL_DIFFUSE_POS // D3DCOLOR_XRGB(66,220,56)
#define TETRA_MATERIAL_DIFFUSE_NEG CUBE_MATERIAL_DIFFUSE_NEG // D3DCOLOR_XRGB(236,50,50)

int TetraObject::createCornerMarkMaterial(bool positive) {
  MATERIAL mat = getScene().getMaterial(0);
  mat.Diffuse = colorToColorValue(positive ? TETRA_MATERIAL_DIFFUSE_POS : TETRA_MATERIAL_DIFFUSE_NEG);
  mat.Specular = colorToColorValue(D3D_WHITE);
  mat.Ambient  = colorToColorValue(D3D_BLACK);
  mat.Emissive = mat.Ambient;
  mat.Power = 200.0f;
  return getScene().addMaterial(mat);
}

int TetraObject::getCornerMarkMaterial(bool positive) {
  int &result = positive ? s_matIndexPos : s_matIndexNeg;
  if(result < 0) {
    result = createCornerMarkMaterial(positive);
  }
  return result;
}

void TetraObject::draw() {
  getScene().selectMaterial(yellowMatIndex.getIndex(getScene(), D3D_YELLOW));
  __super::draw();
  for(size_t i = 0; i < m_cornerMarkArray.size(); i++) {
    m_cornerMarkArray[i]->draw();
  }
}

// ------------------------------------------------------------Face Object ----------------------------------------------

class FaceObject : public D3LineArray {
private:
  static CompactArray<Line3D> createLineArray(const Face3 &face, const Array<IsoSurfaceVertex> &va);
public:
  FaceObject(D3Scene &scene, const Face3 &face, const Array<IsoSurfaceVertex> &va)
    : D3LineArray(scene, createLineArray(face, va))
  {
  }
  void draw();
};

CompactArray<Line3D> FaceObject::createLineArray(const Face3 &face, const Array<IsoSurfaceVertex> &va) { // static
  CompactArray<Line3D> result;
  result.add(Line3D(va[face.m_i1].m_position, va[face.m_i2].m_position));
  result.add(Line3D(va[face.m_i2].m_position, va[face.m_i3].m_position));
  result.add(Line3D(va[face.m_i3].m_position, va[face.m_i1].m_position));

  result.add(Line3D(va[face.m_i1].m_position, va[face.m_i3].m_position));
  result.add(Line3D(va[face.m_i3].m_position, va[face.m_i2].m_position));
  result.add(Line3D(va[face.m_i2].m_position, va[face.m_i1].m_position));
  return result;
}

void FaceObject::draw() {
  getScene().selectMaterial(blackMatIndex.getIndex(getScene(), D3D_BLACK));
  __super::draw();
}

// ------------------------------------------------------------Vertex Object ----------------------------------------------

class VertexObject : public D3LineArray {
  static CompactArray<Line3D> createLineArray(const IsoSurfaceVertex &v, float length);
public:
  VertexObject(D3Scene &scene, const IsoSurfaceVertex &v, float length);
  void draw();
};

VertexObject::VertexObject(D3Scene &scene, const IsoSurfaceVertex &v, float length) : D3LineArray(scene, createLineArray(v, length)) {
}

CompactArray<Line3D> VertexObject::createLineArray(const IsoSurfaceVertex &v, float length) { // static
  CompactArray<Line3D> result;
  const D3DXVECTOR3 p0 = Point3DP(v.m_position);
  float d = length;
  const D3DXVECTOR3 v1( d, d, d);
  const D3DXVECTOR3 v2( d, d,-d);
  const D3DXVECTOR3 v3( d,-d, d);
  const D3DXVECTOR3 v4(-d, d, d);
  result.add(Line3D(p0 - v1, p0 + v1));
  result.add(Line3D(p0 - v2, p0 + v2));
  result.add(Line3D(p0 - v3, p0 + v3));
  result.add(Line3D(p0 - v4, p0 + v4));
  return result;
}

void VertexObject::draw() {
  getScene().selectMaterial(whiteMatIndex.getIndex(getScene(), D3D_WHITE));
  __super::draw();
}

// ------------------------------------------------------------VertexArray Object ----------------------------------------------

class VertexArrayObject : public D3LineArray {
  static CompactArray<Line3D> createLineArray(CompactArray<const IsoSurfaceVertex*> &va, float length);
public:
  VertexArrayObject(D3Scene &scene, CompactArray<const IsoSurfaceVertex*> &va, float length)
    : D3LineArray(scene, createLineArray(va, length))
  {
  }
  void draw();
};

CompactArray<Line3D> VertexArrayObject::createLineArray(CompactArray<const IsoSurfaceVertex*> &va, float length) {
  CompactArray<Line3D> result;
  const size_t n = va.size();
  const float  d = length;
  const D3DXVECTOR3 v1(d, d, d), v2(d, d, -d), v3(d, -d, d), v4(-d, d, d);
  for(size_t i = 0; i < n; i++) {
    const D3DXVECTOR3 p0 = Point3DP(va[i]->m_position);
    result.add(Line3D(p0 - v1, p0 + v1));
    result.add(Line3D(p0 - v2, p0 + v2));
    result.add(Line3D(p0 - v3, p0 + v3));
    result.add(Line3D(p0 - v4, p0 + v4));
  }
  return result;
}

void VertexArrayObject::draw() {
  getScene().selectMaterial(whiteMatIndex.getIndex(getScene(), D3D_WHITE));
  __super::draw();
}

// --------------------------------------- DebugIsoSurface -------------------------------------------

DebugIsoSurface::DebugIsoSurface(Debugger *debugger, D3SceneContainer &sc, const IsoSurfaceParameters &param)
  : m_debugger(*debugger)
  , m_sc(sc)
  , m_param(param)
  , m_polygonizer(NULL)
  , m_vertexArray(NULL)
  , m_exprWrapper(param.m_expr, param.m_machineCode)
  , m_reverseSign(false)
  , m_lastVertexCount(          0)
  , m_faceCount(                0)
  , m_lastFaceCount(            0)
  , m_currentLevel(             0)
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
  m_currentFace.reset();
  m_currentVertex.reset();
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
  m_vertexArray = &m_polygonizer->getVertexArray();

  m_polygonizer->polygonize(Point3D(0, 0, 0)
                           ,m_param.m_cellSize
                           ,m_param.m_boundingBox
                           ,m_param.m_tetrahedral
                           ,m_param.m_tetraOptimize4
                           ,m_param.m_adaptiveCellSize
                           );
  if(m_faceCount == 0) {
    throwException(_T("No polygons generated. Cannot create object"));
  }
  m_statistics = m_polygonizer->getStatistics();
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
  updateFaceObject();
  updateVisibleVertexArrayObject();
  m_sceneObject.setVisibleParts(visibleParts);
}

void DebugIsoSurface::markCurrentOcta(const Octagon &octa) {
  m_octaCount++;
  m_currentOcta = octa;
  m_flags = HAS_OCTA;
  clearVisibleVertexArray();
  if(octa.getLevel() == m_currentLevel) {
    m_debugger.handleStep(NEW_OCTA);
  } else {
    m_currentLevel = octa.getLevel();
    m_debugger.handleStep(NEW_LEVEL);
  }
}

void DebugIsoSurface::markCurrentTetra(const Tetrahedron &tetra) {
  m_tetraCount++;
  m_currentTetra = tetra;
  setFlags(HAS_TETRA).clrFlags(HAS_FACE | HAS_VERTEX);
  clearVisibleVertexArray();
  m_debugger.handleStep(NEW_TETRA);
}

void DebugIsoSurface::markCurrentFace(const Face3 &f) {
  m_visibleFaceCount++;
  m_currentFace   = f;
  setFlags(HAS_FACE).clrFlags(HAS_VERTEX);
  m_debugger.handleStep(NEW_FACE);
}

void DebugIsoSurface::markCurrentVertex(const IsoSurfaceVertex *v) {
  m_vertexCount++;
  setFlags(HAS_VERTEX);
  m_currentVertex = *v;
  m_visibleVertexArray.add(v);
  m_debugger.handleStep(NEW_VERTEX);
}

SceneObjectWithMesh *DebugIsoSurface::createMeshObject() const {
  D3Scene             &scene = m_sc.getScene();
  SceneObjectWithMesh *obj = new SceneObjectWithMesh(scene, m_mb.createMesh(scene, m_param.m_doubleSided)); TRACE_NEW(m_sceneObject);
  obj->setFillMode(m_sceneObject.getFillMode());
  obj->setShadeMode(m_sceneObject.getShadeMode());
  return obj;
}

D3SceneObject *DebugIsoSurface::createOctaObject() {
  D3SceneObject *octa = new OctaObject(m_sc.getScene(), m_currentOcta, m_param.m_cellSize); TRACE_NEW(octa);
  return octa;
}

D3SceneObject *DebugIsoSurface::createTetraObject() {
  D3SceneObject *tetra = new TetraObject(m_sc.getScene(), m_currentTetra, m_param.m_cellSize); TRACE_NEW(tetra);
  return tetra;
}

D3SceneObject *DebugIsoSurface::createFaceObject() {
  D3SceneObject *v = new FaceObject(m_sc.getScene(), m_currentFace, *m_vertexArray); TRACE_NEW(v);
  return v;
}

D3SceneObject *DebugIsoSurface::createVertexObject() {
  D3SceneObject *v = new VertexObject(m_sc.getScene(), m_currentVertex, (float)(m_param.m_cellSize/10)); TRACE_NEW(v);
  return v;
}

D3SceneObject *DebugIsoSurface::createVisibleVertexArrayObject() {
  D3SceneObject *v = new VertexArrayObject(m_sc.getScene(), m_visibleVertexArray, (float)(m_param.m_cellSize / 10)); TRACE_NEW(v);
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
    m_sceneObject.setOctaObject(NULL);
    m_octaCountObj = 0;
  } else if(m_octaCount > m_octaCountObj) {
    m_sceneObject.setOctaObject(createOctaObject());
    m_octaCountObj = m_octaCount;
  }
}

void DebugIsoSurface::updateTetraObject() {
  if(m_param.m_tetrahedral) {
    if((m_flags & HAS_TETRA) == 0) {
      m_sceneObject.setTetraObject(NULL);
      m_tetraCountObj = 0;
    } else if(m_tetraCount > m_tetraCountObj) {
      m_sceneObject.setTetraObject(createTetraObject());
      m_tetraCountObj = m_tetraCount;
    }
  }
}

void DebugIsoSurface::updateFaceObject() {
  if((m_flags & HAS_FACE) == 0) {
    m_sceneObject.setFaceObject(NULL);
    m_visibleFaceCountObj = 0;
  } else if(m_visibleFaceCount > m_visibleFaceCountObj) {
    m_sceneObject.setFaceObject(createFaceObject());
    m_visibleFaceCountObj = m_visibleFaceCount;
  }
}

void DebugIsoSurface::updateVertexObject() {
  if((m_flags & HAS_VERTEX) == 0) {
    m_sceneObject.setVertexObject(NULL);
    m_vertexCountObj = 0;
  } else if(m_vertexCount > m_vertexCountObj) {
    m_sceneObject.setVertexObject(createVertexObject());
    m_vertexCountObj = m_vertexCount;
  }
}

void DebugIsoSurface::updateVisibleVertexArrayObject() {
  if((m_flags & HAS_VERTEX) == 0) {
    m_sceneObject.setVisibleVertexArrayObject(NULL);
    m_visibleVertexArraySizeObj = 0;
  } else if(m_visibleVertexArray.size() != m_visibleVertexArraySizeObj) {
    m_sceneObject.setVisibleVertexArrayObject(createVisibleVertexArrayObject());
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
    return format(_T("%s\nflags:%s\n%s\n")
                 ,m_currentTetra.toString().cstr()
                 ,flagsToString(m_flags).cstr()
                 ,m_currentVertex.toString().cstr()
                 );
  } else {
    return format(_T("%s\nFlags:%s\n%s\n")
                 ,m_currentOcta.toString().cstr()
                 ,flagsToString(m_flags).cstr()
                 ,m_currentVertex.toString().cstr()
                 );
  }
}

#endif // DEBUG_POLYGONIZER
