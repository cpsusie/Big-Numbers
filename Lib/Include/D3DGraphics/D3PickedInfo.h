#pragma once

#include "D3Math.h"

// Informarmation about where a ray intersects a mesh
class D3PickedInfo {
private:
  // Index of face.
  int           m_faceIndex;
  // Indices into vertexArray
  int           m_vertexIndex[3];
  // TextureVertex. Relative coordinates in picked face
  D3DXVECTOR3   m_facePoint[3];
  float         m_U, m_V;
public:
  inline D3PickedInfo() {
    clear();
  }
  inline D3PickedInfo(int faceIndex, int vertexIndex[3], D3DXVECTOR3 facePoint[3], float U, float V)
    : m_faceIndex(faceIndex)
    , m_U(U), m_V(V)
  {
    for (int i = 0; i < 3; i++) {
      m_vertexIndex[i] = vertexIndex[i];
      m_facePoint[i] = facePoint[i];
    }
  }
  inline void clear() {
    memset(this, 0, sizeof(D3PickedInfo));
    m_faceIndex = -1;
  }
  inline bool isEmpty() const {
    return m_faceIndex < 0;
  }
  inline int getFaceIndex() const {
    return m_faceIndex;
  }
  inline TextureVertex getTextureVertex() const {
    return TextureVertex(m_U, m_V);
  }
  inline D3DXVECTOR3 getMeshPoint() const {
    return m_facePoint[0] + m_U * (m_facePoint[1] - m_facePoint[0]) + m_V * (m_facePoint[2] - m_facePoint[0]);
  }
  // i = [0..2];
  inline int getVertexIndex(int i) const {
    return m_vertexIndex[i];
  }
  // i = [0..2];
  inline const D3DXVECTOR3 &getVertexPoint(int i) const {
    return m_facePoint[i];
  }
  String toString(int dec = 3) const;
};

