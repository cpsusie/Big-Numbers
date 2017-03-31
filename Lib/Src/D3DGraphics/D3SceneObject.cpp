#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

// ------------------------------------------------ D3SceneObject ---------------------------------------------------

void D3SceneObject::prepareDraw(unsigned int flags) {
  if(flags & USE_SCENEFILLMODE ) {
    V(getDevice()->SetRenderState(D3DRS_FILLMODE,  m_scene.getFillMode()));
  }
  if(flags & USE_SCENESHADEMODE) {
    V(getDevice()->SetRenderState(D3DRS_SHADEMODE, m_scene.getShadeMode()));
  }
  if(flags & USE_SCENEMATERIAL) {
    V(getDevice()->SetMaterial(&m_scene.getMaterial()));
  }
}

bool D3SceneObject::intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info) const {
  LPD3DXMESH mesh = ((D3SceneObject*)this)->getMesh();
  if(mesh == NULL) {
    return false;
  }
  const D3DXMATRIX m = invers(getWorldMatrix());
  D3DXVECTOR3 vOrig = m * ray.m_orig;
  D3DXVECTOR3 vDir  = ray.m_dir * m;

  BOOL         hit;
  DWORD        faceIndex;
  float        pu,pv;
  V(D3DXIntersect(mesh
                 ,&vOrig
                 ,&vDir
                 ,&hit
                 ,&faceIndex
                 ,&pu,&pv
                 ,&dist
                 ,NULL // when these parameters are NULL, only the closest intersection (if any) will be recognized (dist)
                 ,NULL
                 ));

  if(hit && info != NULL) {
    info->m_faceIndex = faceIndex;
    void *indexItems;
    LPDIRECT3DINDEXBUFFER indexBuffer;
    V(mesh->GetIndexBuffer(&indexBuffer));
    D3DINDEXBUFFER_DESC desc;
    V(indexBuffer->GetDesc(&desc));
    const bool use32Bit = desc.Format == D3DFMT_INDEX32;
    V(indexBuffer->Lock(0,0,&indexItems, D3DLOCK_READONLY));

    const int vertex0Index = faceIndex * 3;
    if(use32Bit) {
      unsigned long *ip = (unsigned long*)indexItems;
      info->m_i1 = ip[vertex0Index+0];
      info->m_i2 = ip[vertex0Index+1];
      info->m_i3 = ip[vertex0Index+2];
    } else {
      unsigned short *ip = (unsigned short*)indexItems;
      info->m_i1 = ip[vertex0Index+0];
      info->m_i2 = ip[vertex0Index+1];
      info->m_i3 = ip[vertex0Index+2];
    }
    V(indexBuffer->Unlock());
  }

  return hit ? true : false;
}

// ------------------------------------------------ SceneObjectWithVertexBuffer ---------------------------------------------------

SceneObjectWithVertexBuffer::SceneObjectWithVertexBuffer(D3Scene &scene) : D3SceneObject(scene) {
  m_vertexBuffer   = NULL;
  m_primitiveCount = 0;
}

SceneObjectWithVertexBuffer::~SceneObjectWithVertexBuffer() {
  if(m_vertexBuffer != NULL) {
    m_vertexBuffer->Release();
  }
}

void *SceneObjectWithVertexBuffer::allocateVertexBuffer(int vertexSize, UINT count, DWORD fvf) {
  const UINT bufferSize = (UINT)(vertexSize*count);
  V(getDevice()->CreateVertexBuffer(bufferSize, 0, fvf, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL));
  m_vertexSize = vertexSize;
  m_fvf        = fvf;
  void *bufferItems = NULL;
  V(m_vertexBuffer->Lock(0, bufferSize, &bufferItems, 0));
  return bufferItems;
}

void SceneObjectWithVertexBuffer::unlockVertexBuffer() {
  V(m_vertexBuffer->Unlock());
}

void SceneObjectWithVertexBuffer::prepareDraw(UINT flags) {
  V(getDevice()->SetStreamSource( 0, m_vertexBuffer, 0, m_vertexSize));
  V(getDevice()->SetFVF(m_fvf));
  D3SceneObject::prepareDraw(flags);
}

#define GETLOCKEDVERTEXBUFFER(type, count) (type*)allocateVertexBuffer(sizeof(type), count, type::FVF_Flags)


// ------------------------------------------------ SceneObjectWithIndexBuffer ---------------------------------------------------

SceneObjectWithIndexBuffer::SceneObjectWithIndexBuffer(D3Scene &scene) : SceneObjectWithVertexBuffer(scene) {
  m_indexBuffer = NULL;
}
 
SceneObjectWithIndexBuffer::~SceneObjectWithIndexBuffer() {
  if(m_indexBuffer != NULL) {
    m_indexBuffer->Release();
  }
}

void *SceneObjectWithIndexBuffer::allocateIndexBuffer(bool int32, int count) {
  const int itemSize = int32 ? sizeof(long) : sizeof(short);
  const int bufferSize = count * itemSize;
  void *bufferItems = NULL;
  V(getDevice()->CreateIndexBuffer(bufferSize, 0, int32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_indexBuffer, NULL));
  V(m_indexBuffer->Lock(0,0,&bufferItems, 0));
  return bufferItems;
}

void SceneObjectWithIndexBuffer::unlockIndexBuffer() {
  V(m_indexBuffer->Unlock());
}

void SceneObjectWithIndexBuffer::prepareDraw(UINT flags) {
  SceneObjectWithVertexBuffer::prepareDraw(flags);
  V(getDevice()->SetIndices(m_indexBuffer));
}

#define GETLOCKEDSHORTBUFFER(count) (unsigned short*)allocateIndexBuffer(false, count)
#define GETLOCKEDLONGBUFFER( count) (unsigned long* )allocateIndexBuffer(true , count)

// ------------------------------------------------ D3LineArray -----------------------------------------------------------

SceneObjectWithMesh::SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh) : D3SceneObject(scene) {
  m_mesh = mesh;
}

SceneObjectWithMesh::~SceneObjectWithMesh() {
  releaseMesh();
}

void SceneObjectWithMesh::createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf) {
  releaseMesh();
  V(D3DXCreateMeshFVF(faceCount, vertexCount, D3DXMESH_SYSTEMMEM/*|D3DXMESH_32BIT*/, fvf, getDevice(), &m_mesh));
}

void SceneObjectWithMesh::releaseMesh() {
  if(m_mesh) {
    m_mesh->Release();
    m_mesh = NULL;
  }
}

void *SceneObjectWithMesh::lockVertexBuffer() {
  void *vertices;
  V(m_mesh->LockVertexBuffer( 0, &vertices));
  return vertices;
}

void *SceneObjectWithMesh::lockIndexBuffer() {
  void *items;
  V(m_mesh->LockIndexBuffer( 0, &items));
  return items;
}

void SceneObjectWithMesh::unlockVertexBuffer() {
  V(m_mesh->UnlockVertexBuffer());
}

void SceneObjectWithMesh::unlockIndexBuffer() {
  V(m_mesh->UnlockIndexBuffer());
}

void SceneObjectWithMesh::draw() {
  prepareDraw();
  V(m_mesh->DrawSubset(0));
}

// -----------------------------------------------------------------------------------------------------------


D3LineArray::D3LineArray(D3Scene &scene, const Line *lines, int n) : SceneObjectWithVertexBuffer(scene) {
  initBuffer(lines, n);
}

D3LineArray::D3LineArray(D3Scene &scene, const Vertex p1, const Vertex p2) : SceneObjectWithVertexBuffer(scene) {
  Line lines[12], *lp = lines, *ll;
  lp->m_p1 = p1;                           lp->m_p2 = createVertex(p1.x,p2.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p2.x,p2.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p2.x,p1.y,p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p1.x,p1.y,p1.z); ll = lp++;

  lp->m_p1 = createVertex(p1.x,p1.y,p2.z); lp->m_p2 = createVertex(p1.x,p2.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p2.x,p2.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p2.x,p1.y,p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = createVertex(p1.x,p1.y,p2.z); ll = lp++;
  for(int i = 0; i < 4; i++) {
    lines[i+8].m_p1 = lines[i  ].m_p1;
    lines[i+8].m_p2 = lines[i+4].m_p1;
  }
  initBuffer(lines, 12);
}

void D3LineArray::initBuffer(const Line *lines, int n) {
  Vertex *vertices = GETLOCKEDVERTEXBUFFER(Vertex, 2*n);
  m_primitiveCount = n;
  memcpy(vertices, lines, sizeof(Line)*n);
  unlockVertexBuffer();
}

void D3LineArray::draw() {
  prepareDraw();
  V(getDevice()->DrawPrimitive( D3DPT_LINELIST , 0, m_primitiveCount));
}

// ----------------------------------------------------- D3LineArrow ------------------------------------------------------

D3DCOLORVALUE colorToColorValue(D3DCOLOR c) {
  D3DCOLORVALUE result;
  result.r = ((float)(ARGB_GETRED(  c))) / (float)255.0;
  result.g = ((float)(ARGB_GETGREEN(c))) / (float)255.0;
  result.b = ((float)(ARGB_GETBLUE( c))) / (float)255.0;
  result.a = 1.0;
  return result;
}

D3LineArrow::D3LineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, D3DCOLOR color) : SceneObjectWithVertexBuffer(scene) {
#define FANCOUNT 14
#define ITEMCOUNT (5 + 2 * FANCOUNT)

  const D3DXVECTOR3 v  = (D3DXVECTOR3)to - (D3DXVECTOR3)from;
  const D3DXVECTOR3 vn = unitVector(v);

  VertexNormal *vertices = GETLOCKEDVERTEXBUFFER(VertexNormal, ITEMCOUNT);

  vertices[0].setPos(from);
  vertices[1].setPosAndNormal(to, vn);
  const D3DXVECTOR3 cirkelCenter = (D3DXVECTOR3)to - 0.1f * vn;
  D3DXVECTOR3   radius1 = ortonormalVector(v) * 0.04f;
  D3DXVECTOR3   p       = cirkelCenter + radius1;
  VertexNormal *vtx1    = vertices + 2;
  vtx1->setPosAndNormal(p, radius1); vtx1++;

  D3DXVECTOR3 radius2 = radius1;
  VertexNormal *vtx2 = vertices + FANCOUNT + 3;
  vtx2->setPosAndNormal(cirkelCenter, -vn); vtx2++;
  vtx2->setPosAndNormal(p,            -vn); vtx2++;

  for(int i = 0; i < FANCOUNT; i++) {
    radius1 = rotate(radius1, v ,  radians(360.0f/FANCOUNT));
    radius2 = rotate(radius2, v, -radians(360.0f/FANCOUNT));
    vtx1[i].setPosAndNormal(cirkelCenter + radius1, unitVector(radius1));
    vtx2[i].setPosAndNormal(cirkelCenter + radius2, -vn);
  }
  unlockVertexBuffer();
  setColor(color);
}

void D3LineArrow::draw() {
  prepareDraw(USE_SCENEFILLMODE | USE_SCENESHADEMODE);
  V(getDevice()->SetMaterial(&getMaterial()));
  V(getDevice()->DrawPrimitive( D3DPT_LINELIST   , 0, 1));
  V(getDevice()->DrawPrimitive( D3DPT_TRIANGLEFAN, 1, FANCOUNT));
  V(getDevice()->DrawPrimitive( D3DPT_TRIANGLEFAN, FANCOUNT+3,FANCOUNT));
}

void D3LineArrow::setColor(D3DCOLOR color) {
  m_color = color;
}

D3DMATERIAL D3LineArrow::getMaterial() const {
  D3DMATERIAL mat;
  ZeroMemory(&mat, sizeof(D3DMATERIAL));
  const D3DCOLORVALUE cv = colorToColorValue(m_color);
  mat.Diffuse  = cv;
//  mat.Ambient  = cv;
  mat.Emissive = cv;
  return mat;
}

// ----------------------------------------------- D3Curve ------------------------------------------------------------

D3Curve::D3Curve(D3Scene &scene, const VertexArray &points) : SceneObjectWithVertexBuffer(scene) {
  m_primitiveCount = (int)points.size()-1;

  Vertex *vertices = GETLOCKEDVERTEXBUFFER(Vertex, (UINT)points.size());
  memcpy(vertices, points.getBuffer(), sizeof(Vertex)*points.size());
  unlockVertexBuffer();
}

void D3Curve::draw() {
  prepareDraw();
  V(getDevice()->DrawPrimitive(D3DPT_LINESTRIP, 0, m_primitiveCount));
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
  prepareDraw();
  int startIndex = 0;
  for(size_t i = 0; i < m_curveSize.size(); i++) {
    const int vertexCount = m_curveSize[i];
    V(getDevice()->DrawPrimitive(D3DPT_LINESTRIP, startIndex, vertexCount-1));
    startIndex += vertexCount;
  }
}

// -----------------------------------------------------------------------------------------------------------

#define SINCOS(degree,c,s) double c = radians(degree), s; sincos(c,s)

CurveArray createSphereObject(double r) {
  CurveArray curves;
  for(int fi = 0; fi < 180; fi += 45) {
    SINCOS(fi, RcosFi, RsinFi);
    RcosFi *= r;
    RsinFi *= r;
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      SINCOS(theta, cosTheta, sinTheta);
      va.add(createVertex(RsinFi*cosTheta, RsinFi*sinTheta, RcosFi));
    }
    curves.add(va);
  }
  for(int fi = -180+30; fi < 180; fi += 30) {
    SINCOS(fi, RcosFi, RsinFi);
    RcosFi *= r;
    RsinFi *= r;
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      SINCOS(theta, cosTheta, sinTheta);
      va.add(createVertex(RsinFi*cosTheta, RsinFi*sinTheta, RcosFi));
    }
    curves.add(va);
  }

  return curves;
}
