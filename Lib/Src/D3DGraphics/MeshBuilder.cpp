#include "pch.h"
#include <Semaphore.h>
#include <comdef.h>
#include <atlconv.h>
#include <CompactHashMap.h>
#include <D3DGraphics/MeshBuilder.h>

DECLARE_THISFILE;

void Face::invertOrientation() {
  UINT n = getIndexCount()-1;
  for (UINT i = 0; i < n;) {
    m_data.swap(i++,n--);
  }
}

void MeshBuilder::clear(UINT capacity) {
  m_vertices.clear();
  m_normals.clear();
  m_faceArray.clear();
  if(capacity > 0) {
    m_faceArray.setCapacity(capacity);
  }
  m_hasColors           = false;
  m_vertexNormalChecked = false;
  m_1NormalPerVertex    = false;
  m_validated           = false;
  m_validateOk          = false;
}

int MeshBuilder::getTriangleCount() const {
  int count = 0;
  const size_t n = m_faceArray.size();
  for(size_t i = 0; i < n; i++) {
    count += m_faceArray[i].getTriangleCount();
  }
  return count;
}

bool MeshBuilder::isEmpty() const {
  const size_t n = m_faceArray.size();
  for(size_t i = 0; i < n; i++) {
    if(m_faceArray[i].getTriangleCount() > 0) {
      return false;
    }
  }
  return true;
}

void MeshBuilder::adjustNegativeVertexIndex(int &v) {
  assert(v < 0);
  v += (int)m_vertices.size();
  assert(v >= 0);
}

void MeshBuilder::adjustNegativeNormalIndex(int &n) {
  assert(n < 0);
  n += (int)m_normals.size();
  assert(n >= 0);
}

void MeshBuilder::adjustNegativeTextureIndex(int &t) {
  assert(t < 0);
  t += (int)m_textureVertexArray.size();
  assert(t >= 0);
}

class UInt {
private:
  UINT m_v;
public:
  inline UInt() {
  }
  inline UInt(UINT v) : m_v(v) {
  }
  inline UINT hashCode() const {
    return m_v;
  }
  inline bool operator==(const UInt u) const {
    return m_v == u.m_v;
  }
};

void MeshBuilder::check1NormalPerVertex() const {
  m_vertexNormalChecked = true;
  m_1NormalPerVertex    = false;
  if(m_vertices.size() != m_normals.size()) {
    return;
  }
  const size_t faceCount = m_faceArray.size();
  CompactHashMap<UInt, UINT> vnMap(2*m_vertices.size() + 241);

  for(size_t i = 0; i < faceCount; i++) {
    const VNTIArray &vna = m_faceArray[i].getIndices();
    for(size_t j = 0; j < vna.size(); j++) {
      const VertexNormalTextureIndex &vn = vna[j];
      const UINT *np = vnMap.get(vn.m_vIndex);
      if(np) {
        if(*np != vn.m_nIndex) {
          return;
        }
      } else {
        vnMap.put(vn.m_vIndex, vn.m_nIndex);
      }
    }
  }
  m_1NormalPerVertex = true;
}

int MeshBuilder::getIndexCount() const {
  int count = 0;
  const size_t n = m_faceArray.size();
  for(size_t i = 0; i < n; i++) {
    count += m_faceArray[i].getIndexCount();
  }
  return count;
}

void MeshBuilder::validate() const {
  m_validated   = true;
  m_validateOk  = false;

  const int maxVertexIndex  = (int)m_vertices.size()           - 1;
  const int maxNormalIndex  = (int)m_normals.size()            - 1;
  const int maxTextureIndex = (int)m_textureVertexArray.size() - 1;
  m_hasNormals  = maxNormalIndex  >= 0;
  m_hasTextures = maxTextureIndex >= 0;
  const size_t faceCount = m_faceArray.size();
  for(size_t i = 0; i < faceCount; i++) {
    const VNTIArray &indexArray = m_faceArray[i].getIndices();
    const size_t     indexCount = indexArray.size();
    if(indexCount == 0) continue;
    const VertexNormalTextureIndex *last = &indexArray.last();
    for(const VertexNormalTextureIndex *vntp = &indexArray.first(); vntp <= last ; vntp++) {
      if(vntp->m_vIndex > maxVertexIndex) {
        throwException(_T("Face %zu/%zu references undefined vertex %d. maxVertexIndex=%d"), i, vntp->m_vIndex, maxVertexIndex);
      }
      if(vntp->m_nIndex > maxNormalIndex) {
        throwException(_T("Face %zu/%zu references undefined normal %d. maxNormalIndex=%d"), i, vntp->m_nIndex, maxNormalIndex);
      }
      if(vntp->m_tIndex > maxTextureIndex) {
        throwException(_T("Face %zu%zu references undefined textureVertex %d. maxTextureVertexIndex=%d"), i, vntp->m_tIndex, maxTextureIndex);
      }
    }
  }
  m_validateOk = true;
}

bool MeshBuilder::isOk() const {
  if(!m_validated) {
    validate();
  }
  return m_validateOk;
}


bool MeshBuilder::has1NormalPerVertex() const {
  if(!m_vertexNormalChecked) {
    check1NormalPerVertex();
  }
  return m_1NormalPerVertex;
}

bool MeshBuilder::use32BitIndices(bool doubleSided) const {
  const int    factor           = doubleSided ? 2 : 1;
  const size_t vertexCount1Side = has1NormalPerVertex() ? m_vertices.size() : getIndexCount();
  const size_t vertexCount      = vertexCount1Side * factor;
  const int    faceCount1Side   = getTriangleCount();
  const int    faceCount        = faceCount1Side * factor;
  return (vertexCount > MAX16BITVERTEXCOUNT) || (faceCount > MAX16BITVERTEXCOUNT);
}

D3DXCube3 MeshBuilder::getBoundingBox() const {
  D3DXVECTOR3  pmin,pmax;
  const size_t faceCount = m_faceArray.size();
  size_t       i;
  bool         firstTime = true;
  const int    maxVertexIndex  = (int)m_vertices.size() - 1;
  for(i = 0; i < faceCount; i++) {
    const Face &f = m_faceArray[i];
    if(f.isEmpty()) continue;
    const size_t     n = f.getIndexCount();
    const VNTIArray &a = f.getIndices();
    for(size_t j = 0; j < n; j++) {
      const int vi = a[j].m_vIndex;
      if((vi >= 0) && (vi <= maxVertexIndex)) {
        const Vertex &v = m_vertices[vi];
        if(firstTime) {
          pmin = pmax = v;
          firstTime = false;
        } else {
          if(v.x < pmin.x) {
            pmin.x = v.x;
          } else if(v.x > pmax.x) {
            pmax.x = v.x;
          }
          if(v.y < pmin.y) {
            pmin.y = v.y;
          } else if(v.y > pmax.y) {
            pmax.y = v.y;
          }
          if(v.z < pmin.z) {
            pmin.z = v.z;
          } else if(v.z > pmax.z) {
            pmax.z = v.z;
          }
        }
      }
    }
  }
  return D3DXCube3(pmin,pmax);
}

static Semaphore   meshCreatorGate;

template<class VertexType, class IndexType> class MeshCreator {
private:
  const MeshBuilder &m_mb;

  inline UINT getMeshFlags() const {
    return D3DXMESH_SYSTEMMEM | ((sizeof(IndexType) == sizeof(long)) ? D3DXMESH_32BIT : 0);
  }

  LPD3DXMESH createMesh1NormalPerVertex(AbstractMeshFactory &amf, bool doubleSided) const {
    const Array<Face>        &faceArray         = m_mb.getFaceArray();
    const VertexArray        &vertexArray       = m_mb.getVertexArray();
    const VertexArray        &normalArray       = m_mb.getNormalArray();
    const TextureVertexArray &textureArray      = m_mb.getTextureVertexArray();

    const int                 factor            = doubleSided ? 2 : 1;
    const int                 vertexCount1Side  = (int)vertexArray.size();
    const int                 vertexCount       = vertexCount1Side * factor;
    const int                 faceCount1Side    = m_mb.getTriangleCount();
    const int                 faceCount         = faceCount1Side * factor;
    bool                      inCriticalSection = false;
    BitSet                    vertexDone(vertexCount1Side);

    LPD3DXMESH mesh = NULL;
    try {
      meshCreatorGate.wait();
      inCriticalSection = true;
      mesh = amf.allocateMesh(VertexType::FVF_Flags, faceCount, vertexCount, getMeshFlags());
      inCriticalSection = false;
      meshCreatorGate.signal();
      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face      &face         = faceArray[i];
        const VNTIArray &vnArray      = face.getIndices();
        const int        aSize        = (int)vnArray.size();
        const D3DCOLOR   diffuseColor = face.getDiffuseColor();

        for(int j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          if(vertexDone.contains(vn.m_vIndex)) {
            continue;
          }
          vertexDone.add(vn.m_vIndex);
          VertexType &v1 = vertices[vn.m_vIndex];
          v1.setPos(vertexArray[vn.m_vIndex]);
          if(vn.m_nIndex >= 0) v1.setNormal(normalArray[vn.m_nIndex]);
          if(vn.m_tIndex >= 0) v1.setTexture(textureArray[vn.m_tIndex]);
          v1.setDiffuse(diffuseColor);
          if(doubleSided) {
            VertexType &v2 = vertices[vn.m_vIndex + vertexCount1Side];
            v2.setPos(vertexArray[vn.m_vIndex]);
            if(vn.m_nIndex >= 0) v2.setNormal(-normalArray[vn.m_nIndex]);
            if(vn.m_tIndex >= 0) v2.setTexture(textureArray[vn.m_tIndex]);
            v2.setDiffuse(diffuseColor);
          }
        }
        for(int j = 2; j < aSize; j++) {
          *(ip1++) = vnArray[0  ].m_vIndex;
          *(ip1++) = vnArray[j-1].m_vIndex;
          *(ip1++) = vnArray[j  ].m_vIndex;
        }
        if(doubleSided) {
          for(int j = 2; j < aSize; j++) {
            *(ip2++) = vnArray[0  ].m_vIndex + vertexCount1Side;
            *(ip2++) = vnArray[j  ].m_vIndex + vertexCount1Side;
            *(ip2++) = vnArray[j-1].m_vIndex + vertexCount1Side;
          }
        }
      }
      V(mesh->UnlockIndexBuffer());
      V(mesh->UnlockVertexBuffer());
      return mesh;
    } catch(...) {
      if(mesh) {
        mesh->UnlockIndexBuffer();
        mesh->UnlockVertexBuffer();
        SAFERELEASE(mesh);
      }
      if(inCriticalSection) {
        meshCreatorGate.signal();
      }
      throw;
    }
  }

  LPD3DXMESH createMeshManyNormalsPerVertex(AbstractMeshFactory &amf, bool doubleSided) const {
    const Array<Face>        &faceArray          = m_mb.getFaceArray();
    const VertexArray        &vertexArray        = m_mb.getVertexArray();
    const VertexArray        &normalArray        = m_mb.getNormalArray();
    const TextureVertexArray &textureArray       = m_mb.getTextureVertexArray();
    const int                 factor             = doubleSided ? 2 : 1;
    const int                 vertexCount1Side   = m_mb.getIndexCount();
    const int                 vertexCount        = vertexCount1Side * factor;
    const int                 faceCount1Side     = m_mb.getTriangleCount();
    const int                 faceCount          = faceCount1Side * factor;
    bool                      inCriticalSection  = false;

    LPD3DXMESH mesh = NULL;
    try {
      meshCreatorGate.wait();
      inCriticalSection = true;
      mesh = amf.allocateMesh(VertexType::FVF_Flags, (DWORD)faceCount, (DWORD)vertexCount, getMeshFlags());
      inCriticalSection = false;
      meshCreatorGate.signal();

      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;
      int vnCount1 = 0, vnCount2 = (int)vertexCount1Side;

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face      &face         = faceArray[i];
        const VNTIArray &vnArray      = face.getIndices();
        const int        aSize        = (int)vnArray.size();
        const D3DCOLOR   diffuseColor = face.getDiffuseColor();

        for(int j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          VertexType                     &v1 = vertices[vnCount1+j];
          v1.setPos(vertexArray[vn.m_vIndex]);
          if(vn.m_nIndex >= 0) v1.setNormal(normalArray[vn.m_nIndex]);
          if(vn.m_tIndex >= 0) v1.setTexture(textureArray[vn.m_tIndex]);
          v1.setDiffuse(diffuseColor);

          if(doubleSided) {
            VertexType                     &v2 = vertices[vnCount2+j];
            v2.setPos(vertexArray[vn.m_vIndex]);
            if(vn.m_nIndex >= 0) v2.setNormal(-normalArray[vn.m_nIndex]);
            if(vn.m_tIndex >= 0) v2.setTexture(textureArray[vn.m_tIndex]);
            v2.setDiffuse(diffuseColor);
          }
        }

        for(int f = 2; f < aSize; f++) {
          *(ip1++) = vnCount1;
          *(ip1++) = vnCount1 + f-1;
          *(ip1++) = vnCount1 + f;

          if(doubleSided) {
            *(ip2++) = vnCount2;
            *(ip2++) = vnCount2 + f;
            *(ip2++) = vnCount2 + f-1;
          }
        }
        vnCount1 += aSize;
        vnCount2 += aSize;
      }
      V(mesh->UnlockIndexBuffer());
      V(mesh->UnlockVertexBuffer());
      return mesh;
    } catch(...) {
      if(mesh) {
        mesh->UnlockIndexBuffer();
        mesh->UnlockVertexBuffer();
        SAFERELEASE(mesh);
      }
      if(inCriticalSection) {
        meshCreatorGate.signal();
      }
      throw;
    }
  }

public:
  MeshCreator(const MeshBuilder &mb) : m_mb(mb) {
  }

  LPD3DXMESH createMesh(AbstractMeshFactory &amf, bool doubleSided) const {
    if(m_mb.has1NormalPerVertex()) {
      return createMesh1NormalPerVertex(amf, doubleSided);
    } else {
      return createMeshManyNormalsPerVertex(amf, doubleSided);
    }
  }
};

template<class VertexType> class MeshCreator1 {
private:
  const MeshBuilder &m_mb;
public:
  MeshCreator1(const MeshBuilder &mb) : m_mb(mb) {
  }
  LPD3DXMESH createMesh(AbstractMeshFactory &amf, bool doubleSided) const {
    if(m_mb.use32BitIndices(doubleSided)) {
      return MeshCreator<VertexType, ULONG >(m_mb).createMesh(amf, doubleSided);
    } else {
      return MeshCreator<VertexType, USHORT>(m_mb).createMesh(amf, doubleSided);
    }
  }
};

typedef struct {
  const Vertex        *m_v, *m_n;
  const TextureVertex *m_t;
  int m_faceIndex, m_index;
} TmpVNTI;

static int tmpVertexCmp(const TmpVNTI &tv1, const TmpVNTI &tv2) {
  const Vertex *v1 = tv1.m_v, *v2 = tv2.m_v;
  int c;
  c = sign(v1->x - v2->x); if(c) return c;
  c = sign(v1->y - v2->y); if(c) return c;
  c = sign(v1->z - v2->z); if(c) return c;
  const Vertex *n1 = tv1.m_n, *n2 = tv2.m_n;
  if(n1 != n2) {
    if(!n2) {
      return 1;
    } else if(!n1) {
      return -1;
    } else {
      c = sign(n1->x - n2->x); if(c) return c;
      c = sign(n1->y - n2->y); if(c) return c;
      c = sign(n1->z - n2->z); if(c) return c;
    }
  }
  const TextureVertex *t1 = tv1.m_t, *t2 = tv2.m_t;
  if(t1 != t2) {
    if(!t2) {
      return 1;
    } else if(!t1) {
      return -1;
    } else {
      c = sign(t1->u - t2->u); if(c) return c;
      c = sign(t1->v - t2->v); if(c) return c;
    }
  }
  return 0;
}

inline bool operator==(const TmpVNTI &tv1, const TmpVNTI &tv2) {
  return tmpVertexCmp(tv1, tv2) == 0;
}

void MeshBuilder::optimize() {
  const int             faceCount = (int)m_faceArray.size();
  CompactArray<TmpVNTI> tmpArray;
  for(int f = 0; f < faceCount; f++) {
    const VNTIArray &a     = m_faceArray[f].getIndices();
    const int        aSize = (int)a.size();
    for(int v = 0; v < aSize; v++) {
      const VertexNormalTextureIndex &vnti = a[v];
      tmpArray.add(TmpVNTI());
      TmpVNTI &tmp    = tmpArray.last();
      tmp.m_faceIndex = f;
      tmp.m_index     = v;
      tmp.m_v         = &m_vertices[vnti.m_vIndex];
      tmp.m_n         = (vnti.m_nIndex<0) ? NULL : &m_normals[           vnti.m_nIndex];
      tmp.m_t         = (vnti.m_tIndex<0) ? NULL : &m_textureVertexArray[vnti.m_tIndex];
    }
  }
  const size_t n = tmpArray.size();
  if(n > 0) {
    tmpArray.sort(tmpVertexCmp);
    TmpVNTI                        *lastTmp = &tmpArray[0];
    const VertexNormalTextureIndex *lastVn  = &m_faceArray[lastTmp->m_faceIndex].m_data[lastTmp->m_index];
    for(size_t i = 1; i < n; i++) {
      TmpVNTI &tmp = tmpArray[i];
      if(tmp == *lastTmp) {
        VertexNormalTextureIndex &vn = m_faceArray[tmp.m_faceIndex].m_data[tmp.m_index];
        vn.m_vIndex = lastVn->m_vIndex;
        vn.m_nIndex = lastVn->m_nIndex;
        vn.m_tIndex = lastVn->m_tIndex;
      } else {
        lastTmp = &tmp;
        lastVn  = &m_faceArray[tmp.m_faceIndex].m_data[tmp.m_index];
      }
    }
    pruneUnused();
  }
}

void MeshBuilder::pruneUnused() {
  const int vCount = getVertexCount();
  const int nCount = getNormalCount();
  const int tCount = getTextureCount();
  BitSet unusedVertices(vCount  );
  BitSet unusedNormals( nCount+1);
  BitSet unusedTextures(tCount+1);
  if(vCount > 0) unusedVertices.add(0, vCount-1);
  if(nCount > 0) unusedNormals.add( 0, nCount-1);
  if(tCount > 0) unusedTextures.add(0, tCount-1);
  const size_t faceCount = m_faceArray.size();
  for(size_t f = 0; f < faceCount; f++) {
    const VNTIArray &vnArray = m_faceArray[f].getIndices();
    size_t           n       = vnArray.size();
    if(n) {
      for(const VertexNormalTextureIndex *vnp = &vnArray[0]; n--; vnp++) {
        unusedVertices.remove(vnp->m_vIndex);
        if(vnp->m_nIndex >= 0) unusedNormals.remove( vnp->m_nIndex);
        if(vnp->m_tIndex >= 0) unusedTextures.remove(vnp->m_tIndex);
      }
    }
  }
  if(unusedVertices.isEmpty()
  && unusedNormals.isEmpty()
  && unusedTextures.isEmpty()) {
#ifdef DUMP_PRUNECOUNT
    debugLog(_T("Pruned 0 vertices, 0 normals, 0 textureVertices\n"));
#endif
    return;
  }
  CompactIntArray vTranslate(vCount), nTranslate(nCount), tTranslate(tCount);
  int lastIndex = 0;
  for(int i = 0; i < vCount; i++) {
    if(unusedVertices.contains(i)) {
      vTranslate.add(-1);
    } else {
      vTranslate.add(lastIndex++);
    }
  }
  lastIndex = 0;
  for(int i = 0; i < nCount; i++) {
    if(unusedNormals.contains(i)) {
      nTranslate.add(-1);
    } else {
      nTranslate.add(lastIndex++);
    }
  }
  lastIndex = 0;
  for(int i = 0; i < tCount; i++) {
    if(unusedTextures.contains(i)) {
      tTranslate.add(-1);
    } else {
      tTranslate.add(lastIndex++);
    }
  }
  for(size_t f = 0; f < faceCount; f++) {
    VNTIArray &vnArray = m_faceArray[f].m_data;
    size_t     n       = vnArray.size();
    if(n) {
      for(VertexNormalTextureIndex *vnp = &vnArray[0]; n--; vnp++) {
        vnp->m_vIndex = vTranslate[vnp->m_vIndex];
        if(vnp->m_nIndex >= 0) {
          vnp->m_nIndex = nTranslate[vnp->m_nIndex];
        }
        if(vnp->m_tIndex >= 0) {
          vnp->m_tIndex = tTranslate[vnp->m_tIndex];
        }
      }
    }
  }
  vTranslate.clear();
  nTranslate.clear();
  tTranslate.clear();

  VertexArray tmp = m_vertices;
  m_vertices.clear();
  for(int i = 0; i < vCount; i++) {
    if(!unusedVertices.contains(i)) {
      m_vertices.add(tmp[i]);
    }
  }

  tmp = m_normals;
  m_normals.clear();
  for(int i = 0; i < nCount; i++) {
    if(!unusedNormals.contains(i)) {
      m_normals.add(tmp[i]);
    }
  }
  TextureVertexArray tmpt = m_textureVertexArray;
  m_textureVertexArray.clear();
  for(int i = 0; i < tCount; i++) {
    if(!unusedTextures.contains(i)) {
      m_textureVertexArray.add(tmpt[i]);
    }
  }
#ifdef DUMP_PRUNECOUNT
  debugLog(_T("Pruned %s vertices, %s normals, %s texturevertices\n")
          ,format1000(unusedVertices.size()).cstr()
          ,format1000(unusedNormals.size() ).cstr()
          ,format1000(unusedTextures.size()).cstr()
          );
#endif
}

LPD3DXMESH MeshBuilder::createMesh(AbstractMeshFactory &amf, bool doubleSided) const {
  if(!isOk()) {
    throwException(_T("MeshBuilder inconsistent"));
  }
//  debugLog(_T("%s:\n%s"), __TFUNCTION__           , indentString(toString()     , 2).cstr());

  const MeshBuilder &copy = *this;
//  debugLog(_T("%s:\n%s"), _T("Copy (unoptimized)"), indentString(copy.toString(), 2).cstr());

//  copy.optimize();
//  debugLog(_T("%s:\n%s"), _T("Copy (optimized)")  , indentString(copy.toString(), 2).cstr());

  if(copy.hasNormals()) {
    if(copy.hasTextureVertices()) {
      if(copy.hasColors()) {
        return MeshCreator1<VertexNormalDiffuseTex1>(copy).createMesh(amf, doubleSided);
      } else { // no colors
        return MeshCreator1<VertexNormalTex1>(       copy).createMesh(amf, doubleSided);
      }
    } else {
      if(copy.hasColors()) {
        return MeshCreator1<VertexNormalDiffuse>(    copy).createMesh(amf, doubleSided);
      } else {
        return MeshCreator1<VertexNormal>(           copy).createMesh(amf, doubleSided);
      }
    }
  } else { // no normals
    if(copy.hasTextureVertices()) {
      if(copy.hasColors()) {
        return MeshCreator1<VertexDiffuseTex1>(      copy).createMesh(amf, doubleSided);
      } else { // no colors
        return MeshCreator1<VertexTex1>(             copy).createMesh(amf, doubleSided);
      }
    } else { // no textures
      if(copy.hasColors()) {
        return MeshCreator1<VertexDiffuse>(          copy).createMesh(amf, doubleSided);
      } else {
        return MeshCreator1<Vertex>(                 copy).createMesh(amf, doubleSided);
      }
    }
  }
}
