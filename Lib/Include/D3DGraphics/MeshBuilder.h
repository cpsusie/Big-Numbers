#pragma once

#include <D3DGraphics/D3AbstractMeshFactory.h>
#include <D3DGraphics/D3Math.h>
#include <D3DGraphics/D3DXCube.h>

class VertexNormalTextureIndex {
public:
  int m_vIndex, m_nIndex, m_tIndex;
  inline VertexNormalTextureIndex() {}
  inline VertexNormalTextureIndex(int vIndex, int nIndex, int tIndex)
    : m_vIndex(vIndex), m_nIndex(nIndex), m_tIndex(tIndex)
  {
  }
  inline bool operator==(const VertexNormalTextureIndex &v) const {
    return (m_vIndex == v.m_vIndex)
        && (m_nIndex == v.m_nIndex)
        && (m_tIndex == v.m_tIndex);
  }
  inline ULONG hashCode() const {
    return (m_vIndex * 100981 + m_nIndex) * 997 + m_tIndex;
  }
  inline String toString() const {
    return format(_T("VertexNormalTextureIndex(v:%4d,n:%4d,t:%4d"), m_vIndex, m_nIndex, m_tIndex);
  }
};

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
  inline int getTriangleCount() const {
    return (int)m_data.size() - 2;
  }
  inline UINT getIndexCount() const {
    return (UINT)m_data.size();
  }
  inline bool isEmpty() const {
    return m_data.isEmpty();
  }
  inline const VNTIArray &getIndices() const {
    return m_data;
  }
  inline int getDiffuseColor() const {
    return m_diffuseColor;
  }
  void invertOrientation();
  friend class MeshBuilder;
  inline String toString() const {
    return format(_T("Face(Color:%#8x, IndexCount:%zu\nIndexArray:%s\n")
                 ,m_diffuseColor, m_data.size(), m_data.toString().cstr());
  }
};

#define MAX16BITVERTEXCOUNT 0xfffe

class MeshBuilder {
private:
  VertexArray                 m_vertices;
  VertexArray                 m_normals;
  TextureVertexArray          m_textureVertexArray;
  Array<Face>                 m_faceArray;
  mutable bool                m_hasNormals          : 1;
  mutable bool                m_hasTextures         : 1;
  bool                        m_hasColors           : 1;
  mutable bool                m_vertexNormalChecked : 1;
  mutable bool                m_1NormalPerVertex    : 1;
  mutable bool                m_validated           : 1;
  mutable bool                m_validateOk          : 1;

  void pruneUnused();
  void check1NormalPerVertex() const;
  void adjustNegativeVertexIndex( int &v);
  void adjustNegativeNormalIndex( int &n);
  void adjustNegativeTextureIndex(int &t);
public:
  MeshBuilder() {
    clear();
  }
  void clear(UINT capacity = 0);
  inline int addVertex(float x, float y, float z) {
    m_vertices.add(Vertex(x, y, z));
    return (int)m_vertices.size()-1;
  }
  inline int addVertex(const Vertex &p) {
    m_vertices.add(p);
    return (int)m_vertices.size()-1;
  }
  inline int addVertex(const D3DXVECTOR3 &v) {
    m_vertices.add(Vertex(v));
    return (int)m_vertices.size()-1;
  }
  inline int addVertex(const Point3D &p) {
    m_vertices.add(Vertex(p));
    return (int)m_vertices.size()-1;
  }
  inline int addNormal(float x, float y, float z) {
    m_normals.add(Vertex(x, y, z));
    return (int)m_normals.size()-1;
  }
  inline int addNormal(const Vertex &n) {
    m_normals.add(n);
    return (int)m_normals.size()-1;
  }
  inline int addNormal(const D3DXVECTOR3 &v) {
    m_normals.add(Vertex(v));
    return (int)m_normals.size()-1;
  }
  inline int addNormal(const Point3D &n) {
    m_normals.add(Vertex(n));
    return (int)m_normals.size()-1;
  }
  inline int addTextureVertex(const TextureVertex &vt) {
    m_textureVertexArray.add(vt);
    return (int)m_textureVertexArray.size()-1;
  }
  inline int addTextureVertex(float u, float v) {
    m_textureVertexArray.add(TextureVertex(u,v));
    return (int)m_textureVertexArray.size()-1;
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
  static D3DXVECTOR3 calculateNormal(const D3DXVECTOR3 &v0, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
    return unitVector(cross(v2-v0, v1-v0));
  }
  inline D3DXVECTOR3 calculateNormal(int v0, int v1, int v2) const {
    return calculateNormal(m_vertices[v0], m_vertices[v1], m_vertices[v2]);
  }
  inline int getVertexCount() const {
    return (int)m_vertices.size();
  }
  inline int getNormalCount() const {
    return (int)m_normals.size();
  }
  inline int getTextureCount() const {
    return (int)m_textureVertexArray.size();
  }
  int getTriangleCount()       const;
  int getIndexCount()          const;
  void validate()              const;
  bool isOk()                  const;
  bool has1NormalPerVertex()   const;
  bool isEmpty()               const;
  inline bool hasNormals() const {
    return m_hasNormals;
  }
  inline bool hasTextureVertices() const {
    return m_hasTextures;
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

  void optimize();
  D3DXCube3 getBoundingBox() const;
  LPD3DXMESH createMesh(AbstractMeshFactory &amf, bool doubleSided) const;
  void parseWavefrontObjFile(FILE *f);
  String toString() const;
};

//#define DUMP_PRUNECOUNT
