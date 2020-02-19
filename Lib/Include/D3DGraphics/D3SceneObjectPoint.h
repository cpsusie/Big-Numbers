#pragma once

#include "D3Math.h"

class D3SceneObject;

class D3SceneObjectPoint {
public:
  D3SceneObject *m_obj;       // which object does m_pos refer to
  D3DXVECTOR3    m_meshPoint; // relative to mesh (0,0,0)
  D3SceneObjectPoint() {
    reset();
  }
  inline void reset() {
    set(NULL, D3DXORIGIN);
  }
  bool isEmpty() const {
    return m_obj == NULL;
  }
  inline const D3DXVECTOR3 &getMeshPoint() const {
    return m_meshPoint;
  }
  const D3DXVECTOR3 getWorldPoint() const;
  inline void set(D3SceneObject *obj, const D3DXVECTOR3 &meshPoint) {
    m_obj       = obj;
    m_meshPoint = meshPoint;
  }
  String toString() const;
};

