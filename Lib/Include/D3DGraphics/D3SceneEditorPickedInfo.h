#pragma once

#include "D3PickedInfo.h"

class D3SceneEditorPickedInfo {
public:
  D3PickedInfo m_info;
  D3DXVECTOR3  m_hitPoint; // in world space
  float        m_dist;
  D3SceneEditorPickedInfo() {
    clear();
  }
  void clear();
  String toString() const;
};
