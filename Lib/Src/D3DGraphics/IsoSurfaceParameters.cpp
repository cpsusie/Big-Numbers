#include "pch.h"
#include "D3DGraphics/IsoSurfaceParameters.h"

IsoSurfaceParameters::IsoSurfaceParameters() {
  m_cellSize         = 0.25;
  m_lambda           = 0.25;
  m_boundingBox      = Cube3D(Point3D(-5,-5,-5), Point3D( 5, 5, 5));
  m_tetrahedral      = true;
  m_tetraOptimize4   = false;
  m_showStatistics   = false;
  m_originOutside    = false;
}
