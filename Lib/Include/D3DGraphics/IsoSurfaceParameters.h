#pragma once

#include <NumberInterval.h>
#include <D3DGraphics/D3Cube.h>

class IsoSurfaceParameters {
public:
  double         m_cellSize;
  double         m_lambda;
  Cube3D         m_boundingBox;
  bool           m_tetrahedral;
  bool           m_tetraOptimize4;
  bool           m_adaptiveCellSize;
  bool           m_originOutside;
  bool           m_doubleSided;
  // if true, an animation will be generated
  bool           m_includeTime;
  DoubleInterval m_timeInterval;
  // number of frames (meshes) that will be generated
  UINT           m_frameCount;

  IsoSurfaceParameters();

  const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
};
