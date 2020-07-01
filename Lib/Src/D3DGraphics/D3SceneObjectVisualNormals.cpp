#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectLineArray.h>
#include <D3DGraphics/D3SceneObjectWithIndexBuffer.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>

#define NEEDEDFVFFLAGS (D3DFVF_XYZ | D3DFVF_NORMAL)

// -------------------------------------------------------------------------------------------------------------

class D3SceneObjectVectorLineArray : public D3SceneObjectLineArray {
public:
  D3SceneObjectVectorLineArray(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lines)
     : D3SceneObjectLineArray(parent, lines)
  {
  }
  int getMaterialId() const override {
    return getParent()->getMaterialId();
  }
  D3DXMATRIX &getWorld() override {
    return getParent()->getWorld();
  }
};

// -------------------------------------------------------------------------------------------------------------

#define TRIANGLECOUNT 6

class D3SceneObjectTriangleArray : public D3SceneObjectWithIndexBuffer {
private:
  const UINT m_vertexCount, m_primitiveCount;
public:
  D3SceneObjectTriangleArray(D3SceneObjectVisual *parent, const CompactArray<VertexNormal> &varray, const CompactUintArray &iarray)
    : D3SceneObjectWithIndexBuffer(parent,varray,iarray)
    , m_vertexCount((UINT)varray.size())
    , m_primitiveCount((UINT)iarray.size()/3)
  {
  }
  int getMaterialId() const override {
    return getParent()->getMaterialId();
  }
  D3DXMATRIX &getWorld() override {
    return getParent()->getWorld();
  }
  void draw() override;
};

void D3SceneObjectTriangleArray::draw() {
  if(hasVertexBuffer()) {
    D3Device &device = setDeviceMaterialIfExist();
    setStreamSource().setIndices(getIndexBuffer())
                     .setWorldMatrix(getWorld())
                     .setFillMode(getFillMode())
                     .setShadeMode(getShadeMode())
                     .setLightingEnable(getLightingEnable())
                     .drawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertexCount, 0, m_primitiveCount);
  }
}

// -------------------------------------------------------------------------------------------------------------

class D3SceneObjectVectorArrary : public D3SceneObjectVisual {
  D3SceneObjectVectorLineArray *m_linesObject;
  D3SceneObjectTriangleArray   *m_trianglesObject;
  int                           m_materialId;
public:
  D3SceneObjectVectorArrary(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lines);
  ~D3SceneObjectVectorArrary();
  int getMaterialId() const override {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override {
    return getParent()->getWorld();
  }
  void draw() override;
};

D3SceneObjectVectorArrary::D3SceneObjectVectorArrary(D3SceneObjectVisual *parent, const CompactArray<Line3D> &lines)
: D3SceneObjectVisual(parent)
{
  m_materialId = getScene().addMaterialWithColor(D3D_YELLOW);
  const size_t               n = lines.size();
  CompactArray<VertexNormal> varray(n*(TRIANGLECOUNT+1));
  CompactUintArray           iarray(n*TRIANGLECOUNT*3);
  for(size_t i = 0; i < n; i++) {
    const Line3D      &line          = lines[i];
    const D3DXVECTOR3 &from          = line.m_p1;
    const D3DXVECTOR3 &to            = line.m_p2;
    const D3DXVECTOR3  v             = to - from;
    const float        vlen          = length(v);
    const UINT         topPointIndex = (UINT)varray.size();
    const D3DXVECTOR3  cirkelCenter  = (D3DXVECTOR3)to - 0.3f * v;
    D3DXVECTOR3        radius        = ortonormalVector(v) * 0.11f * vlen;
    D3DXQUATERNION     rot           = createRotation(v, 2 * D3DX_PI / TRIANGLECOUNT);

    varray.add(VertexNormal().setPos(to).setNormal(v));
    for(int i = 0; i < TRIANGLECOUNT; i++) {
      varray.add(VertexNormal().setPos(cirkelCenter + radius).setNormal(radius));
      radius = rotate(radius, rot);
    }
    const UINT c0 = topPointIndex + 1;
    for(int i = 0; i < TRIANGLECOUNT; i++) {
      iarray.add(topPointIndex);
      iarray.add(c0 + (i + 1) % TRIANGLECOUNT);
      iarray.add(c0 + i);
    }
  }
  m_linesObject     = new D3SceneObjectVectorLineArray(this, lines);          TRACE_NEW(m_linesObject    );
  m_trianglesObject = new D3SceneObjectTriangleArray(  this, varray, iarray); TRACE_NEW(m_trianglesObject);
}

D3SceneObjectVectorArrary::~D3SceneObjectVectorArrary() {
  SAFEDELETE(m_linesObject    );
  SAFEDELETE(m_trianglesObject);
  getScene().removeMaterial(m_materialId);
}

void D3SceneObjectVectorArrary::draw() {
  m_linesObject->draw();
  m_trianglesObject->draw();
}

// -------------------------------------------------------------------------------------------------------------

D3SceneObjectVisualNormals::D3SceneObjectVisualNormals(D3SceneObjectVisual *parent)
  : D3SceneObjectVisual(parent,format(_T("%s.normals"), parent->getName().cstr()))
{
  if(parent->hasMesh()) {
    LPD3DXMESH mesh = parent->getMesh();
    LPDIRECT3DVERTEXBUFFER vertexBuffer;
    V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_CREATE(vertexBuffer);
    createNormalsObject(vertexBuffer);
    SAFERELEASE(vertexBuffer);
  } else if(parent->hasVertexBuffer()) {
    LPDIRECT3DVERTEXBUFFER vertexBuffer = parent->getVertexBuffer();
    createNormalsObject(vertexBuffer);
  }
};

D3SceneObjectVisualNormals::~D3SceneObjectVisualNormals() {
  destroyNormalsObject();
}

static CompactArray<Line3D> &getNormalArray(LPDIRECT3DVERTEXBUFFER vertexBuffer, CompactArray<Line3D> &lines) {
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  if((desc.FVF & NEEDEDFVFFLAGS) != NEEDEDFVFFLAGS) {
    throwInvalidArgumentException(__TFUNCTION__, _T("vertexBuffer has no normals:fvf=%s"), FVFToString(desc.FVF).cstr());
  }
  const D3Cube  boundingBox  = getBoundingBox(vertexBuffer);
  const float   diagLength   = (boundingBox.RTF() - boundingBox.LBN()).length();
  const UINT    itemSize     = FVFToSize(desc.FVF);
  const UINT    vertexCount  = desc.Size / itemSize;
  const float   normalLength = diagLength / sqrtf((float)vertexCount) * 0.8f;

  void         *vertices;
  V(vertexBuffer->Lock(0, desc.Size, &vertices, D3DLOCK_READONLY));
  lines.clear(vertexCount);
  for(const char *srcp = (char*)vertices, *endp = srcp + desc.Size; srcp < endp; srcp += itemSize) {
    const VertexNormal &srcv = *(VertexNormal*)srcp;
    const D3DXVECTOR3   vn = unitVector(srcv.getNormal());
    const D3DXVECTOR3   to = srcv.getPos() + normalLength * vn;
    lines.add(Line3D(srcv.getPos(), to));
  }
  V(vertexBuffer->Unlock());
  return lines;
}

void D3SceneObjectVisualNormals::createNormalsObject(LPDIRECT3DVERTEXBUFFER vertexBuffer) {
  CompactArray<Line3D> lines;
  m_normalsObject = new D3SceneObjectVectorArrary(getParent(), getNormalArray(vertexBuffer,lines));  TRACE_NEW(m_normalsObject);
}

void D3SceneObjectVisualNormals::destroyNormalsObject() {
  SAFEDELETE(m_normalsObject);
}

void D3SceneObjectVisualNormals::draw() {
  m_normalsObject->draw();
}
