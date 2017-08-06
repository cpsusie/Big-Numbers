#pragma once

#include <PersistentData.h>
#include <NumberInterval.h>

class Function2DSurfaceParameters : public PersistentData {
public:
  String         m_expr;
  DoubleInterval m_xInterval;
  DoubleInterval m_yInterval;
  DoubleInterval m_timeInterval;
  UINT           m_pointCount;
  // number of frames (meshes) that will be generated
  UINT           m_frameCount;
  bool           m_machineCode;
  // if true, an animation will be generated
  bool           m_includeTime;
  bool           m_doubleSided;

  Function2DSurfaceParameters();
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  const DoubleInterval &getXInterval() const {
    return m_xInterval;
  }
  const DoubleInterval &getYInterval() const {
    return m_yInterval;
  }
  const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
  int getType() const {
    return PP_2DFUNCTION;
  }
};

