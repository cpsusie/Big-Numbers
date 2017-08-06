#pragma once

#include <NumberInterval.h>
#include <PersistentData.h>
#include <Math/Rectangle2D.h>

class IsoCurveParameters : public PersistentData {
public:
  String         m_expr;
  double         m_cellSize;
  Rectangle2D    m_boundingBox;
  Point2D        m_startPoint;
  bool           m_machineCode;
  // if true, an animation will be generated
  bool           m_includeTime;
  DoubleInterval m_tInterval;
  // Number of frames (meshes) that will be generated
  UINT           m_frameCount;

  IsoCurveParameters();

  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
  int getType() const {
    return PP_ISOCURVE;
  }
};

