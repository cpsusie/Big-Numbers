#include "stdafx.h"
#include <MFCUtil/Coordinatesystem/AxisType.h>

IntervalScale getScale(AxisType type) {
  switch(type) {
  case AXIS_LINEAR             : return LINEAR;
  case AXIS_NORMAL_DISTRIBUTION: return NORMAL_DISTRIBUTION;
  case AXIS_LOGARITHMIC        : return LOGARITHMIC;
  case AXIS_DATE               : return LINEAR;
  default                      : throwException(_T("getScale:Invalid axistype (=%d)."),type);
                                 return LINEAR;
  }
}
