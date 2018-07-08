#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

// ------------------------------------------------ D3SceneObject ---------------------------------------------------

bool D3SceneObject::intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info) const {
  LPD3DXMESH mesh = getMesh();
  if(mesh == NULL) return false;

  const D3DXMATRIX  m      = invers(getWorldMatrix());
  const D3DXVECTOR3 rayPos = m * ray.m_orig;
  const D3DXVECTOR3 rayDir = ray.m_dir * m;

  BOOL         hit;
  DWORD        faceIndex;
  float        pu,pv;
  V(D3DXIntersect(mesh
                 ,&rayPos
                 ,&rayDir
                 ,&hit
                 ,&faceIndex
                 ,&pu,&pv
                 ,&dist
                 ,NULL // when these parameters are NULL, only the closest intersection (if any) will be recognized (dist)
                 ,NULL
                 ));

  if(hit && info != NULL) {
    void *indexItems;
    LPDIRECT3DINDEXBUFFER indexBuffer;
    V(mesh->GetIndexBuffer(&indexBuffer));
    D3DINDEXBUFFER_DESC desc;
    V(indexBuffer->GetDesc(&desc));
    const bool use32Bit = desc.Format == D3DFMT_INDEX32;
    V(indexBuffer->Lock(0,0,&indexItems, D3DLOCK_READONLY));

    const int vertex0Index = faceIndex * 3;
    int i0,i1,i2;
    if(use32Bit) {
      const ULONG *ip = (ULONG*)indexItems + vertex0Index;
      i0 = *(ip++);
      i1 = *(ip++);
      i2 = *(ip++);
    } else {
      const USHORT *ip = (USHORT*)indexItems + vertex0Index;
      i0 = *(ip++);
      i1 = *(ip++);
      i2 = *(ip++);
    }
    V(indexBuffer->Unlock());
    *info = D3PickedInfo(faceIndex, i0,i1,i2,pu,pv,(ray.m_orig + dist * ray.m_dir - getPos()) * m);
  }
  return hit ? true : false;
}

// ------------------------------------------------ SceneObjectWithVertexBuffer ---------------------------------------------------

SceneObjectWithVertexBuffer::SceneObjectWithVertexBuffer(D3Scene &scene) : D3SceneObject(scene) {
  m_vertexBuffer   = NULL;
  m_primitiveCount = 0;
}

SceneObjectWithVertexBuffer::~SceneObjectWithVertexBuffer() {
  releaseVertexBuffer();
}

void *SceneObjectWithVertexBuffer::allocateVertexBuffer(int vertexSize, UINT count, DWORD fvf) {
  releaseVertexBuffer();
  UINT bufferSize;
  m_vertexBuffer = getScene().allocateVertexBuffer(fvf, count, &bufferSize);
  m_vertexSize   = vertexSize;
  m_fvf          = fvf;
  assert(bufferSize == vertexSize * count);
  void *bufferItems = NULL;
  V(m_vertexBuffer->Lock(0, bufferSize, &bufferItems, 0));
  return bufferItems;
}

void SceneObjectWithVertexBuffer::unlockVertexBuffer() {
  V(m_vertexBuffer->Unlock());
}

void SceneObjectWithVertexBuffer::releaseVertexBuffer() {
  SAFERELEASE(m_vertexBuffer);
}

#define GETLOCKEDVERTEXBUFFER(type, count) (type*)allocateVertexBuffer(sizeof(type), count, type::FVF_Flags)

String SceneObjectWithVertexBuffer::toString() const {
  return format(_T("%s\nVertexBuffer:\n%s")
                ,__super::toString().cstr()
                ,indentString(::toString(m_vertexBuffer),2).cstr()
               );
}

// ------------------------------------------------ SceneObjectWithIndexBuffer ---------------------------------------------------

SceneObjectWithIndexBuffer::SceneObjectWithIndexBuffer(D3Scene &scene) : SceneObjectWithVertexBuffer(scene) {
  m_indexBuffer = NULL;
}

SceneObjectWithIndexBuffer::~SceneObjectWithIndexBuffer() {
  releaseIndexBuffer();
}

void *SceneObjectWithIndexBuffer::allocateIndexBuffer(bool int32, int count) {
  releaseIndexBuffer();
  void *bufferItems = NULL;
  m_indexBuffer = getScene().allocateIndexBuffer(int32, count);
  V(m_indexBuffer->Lock(0,0,&bufferItems, 0));
  return bufferItems;
}

void SceneObjectWithIndexBuffer::unlockIndexBuffer() {
  V(m_indexBuffer->Unlock());
}

void SceneObjectWithIndexBuffer::releaseIndexBuffer() {
  SAFERELEASE(m_indexBuffer)
}

#define GETLOCKEDSHORTBUFFER(count) (USHORT*)allocateIndexBuffer(false, count)
#define GETLOCKEDLONGBUFFER( count) (ULONG* )allocateIndexBuffer(true , count)

String SceneObjectWithIndexBuffer::toString() const {
  return format(_T("%s\nIndexBuffer:\n%s")
                ,__super::toString().cstr()
                ,indentString(::toString(m_indexBuffer),2).cstr());
}

// ----------------------------------- SceneObjectWithMesh --------------------------------------------

SceneObjectWithMesh::SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh)
: D3SceneObject(scene)
, m_fillMode(D3DFILL_SOLID)
, m_shadeMode(D3DSHADE_GOURAUD)
{
  m_mesh = mesh;
}

SceneObjectWithMesh::~SceneObjectWithMesh() {
  releaseMesh();
}

void SceneObjectWithMesh::createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf) {
  releaseMesh();
  m_mesh = getScene().allocateMesh(fvf, faceCount, vertexCount, D3DXMESH_SYSTEMMEM);
}

void SceneObjectWithMesh::releaseMesh() {
  SAFERELEASE(m_mesh);
}

void *SceneObjectWithMesh::lockVertexBuffer() {
  void *vertices;
  V(m_mesh->LockVertexBuffer(0, &vertices));
  return vertices;
}

void *SceneObjectWithMesh::lockIndexBuffer() {
  void *items;
  V(m_mesh->LockIndexBuffer(0, &items));
  return items;
}

void SceneObjectWithMesh::unlockVertexBuffer() {
  V(m_mesh->UnlockVertexBuffer());
}

void SceneObjectWithMesh::unlockIndexBuffer() {
  V(m_mesh->UnlockIndexBuffer());
}

void SceneObjectWithMesh::draw() {
  setFillAndShadeMode();
  setSceneMaterial();
  setLightingEnable(true);
  drawSubset(0);
}

String SceneObjectWithMesh::toString() const {
  return format(_T("%s\nMesh:\n%s")
                ,getName().cstr()
                ,indentString(::toString(getMesh()),2).cstr());
}

// ------------------------------------------------ D3LineArray -----------------------------------------------------------

D3LineArray::D3LineArray(D3Scene &scene, const Line3D *lines, int n) : SceneObjectWithVertexBuffer(scene) {
  initBuffer(lines, n);
}

void D3LineArray::initBuffer(const Line3D *lines, int n) {
  Vertex *vertices = GETLOCKEDVERTEXBUFFER(Vertex, 2*n);
  m_primitiveCount = n;
  memcpy(vertices, lines, sizeof(Line3D)*n);
  unlockVertexBuffer();
}

void D3LineArray::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    drawPrimitive(D3DPT_LINELIST, 0, m_primitiveCount);
  }
}

void D3WireFrameBox::init(const Vertex &p1, const Vertex &p2) {
  Line3D lines[12], *lp = lines, *ll;
  lp->m_p1 = p1;                           lp->m_p2 = Vertex(p1.x,p2.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x,p2.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x,p1.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p1.x,p1.y,p1.z); ll = lp++;

  lp->m_p1 = Vertex(p1.x,p1.y,p2.z);       lp->m_p2 = Vertex(p1.x,p2.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x,p2.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x,p1.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p1.x,p1.y,p2.z); ll = lp++;
  for(int i = 0; i < 4; i++) {
    lines[i+8].m_p1 = lines[i  ].m_p1;
    lines[i+8].m_p2 = lines[i+4].m_p1;
  }
  initBuffer(lines, 12);
}

// ----------------------------------------------------- D3LineArrow ------------------------------------------------------

D3LineArrow::D3LineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, D3DCOLOR color) : SceneObjectWithVertexBuffer(scene) {
#define FANCOUNT 14
#define ITEMCOUNT (5 + 2 * FANCOUNT)

  const D3DXVECTOR3 v  = (D3DXVECTOR3)to - (D3DXVECTOR3)from;
  const D3DXVECTOR3 vn = unitVector(v);

  VertexNormal *vertices = GETLOCKEDVERTEXBUFFER(VertexNormal, ITEMCOUNT);

  vertices[0].setPos(from);
  vertices[1].setPos(to); vertices[1].setNormal(vn);
  const D3DXVECTOR3 cirkelCenter = (D3DXVECTOR3)to - 0.1f * vn;
  D3DXVECTOR3   radius1 = ortonormalVector(v) * 0.04f;
  D3DXVECTOR3   p       = cirkelCenter + radius1;
  VertexNormal *vtx1    = vertices + 2;
  vtx1->setPos(p); vtx1->setNormal(radius1); vtx1++;

  D3DXVECTOR3   radius2 = radius1;
  VertexNormal *vtx2 = vertices + FANCOUNT + 3;
  vtx2->setPos(cirkelCenter); vtx2->setNormal(-vn); vtx2++;
  vtx2->setPos(p);            vtx2->setNormal(-vn); vtx2++;

  for(int i = 0; i < FANCOUNT; i++) {
    radius1 = rotate(radius1, v,  radians(360.0f/FANCOUNT));
    radius2 = rotate(radius2, v, -radians(360.0f/FANCOUNT));
    vtx1[i].setPos(cirkelCenter + radius1); vtx1[i].setNormal(unitVector(radius1));
    vtx2[i].setPos(cirkelCenter + radius2); vtx2[i].setNormal(-vn);
  }
  unlockVertexBuffer();
  m_materialIndex = -1;
  setColor(color);
}

void D3LineArrow::draw() {
  if(hasVertexBuffer()) {
    getScene().setFillMode(D3DFILL_SOLID).setShadeMode(D3DSHADE_GOURAUD);
    setStreamSource();
    setLightingEnable(true);
    setSceneMaterial();
    drawPrimitive( D3DPT_LINELIST   , 0         , 1       );
    drawPrimitive( D3DPT_TRIANGLEFAN, 1         , FANCOUNT);
    drawPrimitive( D3DPT_TRIANGLEFAN, FANCOUNT+3, FANCOUNT);
  }
}

void D3LineArrow::setColor(D3DCOLOR color) {
  const D3DCOLORVALUE cv = colorToColorValue(color);
  if(hasMaterial()) {
    MATERIAL mat = getMaterial();
    mat.Diffuse  = cv;
    mat.Emissive = cv;
    getScene().setMaterial(mat);
  } else {
    D3DMATERIAL mat;
    memset(&mat, 0, sizeof(mat));
    mat.Diffuse  = cv;
    mat.Emissive = cv;
    m_materialIndex = getScene().addMaterial(mat);
  }
}

// ----------------------------------------------- D3Curve ------------------------------------------------------------

D3Curve::D3Curve(D3Scene &scene, const VertexArray &points) : SceneObjectWithVertexBuffer(scene) {
  m_primitiveCount = (int)points.size()-1;

  Vertex *vertices = GETLOCKEDVERTEXBUFFER(Vertex, (UINT)points.size());
  memcpy(vertices, points.getBuffer(), sizeof(Vertex)*points.size());
  unlockVertexBuffer();
}

void D3Curve::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    drawPrimitive(D3DPT_LINESTRIP, 0, m_primitiveCount);
  }
}

D3CurveArray::D3CurveArray(D3Scene &scene, const CurveArray &curves) : SceneObjectWithVertexBuffer(scene) {
  int totalVertexCount = 0;

  for(size_t i = 0; i < curves.size(); i++) {
    const int vertexCount = (int)curves[i].size();
    m_curveSize.add(vertexCount);
    totalVertexCount += vertexCount;
  }
  Vertex *vertices = GETLOCKEDVERTEXBUFFER(Vertex, totalVertexCount);

  int startIndex = 0;
  for(size_t i = 0; i < curves.size(); i++) {
    const VertexArray &a     = curves[i];
    const int          count = (int)a.size();
    memcpy(vertices+startIndex, a.getBuffer(), sizeof(Vertex)*count);
    startIndex += count;
  }
  unlockVertexBuffer();
}

void D3CurveArray::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    int startIndex = 0;
    for(size_t i = 0; i < m_curveSize.size(); i++) {
      const int vertexCount = m_curveSize[i];
      drawPrimitive(D3DPT_LINESTRIP, startIndex, vertexCount-1);
      startIndex += vertexCount;
    }
  }
}

void SceneObjectSolidBox::makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3) {
  Face &f = mb.addFace();
  const int nIndex = mb.addNormal(mb.calculateNormal(v0,v1,v2));
  f.addVertexNormalIndex(v0, nIndex);
  f.addVertexNormalIndex(v1, nIndex);
  f.addVertexNormalIndex(v2, nIndex);
  f.addVertexNormalIndex(v3, nIndex);
}

SceneObjectSolidBox::SceneObjectSolidBox(D3Scene &scene, const D3DXCube3 &cube, int materialIndex)
: SceneObjectWithMesh(scene)
, m_materialIndex(materialIndex)
{
  MeshBuilder mb;
  const D3DXVECTOR3 &pmin = cube.getMin(), &pmax = cube.getMax();
  const int  lbn = mb.addVertex(pmin.x,pmin.y,pmin.z); // left  bottom near corner
  const int  lbf = mb.addVertex(pmin.x,pmin.y,pmax.z); // left  bottom far  corner
  const int  ltn = mb.addVertex(pmin.x,pmax.y,pmin.z); // left  top    near corner
  const int  ltf = mb.addVertex(pmin.x,pmax.y,pmax.z); // left  top    far  corner
  const int  rbn = mb.addVertex(pmax.x,pmin.y,pmin.z); // right bottom near corner
  const int  rbf = mb.addVertex(pmax.x,pmin.y,pmax.z); // right bottom far  corner
  const int  rtn = mb.addVertex(pmax.x,pmax.y,pmin.z); // right top    near corner
  const int  rtf = mb.addVertex(pmax.x,pmax.y,pmax.z); // right top    far  corner

  makeSquareFace(mb,lbn,lbf,rbf,rbn);              // bottom
  makeSquareFace(mb,ltn,rtn,rtf,ltf);              // top
  makeSquareFace(mb,lbn,ltn,ltf,lbf);              // left side
  makeSquareFace(mb,lbf,ltf,rtf,rbf);              // back side
  makeSquareFace(mb,rbf,rtf,rtn,rbn);              // right side
  makeSquareFace(mb,rbn,rtn,ltn,lbn);              // front side

  m_mesh = mb.createMesh(getScene(), false);
}

// -----------------------------------------------------------------------------------------------------------

#define SINCOS(degree,c,s) double c = radians(degree), s; sincos(c,s)
#define RSINCOS(degree,r,c,s) SINCOS(degree,c,s); c*=r; s*=r

CurveArray createSphereObject(double r) {
  CurveArray curves;
  for(int fi = 0; fi < 180; fi += 45) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  for(int fi = -180+30; fi < 180; fi += 30) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  return curves;
}
