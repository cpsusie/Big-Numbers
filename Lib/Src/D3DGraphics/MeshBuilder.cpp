#include "pch.h"
#include <CompactHashMap.h>
#if defined(_DEBUG)
#include <D3DGraphics/D3ToString.h>
#endif // _DEBUG
#include <D3DGraphics/MeshBuilder.h>

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

MeshBuilder &MeshBuilder::addSquareFace(int v0, int v1, int v2, int v3) {
  Face &f = addFace();
  const int nIndex = addNormal(calculateNormal(v0, v1, v2));
  f.addVertexNormalIndex(v0, nIndex);
  f.addVertexNormalIndex(v1, nIndex);
  f.addVertexNormalIndex(v2, nIndex);
  f.addVertexNormalIndex(v3, nIndex);
  return *this;
}

// Return true, if normals point in same direction as the visible side of the face
bool MeshBuilder::hasCorrectOrientation(const Face &f) const {
  const UINT         n  = f.getIndexCount();
  const VNTIArray   &fa = f.getIndexArray();
  const VertexArray &va = getVertexArray();
  const VertexArray &na = getNormalArray();
  const D3DXVECTOR3 &p1 = va[fa[0].m_vIndex].getPos();
  const D3DXVECTOR3 &p2 = va[fa[1].m_vIndex].getPos();
  const D3DXVECTOR3 &p3 = va[fa[2].m_vIndex].getPos();
  const D3DXVECTOR3 &n1 = na[fa[0].m_nIndex].getPos();
//const D3DXVECTOR3 &n2 = na[fa[1].m_nIndex].getPos();
//const D3DXVECTOR3 &n3 = na[fa[2].m_nIndex].getPos();
  const D3DXVECTOR3 c   = cross(p3 - p1, p2 - p1);
  const float       s1  = c * n1; // , s2 = c * n2, s3 = c * n3; // TODO
  return s1 > 0;
}

UINT MeshBuilder::getTriangleCount() const {
  UINT count = 0;
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


void MeshBuilder::check1NormalPerVertex() const {
  m_vertexNormalChecked = true;
  m_1NormalPerVertex    = false;
  if(m_vertices.size() != m_normals.size()) {
    return;
  }
  const size_t faceCount = m_faceArray.size();
  CompactUIntHashMap<UINT> vnMap(2*m_vertices.size() + 241);

  for(size_t i = 0; i < faceCount; i++) {
    const VNTIArray &vna = m_faceArray[i].getIndexArray();
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

UINT MeshBuilder::getIndexCount() const {
  UINT count = 0;
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
  m_calculateNormals = maxNormalIndex  >= 0;
  m_calculateTexture = maxTextureIndex >= 0;
  const UINT faceCount = (UINT)m_faceArray.size();
  for(UINT i = 0; i < faceCount; i++) {
    const VNTIArray &indexArray = m_faceArray[i].getIndexArray();
    for(const VertexNormalTextureIndex vnt : indexArray) {
      if(vnt.m_vIndex > maxVertexIndex) {
        throwException(_T("Face %u references undefined vertex %d. maxVertexIndex=%d"), i, vnt.m_vIndex, maxVertexIndex);
      }
      if(vnt.m_nIndex > maxNormalIndex) {
        throwException(_T("Face %u references undefined normal %d. maxNormalIndex=%d"), i, vnt.m_nIndex, maxNormalIndex);
      }
      if(vnt.m_tIndex > maxTextureIndex) {
        throwException(_T("Face %u references undefined textureVertex %d. maxTextureVertexIndex=%d"), i, vnt.m_tIndex, maxTextureIndex);
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
  const UINT   factor           = doubleSided ? 2 : 1;
  const size_t vertexCount1Side = has1NormalPerVertex() ? m_vertices.size() : getIndexCount();
  const size_t vertexCount      = vertexCount1Side * factor;
  const UINT   faceCount1Side   = getTriangleCount();
  const UINT   faceCount        = faceCount1Side * factor;
  return (vertexCount > MAX16BITVERTEXCOUNT) || (faceCount > MAX16BITVERTEXCOUNT);
}

template<typename VertexType, typename IndexType> class MeshCreator {
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

    const UINT                factor            = doubleSided ? 2 : 1;
    const UINT                vertexCount1Side  = (UINT)vertexArray.size();
    const UINT                vertexCount       = vertexCount1Side * factor;
    const UINT                faceCount1Side    = m_mb.getTriangleCount();
    const UINT                faceCount         = faceCount1Side * factor;
    BitSet                    vertexDone(vertexCount1Side);

    LPD3DXMESH mesh = NULL;
    try {
      mesh = amf.allocateMesh(VertexType::FVF_Flags, faceCount, vertexCount, getMeshFlags());
      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;

#if defined(_DEBUG)
      const D3DVERTEXBUFFER_DESC vdesc = getvDesc(mesh);
      const D3DINDEXBUFFER_DESC  idesc = getiDesc(mesh);
      const UINT                 vSize = vdesc.Size;
      const UINT                 iSize = idesc.Size;
      const VertexType          *maxvp = vertices   + vSize;
      const IndexType           *maxip = indexArray + iSize;
#endif // _DEBUG

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face      &face         = faceArray[i];
        const UINT       aSize        = face.getIndexCount();
        const VNTIArray &vnArray      = face.getIndexArray();
        const D3DCOLOR   diffuseColor = face.getDiffuseColor();

        for(UINT j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          if(vertexDone.contains(vn.m_vIndex)) {
            continue;
          }
          vertexDone.add(vn.m_vIndex);
          VertexType &v1 = vertices[vn.m_vIndex];
          assert((VertexType*)(((BYTE*)&v1)+sizeof(VertexType)) < maxvp);
          v1.setPos(vertexArray[vn.m_vIndex]);
          if(vn.m_nIndex >= 0) v1.setNormal(normalArray[vn.m_nIndex]);
          if(vn.m_tIndex >= 0) v1.setTexture(textureArray[vn.m_tIndex]);
          v1.setDiffuse(diffuseColor);
          if(doubleSided) {
            VertexType &v2 = vertices[vn.m_vIndex + vertexCount1Side];
            assert((VertexType*)(((BYTE*)&v2)+sizeof(VertexType)) < maxvp);
            v2.setPos(vertexArray[vn.m_vIndex]);
            if(vn.m_nIndex >= 0) v2.setNormal(-normalArray[vn.m_nIndex]);
            if(vn.m_tIndex >= 0) v2.setTexture(textureArray[vn.m_tIndex]);
            v2.setDiffuse(diffuseColor);
          }
        }
        for(UINT j = 2; j < aSize; j++) {
          *(ip1++) = vnArray[0  ].m_vIndex;
          *(ip1++) = vnArray[j-1].m_vIndex;
          assert(ip1 < maxip);
          *(ip1++) = vnArray[j  ].m_vIndex;
        }
        if(doubleSided) {
          for(UINT j = 2; j < aSize; j++) {
            *(ip2++) = vnArray[0  ].m_vIndex + vertexCount1Side;
            *(ip2++) = vnArray[j  ].m_vIndex + vertexCount1Side;
            assert(ip2 < maxip);
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
      throw;
    }
  }

  LPD3DXMESH createMeshManyNormalsPerVertex(AbstractMeshFactory &amf, bool doubleSided) const {
    const Array<Face>        &faceArray          = m_mb.getFaceArray();
    const VertexArray        &vertexArray        = m_mb.getVertexArray();
    const VertexArray        &normalArray        = m_mb.getNormalArray();
    const TextureVertexArray &textureArray       = m_mb.getTextureVertexArray();
    const UINT                factor             = doubleSided ? 2 : 1;
    const UINT                vertexCount1Side   = m_mb.getIndexCount();
    const UINT                vertexCount        = vertexCount1Side * factor;
    const UINT                faceCount1Side     = m_mb.getTriangleCount();
    const UINT                faceCount          = faceCount1Side * factor;

    LPD3DXMESH mesh = NULL;
    try {
      mesh = amf.allocateMesh(VertexType::FVF_Flags, (DWORD)faceCount, (DWORD)vertexCount, getMeshFlags());
      VertexType *vertices;
      IndexType  *indexArray;
      V(mesh->LockVertexBuffer( 0, (void**)&vertices  ));
      V(mesh->LockIndexBuffer(  0, (void**)&indexArray));
      IndexType *ip1 = indexArray, *ip2 = indexArray + 3*faceCount1Side;
      UINT vnCount1 = 0, vnCount2 = vertexCount1Side;

#if defined(_DEBUG)
      const D3DVERTEXBUFFER_DESC vdesc = getvDesc(mesh);
      const D3DINDEXBUFFER_DESC  idesc = getiDesc(mesh);
      const UINT                 vSize = vdesc.Size;
      const UINT                 iSize = idesc.Size;
      const VertexType          *maxvp = vertices   + vSize;
      const IndexType           *maxip = indexArray + iSize;
#endif // _DEBUG

      for(size_t i = 0; i < faceArray.size(); i++) {
        const Face      &face         = faceArray[i];
        const UINT       aSize        = face.getIndexCount();
        const VNTIArray &vnArray      = face.getIndexArray();
        const D3DCOLOR   diffuseColor = face.getDiffuseColor();

        for(UINT j = 0; j < aSize; j++) {
          const VertexNormalTextureIndex &vn = vnArray[j];
          VertexType                     &v1 = vertices[vnCount1+j];
          assert((VertexType*)(((BYTE*)&v1)+sizeof(VertexType)) < maxvp);
          v1.setPos(vertexArray[vn.m_vIndex]);
          if(vn.m_nIndex >= 0) v1.setNormal( normalArray[ vn.m_nIndex]);
          if(vn.m_tIndex >= 0) v1.setTexture(textureArray[vn.m_tIndex]);
          v1.setDiffuse(diffuseColor);

          if(doubleSided) {
            VertexType                   &v2 = vertices[vnCount2+j];
            assert((VertexType*)(((BYTE*)&v2)+sizeof(VertexType)) < maxvp);
            v2.setPos(vertexArray[vn.m_vIndex]);
            if(vn.m_nIndex >= 0) v2.setNormal(-normalArray[ vn.m_nIndex]);
            if(vn.m_tIndex >= 0) v2.setTexture(textureArray[vn.m_tIndex]);
            v2.setDiffuse(diffuseColor);
          }
        }

        for(UINT f = 2; f < aSize; f++) {
          *(ip1++) = vnCount1;
          *(ip1++) = vnCount1 + f-1;
          assert(ip1 < maxip);
          *(ip1++) = vnCount1 + f;

          if(doubleSided) {
            *(ip2++) = vnCount2;
            *(ip2++) = vnCount2 + f;
            assert(ip2 < maxip);
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

template<typename VertexType> class MeshCreator1 {
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

LPD3DXMESH MeshBuilder::createMesh(AbstractMeshFactory &amf, bool doubleSided) const {
  if(!isOk()) {
    throwException(_T("MeshBuilder inconsistent"));
  }
//  debugLog(_T("%s:\n%s"), __TFUNCTION__           , indentString(toString()     , 2).cstr());

  const MeshBuilder &copy = *this;
//  debugLog(_T("%s:\n%s"), _T("Copy (unoptimized)"), indentString(copy.toString(), 2).cstr());

//  copy.optimize();
//  debugLog(_T("%s:\n%s"), _T("Copy (optimized)")  , indentString(copy.toString(), 2).cstr());

  if(copy.calculateNormals()) {
    if(copy.calculateTexture()) {
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
    if(copy.calculateTexture()) {
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
