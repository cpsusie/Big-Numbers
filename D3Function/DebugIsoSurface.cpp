#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include "Debugger.h"
#include "DebugIsoSurface.h"

class ColorMaterialId {
private:
  mutable int  m_materialId;
  const   bool m_specular;
  static int createColorMaterial(D3Scene &scene, D3DCOLOR color) {
    const D3DCOLORVALUE cv = colorToColorValue(color);
    D3DMATERIAL mat;
    memset(&mat, 0, sizeof(mat));
    mat.Diffuse  = cv;
    mat.Emissive = cv;
    return scene.addMaterial(mat);
  }
  static int createColorMaterialWithSpecular(D3Scene &scene, D3DCOLOR color) {
    MATERIAL mat = scene.getMaterial(0);
    mat.Diffuse  = colorToColorValue(color);
    mat.Specular = colorToColorValue(D3D_WHITE);
    mat.Ambient  = colorToColorValue(D3D_BLACK);
    mat.Emissive = mat.Ambient;
    mat.Power    = 200.0f;
    return scene.addMaterial(mat);
  }

public:
  ColorMaterialId(bool specular=false) : m_materialId(-1), m_specular(specular) {
  }
  inline UINT getId(D3Scene &scene, D3DCOLOR color) const {
    if(m_materialId < 0) {
      m_materialId = m_specular
                   ? createColorMaterialWithSpecular(scene, color)
                   : createColorMaterial(scene, color);
    }
    return m_materialId;
  }
};

static ColorMaterialId blackMatIndex, whiteMatIndex, blueMatIndex, yellowMatIndex, cyanMatIndex;
static ColorMaterialId matIndexPos(true), matIndexNeg(true);

// ------------------------------------------------------------Octa Object ----------------------------------------------

class OctaObject : public D3SceneObject {
private:
  D3SceneObjectWireFrameBox *m_wireFrameBox;
  D3SceneObjectSolidBox     *m_cornerMark[2];
  D3DXVECTOR3                m_cornerCenters[8]; // relative to m_center
  const float                m_cellSize;
  D3DXVECTOR3                m_center;
  BitSet8                    m_positive;
  LPDIRECT3DDEVICE           m_device;
  D3DXMATRIX                 m_world;
  int  getCornerMarkMaterial(bool positive);
public:
  OctaObject(D3Scene &scene, float cellSize);
  ~OctaObject();
  void setOctagon(const Octagon &octa);
  inline const D3DXVECTOR3 *getCornerCenters() const {
    return m_cornerCenters;
  }
  inline float getCellSize() const {
    return m_cellSize;
  }
  inline const D3DXVECTOR3 &getCenter() const {
    return m_center;
  }
  inline const D3DXMATRIX &getWorld() const {
    return m_world;
  }
  inline LPDIRECT3DDEVICE &getDevice() {
    return m_device;
  }
  void draw();
};

OctaObject::OctaObject(D3Scene &scene, float cellSize)
  : D3SceneObject(scene)
  , m_cellSize(cellSize)
  , m_device(scene.getDevice())
{
  float t = m_cellSize / 2;
  Vertex lbn(-t, -t, -t), rtf(t,t,t);
  m_wireFrameBox = new D3SceneObjectWireFrameBox(getScene(), lbn, rtf); TRACE_NEW(m_wireFrameBox);

  m_cornerCenters[LBN] = D3DXVECTOR3(lbn.x, lbn.y, lbn.z); // left  bottom near corner
  m_cornerCenters[LBF] = D3DXVECTOR3(lbn.x, lbn.y, rtf.z); // left  bottom far  corner
  m_cornerCenters[LTN] = D3DXVECTOR3(lbn.x, rtf.y, lbn.z); // left  top    near corner
  m_cornerCenters[LTF] = D3DXVECTOR3(lbn.x, rtf.y, rtf.z); // left  top    far  corner
  m_cornerCenters[RBN] = D3DXVECTOR3(rtf.x, lbn.y, lbn.z); // right bottom near corner
  m_cornerCenters[RBF] = D3DXVECTOR3(rtf.x, lbn.y, rtf.z); // right bottom far  corner
  m_cornerCenters[RTN] = D3DXVECTOR3(rtf.x, rtf.y, lbn.z); // right top    near corner
  m_cornerCenters[RTF] = D3DXVECTOR3(rtf.x, rtf.y, rtf.z); // right top    far  corner

  t   = m_cellSize / 20;
  lbn = Vertex(-t, -t, -t); rtf = Vertex( t,  t,  t);
  m_cornerMark[0] = new D3SceneObjectSolidBox(getScene(), lbn, rtf, getCornerMarkMaterial(false)); TRACE_NEW(m_cornerMark[0]);
  m_cornerMark[1] = new D3SceneObjectSolidBox(getScene(), lbn, rtf, getCornerMarkMaterial(true )); TRACE_NEW(m_cornerMark[1]);
}

OctaObject::~OctaObject() {
  SAFEDELETE(m_cornerMark[0]);
  SAFEDELETE(m_cornerMark[1]);
  SAFEDELETE(m_wireFrameBox);
}

void OctaObject::setOctagon(const Octagon &octa) {
  m_center = octa.getCube().getCenter();
  m_positive.clear();
  const UINT n = octa.getCornerCount();
  for(UINT i = 0; i < n; i++) {
    if(octa.getHashedCorner(i).m_positive) {
      m_positive.add(i);
    }
  }
}

#define CUBE_MATERIAL_DIFFUSE_POS D3DCOLOR_XRGB(33,150,28)
#define CUBE_MATERIAL_DIFFUSE_NEG D3DCOLOR_XRGB(120,25,30)

int OctaObject::getCornerMarkMaterial(bool positive) {
  int result = positive 
             ? matIndexPos.getId(getScene(), CUBE_MATERIAL_DIFFUSE_POS)
             : matIndexNeg.getId(getScene(), CUBE_MATERIAL_DIFFUSE_NEG);
  return result;
}

void OctaObject::draw() {
  D3Scene &s = getScene();
  D3DXMATRIX objWorld = getPDUS().getWorldMatrix();
  D3DXMATRIX tm;
  D3DXMatrixTranslation(&tm, m_center.x, m_center.y, m_center.z);
  m_world = objWorld * tm;
  s.selectMaterial(blueMatIndex.getId(getScene(), D3D_BLUE)).setDevWorldMatrix(m_world);
  m_wireFrameBox->draw();
  for(UINT i = 0; i < ARRAYSIZE(m_cornerCenters); i++) {
    const D3DXVECTOR3 &cc = m_cornerCenters[i];
    D3DXMatrixTranslation(&tm, cc.x, cc.y, cc.z);
    const D3DXMATRIX cornerWorld = m_world * tm;
    s.setDevWorldMatrix(cornerWorld);
    const int markIndex = m_positive.contains(i) ? 1 : 0;
    m_cornerMark[markIndex]->draw();
  }
}

// ----------------------------------------------------Tetra Object ----------------------------------------------

class TetraObject : public D3SceneObject {
private:
  OctaObject                                                  &m_octaObject;
  mutable CompactHashMap<Tetrahedron, D3SceneObjectLineArray*> m_objectCache;
  D3SceneObjectLineArray                                      *m_linesObject;
  LPDIRECT3DDEVICE                                             m_device;
  CompactArray<Line3D>    createLineArray(const Tetrahedron &tetra) const;
  D3SceneObjectLineArray &getLinesObject( const Tetrahedron &tetra) const;
  const D3DXMATRIX       &getWorld() const {
    return m_octaObject.getWorld();
  }
public:
  TetraObject(OctaObject *octaObject);
  ~TetraObject();
  void setTetrahedron(const Tetrahedron &tetra);
  void draw();
};

TetraObject::TetraObject(OctaObject *octaObject)
  : D3SceneObject(octaObject->getScene())
  , m_octaObject(*octaObject)
  , m_linesObject(NULL)
{
  m_device = m_octaObject.getDevice();
}

TetraObject::~TetraObject() {
  for(Iterator<Entry<Tetrahedron, D3SceneObjectLineArray*> > it = m_objectCache.getEntryIterator(); it.hasNext();) {
    D3SceneObjectLineArray *obj = it.next().getValue();
    SAFEDELETE(obj);
  }
  m_objectCache.clear();
}

void TetraObject::setTetrahedron(const Tetrahedron &tetra) {
  m_linesObject = &getLinesObject(tetra);
}

D3SceneObjectLineArray &TetraObject::getLinesObject(const Tetrahedron &tetra) const {
  D3SceneObjectLineArray **a = m_objectCache.get(tetra);
  if(a == NULL) {
    D3SceneObjectLineArray *laObj = new D3SceneObjectLineArray(getScene(), createLineArray(tetra)); TRACE_NEW(laObj);
    m_objectCache.put(tetra, laObj);
  }
  return **m_objectCache.get(tetra);
}

CompactArray<Line3D> TetraObject::createLineArray(const Tetrahedron &tetra) const {
  const D3DXVECTOR3 *corners = m_octaObject.getCornerCenters();
  CompactArray<Line3D> result(6);
  const UINT n = tetra.getCornerCount();
  for(UINT i = 0; i < n; i++) {
    for(UINT j = i+1; j < n; j++) {
      result.add(Line3D(corners[tetra.getCorner(i)], corners[tetra.getCorner(j)]));
    }
  }
  return result;
}

void TetraObject::draw() {
  if(m_linesObject == NULL) return;
  getScene().selectMaterial(yellowMatIndex.getId(getScene(), D3D_YELLOW)).setDevWorldMatrix(getWorld());
  m_linesObject->draw();
}

// ------------------------------------------------------------Face Object ----------------------------------------------

class FacesObject : public D3SceneObject {
private:
  D3SceneObjectLineArray *m_currentFaceObject, *m_prevFaceObjects;
  LPDIRECT3DDEVICE        m_device;
  CompactArray<Line3D> createLineArray(const Face3 &face,              const IsoSurfaceVertexArray &va) const;
  CompactArray<Line3D> createLineArray(CompactArray<Face3> &faceArray, const IsoSurfaceVertexArray &va) const;
public:
  FacesObject(DebugIsoSurface *dbgObject, CompactArray<Face3> &faceArray);
  ~FacesObject();
  void draw();
};

FacesObject::FacesObject(DebugIsoSurface *dbgObject, CompactArray<Face3> &faceArray)
: D3SceneObject(dbgObject->getScene())
, m_currentFaceObject(NULL)
, m_prevFaceObjects( NULL)
{
  m_device = getScene().getDevice();
  if(faceArray.isEmpty()) return;
  const IsoSurfaceVertexArray &va = dbgObject->getVertexArray();
  m_currentFaceObject = new D3SceneObjectLineArray(getScene(), createLineArray(faceArray.last(), va)); TRACE_NEW(m_currentFaceObject);
  if(faceArray.size() > 1) {
    m_prevFaceObjects = new D3SceneObjectLineArray(getScene(), createLineArray(faceArray, va)); TRACE_NEW(m_prevFaceObjects);
  }
}

FacesObject::~FacesObject() {
  SAFEDELETE(m_currentFaceObject);
  SAFEDELETE(m_prevFaceObjects  );
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

void FacesObject::draw() {
  D3Scene &s = getScene();
  s.setDevWorldMatrix(getPDUS().getWorldMatrix());
  if(m_prevFaceObjects) {
    s.selectMaterial(blackMatIndex.getId(s, D3D_BLACK));
    m_prevFaceObjects->draw();
  }
  if(m_currentFaceObject) {
    s.selectMaterial(cyanMatIndex.getId(s, D3D_CYAN));
    m_currentFaceObject->draw();
  }
}

// ------------------------------------------------------------Vertex Object ----------------------------------------------

class VertexObject : public D3SceneObject {
private:
  D3SceneObjectLineArray   *m_linesObject;
  CompactArray<D3DXVECTOR3> m_positions;
  LPDIRECT3DDEVICE          m_device;
  CompactArray<Line3D> createLineArray(float lineLength) const;
public:
  VertexObject(OctaObject *octaObject);
  ~VertexObject();
  void setSurfaceVertexArray(const IsoSurfaceVertexArray &a);
  void draw();
};

VertexObject::VertexObject(OctaObject *octaObject) : D3SceneObject(octaObject->getScene()) {
  m_device = getScene().getDevice();
  m_linesObject = new D3SceneObjectLineArray(getScene(), createLineArray(octaObject->getCellSize() / 25));
}

VertexObject::~VertexObject() {
  SAFEDELETE(m_linesObject)
}

void VertexObject::setSurfaceVertexArray(const IsoSurfaceVertexArray &a) {
  m_positions.clear(-1);
  for(size_t i = 0; i < a.size(); i++) {
    m_positions.add(a[i].m_position);
  }
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
  const size_t n = m_positions.size();
  if(n == 0) return;
  D3Scene &s = getScene();
  s.selectMaterial(whiteMatIndex.getId(getScene(), D3D_WHITE));
  D3DXMATRIX objWorld = getPDUS().getWorldMatrix(), tm;
  for(size_t i = 0; i < n; i++) {
    const D3DXVECTOR3 &p = m_positions[i];
    D3DXMatrixTranslation(&tm, p.x, p.y, p.z);
    s.setDevWorldMatrix(objWorld * tm);
    m_linesObject->draw();
  }
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
  OctaObject   *octaObject   = new OctaObject(sc.getScene(), (float)param.m_cellSize);
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
  m_visibleVertexArray.add(v);
  m_debugger.handleStep(NEW_VERTEX);
}

D3SceneObjectWithMesh *DebugIsoSurface::createMeshObject() const {
  D3Scene             &scene = m_sc.getScene();
  D3SceneObjectWithMesh *obj = new D3SceneObjectWithMesh(scene, m_mb.createMesh(scene, m_param.m_doubleSided)); TRACE_NEW(m_sceneObject);
  obj->setFillMode(m_sceneObject.getFillMode());
  obj->setShadeMode(m_sceneObject.getShadeMode());
  return obj;
}

D3SceneObject *DebugIsoSurface::createFacesObject() {
  D3SceneObject *v = new FacesObject(this, m_currentFaceArray); TRACE_NEW(v);
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
                 ,m_visibleVertexArray.toString().cstr()
                 );
  } else {
    return format(_T("%s\nFlags:%s\nVertices:%s\n")
                 ,m_currentOcta.toString().cstr()
                 ,flagsToString(m_flags).cstr()
                 ,m_visibleVertexArray.toString().cstr()
                 );
  }
}

#endif // DEBUG_POLYGONIZER
