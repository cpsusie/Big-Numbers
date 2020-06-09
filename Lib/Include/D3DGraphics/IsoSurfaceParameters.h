#pragma once

#include <D3DGraphics/D3SurfaceCommonParameters.h>
#include <D3DGraphics/D3Cube.h>

class IsoSurfaceParameters : public D3SurfaceCommonParameters {
public:
  double              m_cellSize;
  double              m_lambda;
  Cube3D              m_boundingBox;
  bool                m_tetrahedral;
  bool                m_tetraOptimize4;
  bool                m_adaptiveCellSize;
  bool                m_originOutside;
  IsoSurfaceParameters();
};
