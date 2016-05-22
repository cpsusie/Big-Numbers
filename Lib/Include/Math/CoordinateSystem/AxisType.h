#pragma once

#include <Math/Transformation.h>

typedef enum {
  AXIS_LINEAR
 ,AXIS_LOGARITHMIC
 ,AXIS_NORMAL_DISTRIBUTION
 ,AXIS_DATE
} AxisType;

IntervalScale getScale(AxisType type);
