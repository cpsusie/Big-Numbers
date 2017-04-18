#pragma once

#include <NumberInterval.h>
#include <PersistentParameter.h>
#include <Math/Rectangle2D.h>

class IsoCurveParameters : public PersistentParameter {
public:
  String         m_expr;
  double         m_cellSize;
  Rectangle2D    m_boundingBox;
  Point2D        m_startPoint;
  bool           m_machineCode;
  bool           m_includeTime; // if true, an animation will be generated
  DoubleInterval m_tInterval;
  UINT           m_frameCount;  // number of frames (meshes) that will be generated

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

