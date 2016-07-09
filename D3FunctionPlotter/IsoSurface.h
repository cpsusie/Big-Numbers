#pragma once

#include <NumberInterval.h>
#include "Cube3D.h"
#include "PersistentParameter.h"

class IsoSurfaceParameters : public PersistentParameter {
public:
  String         m_expr;
  double         m_size;
  Cube3D         m_boundingBox;
  bool           m_tetrahedral;
  bool           m_adaptiveCellSize;
  bool           m_originOutside;
  bool           m_machineCode;
  bool           m_doubleSided;
  bool           m_includeTime; // if true, an animation will be generated
  DoubleInterval m_tInterval;
  unsigned int   m_timeCount;   // number of frames (meshes) that will be generated

  IsoSurfaceParameters();

  void read( FILE *f);
  void write(FILE *f);
  const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
  PersistentParameterType getType() const {
    return PP_ISOSURFACE;
  }
};
