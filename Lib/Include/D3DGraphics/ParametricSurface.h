#pragma once

#pragma once

#include <PersistentParameter.h>

class ParametricSurfaceParameters : public PersistentParameter {
public:
  String         m_exprX, m_exprY, m_exprZ;
  DoubleInterval m_tInterval;
  DoubleInterval m_sInterval;
  UINT           m_tStepCount;
  UINT           m_sStepCount;
  DoubleInterval m_timeInterval;
  UINT           m_frameCount;   // number of frames (meshes) that will be generated
  bool           m_machineCode;
  bool           m_includeTime; // if true, an animation will be generated
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
  PersistentParameterType getType() const {
    return PP_PARAMETRICSURFACE;
  }
};

