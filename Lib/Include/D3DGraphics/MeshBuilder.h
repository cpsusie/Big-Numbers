#pragma once

#include "D3AbstractMeshFactory.h"
#include "D3Math.h"
#include "D3Cube.h"

class VertexNormalTextureIndex {
public:
  int m_vIndex, m_nIndex, m_tIndex;
  inline VertexNormalTextureIndex() {}
  inline VertexNormalTextureIndex(int vIndex, int nIndex, int tIndex)
    : m_vIndex(vIndex), m_nIndex(nIndex), m_tIndex(tIndex)
  {
  }
  bool operator==(const VertexNormalTextureIndex &v) const;
  ULONG hashCode() const;
  String toString() const;
};

template<typename OUTSTREAM> OUTSTREAM &operator<<(OUTSTREAM &out, const VertexNormalTextureIndex &v) {
  out << v.toString();
  return out;
}

typedef CompactArray<VertexNormalTextureIndex> VNTIArray;

class Face {
private:
  VNTIArray m_data;
  D3DCOLOR  m_diffuseColor;
public:
  inline Face(D3DCOLOR diffuseColor, size_t capacity=3) : m_data(capacity) {
    m_diffuseColor = diffuseColor;
  }
  inline void addVertexNormalTextureIndex(int vIndex, int nIndex, int tIndex) {
    m_data.add(VertexNormalTextureIndex(vIndex, nIndex, tIndex));
  }
  inline void addVertexNormalIndex(int vIndex, int nIndex) {
    m_data.add(VertexNormalTextureIndex(vIndex, nIndex, -1));
  }
  inline void addVertexTextureIndex(int vIndex, int tIndex) {
    m_data.add(VertexNormalTextureIndex(vIndex, -1, tIndex));
  }
  inline UINT getTriangleCount() const {
    const UINT ic = getIndexCount();
    return (ic < 3) ? 0 : (ic - 2);
  }
  inline UINT getIndexCount() const {
    return (UINT)m_data.size();
  }
  inline bool isEmpty() const {
    return m_data.isEmpty();
  }
  inline const VNTIArray &getIndexArray() const {
    return m_data;
  }
  inline int getDiffuseColor() const {
    return m_diffuseColor;
  }
  void invertOrientation();
  friend class MeshBuilder;
  String toString() const;
};

#define MAX16BITVERTEXCOUNT 0xfffe

class MeshBuilder {
private:
  VertexArray                 m_vertices;
  VertexArray                 m_normals;
  TextureVertexArray          m_textureVertexArray;
  Array<Face>                 m_faceArray;
  mutable bool                m_calculateNormals    : 1;
  mutable bool                m_calculateTexture    : 1;
  bool                        m_hasColors           : 1;
  mutable bool                m_vertexNormalChecked : 1;
  mutable bool                m_1NormalPerVertex    : 1;
  mutable bool                m_validated           : 1;
  mutable bool                m_validateOk          : 1;

  void pruneUnused();
  MeshBuilder &reduceVertexArray();
  MeshBuilder &reduceNormalArray();
  void check1NormalPerVertex() const;
  void adjustNegativeVertexIndex( int &v);
  void adjustNegativeNormalIndex( int &n);
  void adjustNegativeTextureIndex(int &t);
public:
  MeshBuilder() {
    clear();
  }
  void clear(UINT capacity = 0);
  inline UINT addVertex(const Vertex &p) {
    const UINT n = (UINT)m_vertices.size();
    m_vertices.add(p);
    return n;
  }
  template<typename TX, typename TY, typename TZ> UINT addVertex(TX x, TY y, TZ z) {
    return addVertex(Vertex(x, y, z));
  }
  inline UINT addVertex(const D3DXVECTOR3 &v) {
    return addVertex(Vertex(v));
  }
  template<typename T> UINT addVertex(const Point3DTemplate<T> &p) {
    return addVertex(Vertex(p));
  }
  inline UINT addNormal(const Vertex &n) {
    const UINT c = (UINT)m_normals.size();
    m_normals.add(n);
    return c;
  }
  template<typename TX, typename TY, typename TZ> UINT addNormal(TX x, TY y, TZ z) {
    return addNormal(Vertex(x, y, z));
  }
  inline UINT addNormal(const D3DXVECTOR3 &v) {
    return addNormal(Vertex(v));
  }
  template<typename T> UINT addNormal(const Point3DTemplate<T> &n) {
    return addNormal(Vertex(n));
  }
  inline UINT addTextureVertex(const TextureVertex &vt) {
    const UINT c = (UINT)m_textureVertexArray.size();
    m_textureVertexArray.add(vt);
    return c;
  }
  inline UINT addTextureVertex(float u, float v) {
    return addTextureVertex(TextureVertex(u,v));
  }
  inline Face &addFace(D3DCOLOR color, size_t capacity=3) {
    m_faceArray.add(Face(color, capacity));
    m_vertexNormalChecked = m_validated = false;
    m_hasColors                         = true;
    return m_faceArray.last();
  }
  inline Face &addFace(size_t capacity=3) {
    m_faceArray.add(Face(-1, capacity));
    m_vertexNormalChecked = m_validated = false;
    return m_faceArray.last();
  }
  MeshBuilder &addSquareFace(int v0, int v1, int v2, int v3);
  // Return true, if normals point in same direction as the visible side of the face
  bool hasCorrectOrientation(const Face &f) const;
  static D3DXVECTOR3 calculateNormal(const D3DXVECTOR3 &v0, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    return unitVector(cross(v2-v0, v1-v0));
  }
  inline D3DXVECTOR3 calculateNormal(int v0, int v1, int v2) const {
    return calculateNormal(m_vertices[v0], m_vertices[v1], m_vertices[v2]);
  }
  inline UINT getVertexCount() const {
    return (UINT)m_vertices.size();
  }
  inline UINT getNormalCount() const {
    return (UINT)m_normals.size();
  }
  inline UINT getTextureCount() const {
    return (UINT)m_textureVertexArray.size();
  }
  UINT getTriangleCount()      const;
  UINT getIndexCount()         const;
  void validate()              const;
  bool isOk()                  const;
  bool has1NormalPerVertex()   const;
  bool isEmpty()               const;
  inline bool calculateNormals() const {
    return m_calculateNormals;
  }
  inline bool calculateTexture() const {
    return m_calculateTexture;
  }
  inline bool hasColors()      const {
    return m_hasColors;
  }
  bool use32BitIndices(bool doubleSided) const;
  inline const Array<Face> &getFaceArray() const {
    return m_faceArray;
  }
  inline const VertexArray &getVertexArray() const {
    return m_vertices;
  }
  inline const VertexArray &getNormalArray() const {
    return m_normals;
  }
  inline const TextureVertexArray &getTextureVertexArray() const {
    return m_textureVertexArray;
  }

  // Return *this;
  MeshBuilder &optimize();
  D3Cube getBoundingBox() const;
  LPD3DXMESH createMesh(AbstractMeshFactory &amf, bool doubleSided) const;
  void parseWavefrontObjFile(FILE *f);
  String toString() const;
};

//#define DUMP_PRUNECOUNT
