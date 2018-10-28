#pragma once

#include <PersistentData.h>
#include <NumberInterval.h>
#include <D3DGraphics/Cube3D.h>

class IsoSurfaceParameters : public PersistentData {
public:
  String         m_expr;
  double         m_cellSize;
  Cube3D         m_boundingBox;
  bool           m_tetrahedral;
  bool           m_tetraOptimize4;
  bool           m_adaptiveCellSize;
  bool           m_originOutside;
  bool           m_machineCode;
  bool           m_doubleSided;
  // if true, an animation will be generated
  bool           m_includeTime;
  DoubleInterval m_timeInterval;
  // number of frames (meshes) that will be generated
  UINT           m_frameCount;

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
