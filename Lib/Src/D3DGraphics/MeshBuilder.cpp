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
    const Face &face = m_faceArray[i];
    const CompactArray<VertexNormalTextureIndex> &vna = face.getIndices();
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
  m_validated  = true;
  m_validateOk = false;
  const int maxVertexIndex = (int)m_vertices.size()-1;
  const int maxNormalIndex = (int)m_normals.size() -1;
  for(size_t i = 0; i < m_faceArray.size(); i++) {
    const CompactArray<VertexNormalTextureIndex> &indexArray = m_faceArray[i].getIndices();
    for(size_t j = 0; j < indexArray.size(); j++) {
      const VertexNormalTextureIndex &v = indexArray[j];
      if((int)v.m_vIndex > maxVertexIndex) {
        throwException(_T("Face %d reference undefined vertex %u. maxVertexIndex=%d"), (int)i, v.m_vIndex, maxVertexIndex);
      }
      if((int)v.m_nIndex > maxNormalIndex) {
        throwException(_T("Face %d reference undefined normal %u. maxNormalIndex=%d"), (int)i, v.m_nIndex, maxNormalIndex);
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

static Semaphore   meshCreatorGate;

template<class VertexType, class IndexType> class MeshCreator {
private:
  const MeshBuilder &m_mb;

  inline UINT getMeshFlags() const {
    return D3DXMESH_SYSTEMMEM | ((sizeof(IndexType) == sizeof(long)) ? D3DXMESH_32BIT : 0);
  }

  LPD3DXMESH createMesh1NormalPerVertex(DIRECT3DDEVICE device, bool doubleSided) const {
    const Array<Face> &faceArray         = m_mb.getFaceArray();
    const VertexArray &vertexArray       = m_mb.getVertexArray();
    const VertexArray &normalArray       = m_mb.getNormalArray();

    const int          factor            = doubleSided ? 2 : 1;
    const int          vertexCount1Side  = (int)vertexArray.size();
    const int          vertexCount       = vertexCount1Side * factor;
    const int          faceCount1Side    = m_mb.getTriangleCount();
    const int          faceCount         = faceCount1Side * factor;
    bool               inCriticalSection = false;
    BitSet             vertexDone(vertexCount1Side);

    LPD3DXMESH mesh = NULL;
    try {
      meshCreatorGate.wait();
      inCriticalSection = true;
      V(D3DXCreateMeshFVF(faceCount, vertexCount, getMeshFlags(), VertexType::FVF_Flags, device, &mesh));
      inCriticalSection = false;
      meshCreatorGate.signal();
      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face                                   &face         = faceArray[i];
        const CompactArray<VertexNormalTextureIndex> &vnArray      = face.getIndices();
        const int                                     aSize        = (int)vnArray.size();
        const D3DCOLOR                                diffuseColor = face.getDiffuseColor();

        for(int j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          if(vertexDone.contains(vn.m_vIndex)) {
            continue;
          }
          vertexDone.add(vn.m_vIndex);
          const Vertex &v = vertexArray[vn.m_vIndex];
          const Vertex &n = normalArray[vn.m_nIndex];
          vertices[vn.m_vIndex].setPosAndNormal(v, n, diffuseColor);
          if(doubleSided) {
            vertices[vn.m_vIndex + vertexCount1Side].setPosAndNormal(v,-n, diffuseColor);
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
        mesh->Release();
      }
      if(inCriticalSection) {
        meshCreatorGate.signal();
      }
      throw;
    }
  }

  LPD3DXMESH createMeshManyNormalsPerVertex(DIRECT3DDEVICE device, bool doubleSided) const {
    const Array<Face> &faceArray         = m_mb.getFaceArray();
    const VertexArray &vertexArray       = m_mb.getVertexArray();
    const VertexArray &normalArray       = m_mb.getNormalArray();

    const int          factor            = doubleSided ? 2 : 1;
    const int          vertexCount1Side  = m_mb.getIndexCount();
    const int          vertexCount       = vertexCount1Side * factor;
    const int          faceCount1Side    = m_mb.getTriangleCount();
    const int          faceCount         = faceCount1Side * factor;
    bool               inCriticalSection = false;

    LPD3DXMESH mesh = NULL;
    try {
      meshCreatorGate.wait();
      inCriticalSection = true;
      V(D3DXCreateMeshFVF((DWORD)faceCount, (DWORD)vertexCount, getMeshFlags(), VertexType::FVF_Flags, device, &mesh));
      inCriticalSection = false;
      meshCreatorGate.signal();

      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;
      int vnCount1 = 0, vnCount2 = (int)vertexCount1Side;

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face                                   &face         = faceArray[i];
        const CompactArray<VertexNormalTextureIndex> &vnArray      = face.getIndices();
        const int                                     aSize        = (int)vnArray.size();
        const D3DCOLOR                                diffuseColor = face.getDiffuseColor();

        for(int j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          const Vertex &v = vertexArray[vn.m_vIndex];
          const Vertex &n = normalArray[vn.m_nIndex];
          vertices[vnCount1+j].setPosAndNormal(v, n, diffuseColor);

          if(doubleSided) {
            vertices[vnCount2+j].setPosAndNormal(v,-n, diffuseColor);
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
        mesh->Release();
      }
      if(inCriticalSection) {
        meshCreatorGate.signal();
      }
      throw;
    }
  }

public:
  MeshCreator(const MeshBuilder *mb) : m_mb(*mb) {
  }

  LPD3DXMESH createMesh(DIRECT3DDEVICE device, bool doubleSided) const {
    if(m_mb.has1NormalPerVertex()) {
      return createMesh1NormalPerVertex(device, doubleSided);
    } else {
      return createMeshManyNormalsPerVertex(device, doubleSided);
    }
  }
};

/*
typedef struct {
  const Vertex *m_v, *m_n;
  int m_faceIndex, m_vindex;
} TmpVertex;

static int tmpVertexCmp(const TmpVertex &tv1, const TmpVertex &tv2) {
  const Vertex &v1 = *tv1.m_v;
  const Vertex &v2 = *tv2.m_v;
  int c;
  c = sign(v1.x - v2.x); if(c) return c;
  c = sign(v1.y - v2.y); if(c) return c;
  c = sign(v1.z - v2.z); if(c) return c;
  const Vertex &n1 = *tv1.m_n;
  const Vertex &n2 = *tv2.m_n;
  c = sign(n1.x - n2.x); if(c) return c;
  c = sign(n1.y - n2.y); if(c) return c;
  c = sign(n1.z - n2.z); if(c) return c;
  return 0;
}

inline bool operator==(const TmpVertex &tv1, const TmpVertex &tv2) {
  return tmpVertexCmp(tv1, tv2) == 0;
}

void MeshBuilder::optimize() {
  const int faceCount = m_faceArray.size();

  CompactArray<TmpVertex> tvArray;
  for(int f = 0; f < faceCount; f++) {
    const Face                            &face         = m_faceArray[f];
    const CompactArray<VertexNormalIndex> &vnArray      = face.getIndices();
    const int                              aSize        = vnArray.size();
    for(int v = 0; v < aSize; v++) {
      const VertexNormalIndex &vn = vnArray[v];
      TmpVertex tv;
      tv.m_faceIndex = f;
      tv.m_vindex    = v;
      tv.m_v         = &m_vertices[vn.m_vIndex];
      tv.m_n         = &m_normals[vn.m_nIndex];
      tvArray.add(tv);
    }
  }
  const int n = tvArray.size();
  if(n > 0) {
    tvArray.sort(tmpVertexCmp);
    TmpVertex               *lastTv = &tvArray[0];
    const VertexNormalIndex *lastVn = &m_faceArray[lastTv->m_faceIndex].m_data[lastTv->m_vindex];
    for(int i = 1; i < n; i++) {
      TmpVertex *tv = &tvArray[i];
      if(*tv == *lastTv) {
        VertexNormalIndex &vn = m_faceArray[tv->m_faceIndex].m_data[tv->m_vindex];
        vn.m_vIndex = lastVn->m_vIndex;
        vn.m_nIndex = lastVn->m_nIndex;
      } else {
        lastTv = tv;
        lastVn = &m_faceArray[lastTv->m_faceIndex].m_data[lastTv->m_vindex];
      }
    }
  }
  pruneUnused();
}
*/
void MeshBuilder::pruneUnused() {
  const size_t vCount = getVertexArray().size();
  const size_t nCount = getNormalArray().size();
  BitSet unusedVertices(vCount);
  BitSet unusedNormals(nCount);
  unusedVertices.add(0, vCount-1);
  unusedNormals.add( 0, nCount-1);
  const size_t faceCount = m_faceArray.size();
  for(size_t f = 0; f < faceCount; f++) {
    const CompactArray<VertexNormalTextureIndex> &vnArray = m_faceArray[f].m_data;
    size_t n = vnArray.size();
    if(n) {
      for(const VertexNormalTextureIndex *vnp = &vnArray[0]; n--; vnp++) {
        unusedVertices.remove(vnp->m_vIndex);
        unusedNormals.remove( vnp->m_nIndex);
      }
    }
  }
  if(unusedVertices.isEmpty() && unusedNormals.isEmpty()) {
#ifdef DUMP_PRUNECOUNT
    debugLog(_T("Pruned 0 vertices, 0 normals\n"));
#endif
    return;
  }
  CompactIntArray vTranslate(vCount),nTranslate(nCount);
  int lastIndex = 0;
  for(size_t i = 0; i < vCount; i++) {
    if(unusedVertices.contains(i)) {
      vTranslate.add(-1);
    } else {
      vTranslate.add(lastIndex++);
    }
  }
  lastIndex = 0;
  for(size_t i = 0; i < nCount; i++) {
    if(unusedNormals.contains(i)) {
      nTranslate.add(-1);
    } else {
      nTranslate.add(lastIndex++);
    }
  }
  for(size_t f = 0; f < faceCount; f++) {
    CompactArray<VertexNormalTextureIndex> &vnArray = m_faceArray[f].m_data;
    size_t n = vnArray.size();
    if(n) {
      for(VertexNormalTextureIndex *vnp = &vnArray[0]; n--; vnp++) {
        vnp->m_vIndex = vTranslate[vnp->m_vIndex];
        vnp->m_nIndex = nTranslate[vnp->m_nIndex];
      }
    }
  }
  vTranslate.clear();
  nTranslate.clear();

  VertexArray tmp = m_vertices;
  m_vertices.clear();
  for(size_t i = 0; i < vCount; i++) {
    if(!unusedVertices.contains(i)) {
      m_vertices.add(tmp[i]);
    }
  }

  tmp = m_normals;
  m_normals.clear();
  for(size_t i = 0; i < nCount; i++) {
    if(!unusedNormals.contains(i)) {
      m_normals.add(tmp[i]);
    }
  }
#ifdef DUMP_PRUNECOUNT
  debugLog(_T("Pruned %s vertices, %s normals\n")
          ,format1000(unusedVertices.size()).cstr()
          ,format1000(unusedNormals.size()).cstr()
          );
#endif
}

LPD3DXMESH MeshBuilder::createMesh(DIRECT3DDEVICE device, bool doubleSided) const {
  if(!isOk()) {
    throwException(_T("MeshBuilder inconsistent"));
  }

  const bool use32Bit = use32BitIndices(doubleSided);

  if(hasColors()) {
    if(use32Bit) {
      return MeshCreator<VertexNormalDiffuse, ULONG>(this).createMesh(device, doubleSided);
    } else {
      return MeshCreator<VertexNormalDiffuse, USHORT>(this).createMesh(device, doubleSided);
    }
  } else {
    if(use32Bit) {
      return MeshCreator<VertexNormal, ULONG>(this).createMesh(device, doubleSided);
    } else {
      return MeshCreator<VertexNormal, USHORT>(this).createMesh(device, doubleSided);
    }
  }
}

void MeshBuilder::dump(const String &fileName) const {
  const String name = (fileName.length() > 0) ? fileName : _T("c:\\temp\\meshbuilderDump.txt");
  FILE *f = MKFOPEN(name, _T("w"));
  _ftprintf(f, _T("MeshBuilder:\n"));
  _ftprintf(f, _T("Vertices:%d\n"), (int)m_vertices.size());
  for(int i = 0; i < (int)m_vertices.size(); i++) {
    _ftprintf(f, _T("%5d %s\n"), i, toString(m_vertices[i], 5).cstr());
  }
  _ftprintf(f, _T("Normals:%d\n"), (int)m_normals.size());
  for(int i = 0; i < (int)m_normals.size(); i++) {
    _ftprintf(f, _T("%5d %s\n"), i, toString(m_normals[i], 5).cstr());
  }
  _ftprintf(f, _T("TextureVertices:%d\n"), (int)m_textureVertexArray.size());
  for(int i = 0; i < (int)m_textureVertexArray.size(); i++) {
    _ftprintf(f, _T("%5d %s\n"), i, m_textureVertexArray[i].toString(5).cstr());
  }
  _ftprintf(f, _T("Faces:%d\n"), (int)m_faceArray.size());
  for(int i = 0; i < (int)m_faceArray.size(); i++) {
    const Face                            &face = m_faceArray[i];
    const CompactArray<VertexNormalTextureIndex> &vna  = face.getIndices();
    _ftprintf(f, _T("%5d %d "), i, (int)vna.size());
    for(int v = 0; v < (int)vna.size(); v++) {
      _ftprintf(f, _T(" %5d %5d %5d"), vna[v].m_vIndex, vna[v].m_nIndex, vna[v].m_tIndex);
    }
    _ftprintf(f, _T("\n"));
  }
  fclose(f);
}
