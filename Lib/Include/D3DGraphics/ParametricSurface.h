#pragma once

#include <PersistentData.h>
#include <NumberInterval.h>

class ParametricSurfaceParameters : public PersistentData {
public:
  String         m_commonText, m_exprX, m_exprY, m_exprZ;
  DoubleInterval m_tInterval;
  DoubleInterval m_sInterval;
  UINT           m_tStepCount;
  UINT           m_sStepCount;
  DoubleInterval m_timeInterval;
  // number of frames (meshes) that will be generated
  UINT           m_frameCount;
  bool           m_machineCode;
  // if true, an animation will be generated
  bool           m_includeTime;
  bool           m_doubleSided;

  ParametricSurfaceParameters();
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
  const DoubleInterval &getSInterval() const {
    return m_sInterval;
  }
  const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
  int getType() const {
    return PP_PARAMETRICSURFACE;
  }
};

