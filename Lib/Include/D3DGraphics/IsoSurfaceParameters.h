#pragma once

#include <D3DGraphics/MeshAnimationData.h>
#include <D3DGraphics/D3Cube.h>

class IsoSurfaceParameters {
public:
  double            m_cellSize;
  double            m_lambda;
  Cube3D            m_boundingBox;
  bool              m_tetrahedral;
  bool              m_tetraOptimize4;
  bool              m_adaptiveCellSize;
  bool              m_originOutside;
  bool              m_doubleSided;
  MeshAnimationData m_animation;
  IsoSurfaceParameters();
  inline bool isAnimated() const {
    return m_animation.includeTime();
  }

};
