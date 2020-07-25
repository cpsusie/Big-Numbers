#pragma once

#include <Math/Rectangle2D.h>
#include "FunctionImageParamPersistentData.h"
#include "AnimationParameters.h"

class IsoCurveParameters : public FunctionImageParamPersistentData {
private:
  void getDataFromDocOld(XMLDoc &doc);

public:
  String              m_expr;
  double              m_cellSize;
  Rectangle2D         m_boundingBox;
  Point2D             m_startPoint;
  bool                m_machineCode;
  AnimationParameters m_animation;

  IsoCurveParameters();

  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  inline bool isAnimated() const {
    return m_animation.includeTime();
  }
  FunctionDomainRangeDimension getType() const {
    return TYPE_ISOCURVE;
  }
};

