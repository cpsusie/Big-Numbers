#pragma once

#include <NumberInterval.h>
#include "Cube3D.h"
#include "PersistentParameter.h"

class IsoSurfaceParameters : public PersistentParameter {
public:
  String         m_expr;
  double         m_cellSize;
  Cube3D         m_boundingBox;
  bool           m_tetrahedral;
  bool           m_adaptiveCellSize;
  bool           m_originOutside;
  bool           m_machineCode;
  bool           m_doubleSided;
  bool           m_includeTime; // if true, an animation will be generated
  DoubleInterval m_timeInterval;
  UINT           m_frameCount;   // number of frames (meshes) that will be generated

  IsoSurfaceParameters();

  void read( FILE *f);
  void write(FILE *f);
  const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
  PersistentParameterType getType() const {
    return PP_ISOSURFACE;
  }
};
