#pragma once

#include "D3DeviceFactory.h"

class VertexNormalTextureIndex {
public:
  UINT m_vIndex, m_nIndex, m_tIndex;
  inline VertexNormalTextureIndex() {}
  inline VertexNormalTextureIndex(UINT vIndex, UINT nIndex, UINT tIndex)
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
};

class Face {
private:
  CompactArray<VertexNormalTextureIndex> m_data;
  D3DCOLOR                               m_diffuseColor;
public:
  Face(D3DCOLOR diffuseColor) { 
    m_diffuseColor = diffuseColor;
  }
  inline void addVertexAndNormalIndex(UINT vIndex, UINT nIndex, UINT tIndex) {
    m_data.add(VertexNormalTextureIndex(vIndex, nIndex, tIndex));
  }
  inline int getTriangleCount() const {
    return (int)m_data.size() - 2;
  }
  inline UINT getIndexCount() const {
    return (UINT)m_data.size();
  }
  inline const CompactArray<VertexNormalTextureIndex> &getIndices() const {
    return m_data;
  }
  inline int getDiffuseColor() const {
    return m_diffuseColor;
  }
  void invertOrientation();
  friend class MeshBuilder;
};

#define MAX16BITVERTEXCOUNT 0xfffe

class MeshBuilder {
private:
  VertexArray                 m_vertices;
  VertexArray                 m_normals;
  CompactArray<TextureVertex> m_textureVertexArray;
  Array<Face>                 m_faceArray;
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
    return (int)m_textureVertexArray.size();
  }
  inline Face &addFace(D3DCOLOR color) {
    m_faceArray.add(Face(color));
    m_vertexNormalChecked = m_validated = false;
    m_hasColors                         = true;
    return m_faceArray.last();
  }
  inline Face &addFace() {
    m_faceArray.add(Face(-1));
    m_vertexNormalChecked = m_validated = false;
    return m_faceArray.last();
  }
  inline int getNormalCount() const {
    return (int)m_normals.size();
  }
  int getTriangleCount()       const;
  int getIndexCount()          const;
  void validate()              const;
  bool isOk()                  const;
  bool has1NormalPerVertex()   const;
  bool isEmpty()               const;
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
  inline const CompactArray<TextureVertex> &getTextureVertexArray() const {
    return m_textureVertexArray;
  }
/*
  void optimize();
*/

  LPD3DXMESH createMesh(DIRECT3DDEVICE device, bool doubleSided) const;
  void parseWavefrontObjFile(FILE *f);
  void dump(const String &fileName = "") const;
};

#define DUMP_PRUNECOUNT
