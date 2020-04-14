#pragma once

#include "D3Math.h"

class D3SceneObjectVisual;

// Information about where a ray intersects a mesh
class D3PickedInfo {
private:
  // Visual object containing the mesh which intersect the ray
  const D3SceneObjectVisual *m_visualObject;
  // Index of face.
  int                        m_faceIndex;
  // Indices into vertexArray
  int                        m_vertexIndex[3];
  // hitpoint In world space
  D3DXVECTOR3                m_intersectionPoint;
  // TextureVertex. Relative coordinates in picked face
  D3DXVECTOR3                m_facePoint[3];
  float                      m_U, m_V;
public:
  inline D3PickedInfo() {
    clear();
  }
  D3PickedInfo(const D3SceneObjectVisual *visualObject, int faceIndex, int vertexIndex[3], D3DXVECTOR3 facePoint[3], float U, float V);
  void clear();
  inline bool isEmpty() const {
    return m_visualObject == NULL;
  }
  const D3SceneObjectVisual *getVisual() const {
    return m_visualObject;
  }
  inline int getFaceIndex() const {
    return m_faceIndex;
  }
  inline TextureVertex getTextureVertex() const {
    return TextureVertex(m_U, m_V);
  }
  D3DXVECTOR3 getMeshPoint() const;
  inline const D3DXVECTOR3 &getWorldPoint() const {
    return m_intersectionPoint;
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
