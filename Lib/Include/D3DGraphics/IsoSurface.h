#pragma once

#include <PersistentData.h>
#include <NumberInterval.h>

class IsoSurfaceParameters : public PersistentData {
public:
  String         m_expr;
  double         m_cellSize;
  Cube3D         m_boundingBox;
  bool           m_tetrahedral;
  bool           m_adaptiveCellSize;
  bool           m_originOutside;
  bool           m_machineCode;
  bool           m_doubleSided;
  bool           m_includeTime;  // if true, an animation will be generated
  DoubleInterval m_timeInterval;
  UINT           m_frameCount;   // number of frames (meshes) that will be generated

  IsoSurfaceParameters();

  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
  int getType() const {
    return PP_ISOSURFACE;
  }
};
