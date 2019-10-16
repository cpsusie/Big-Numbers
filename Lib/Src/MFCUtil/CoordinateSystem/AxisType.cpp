#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/Coordinatesystem/AxisType.h>

IntervalScale getScale(AxisType type) {
  switch(type) {
  case AXIS_LINEAR             : return LINEAR;
  case AXIS_NORMAL_DISTRIBUTION: return NORMAL_DISTRIBUTION;
  case AXIS_LOGARITHMIC        : return LOGARITHMIC;
  case AXIS_DATE               : return LINEAR;
  default                      : throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"),type);
                                 return LINEAR;
  }
}

AxisAttribute::AxisAttribute() {
  m_type  = AXIS_LINEAR;
  m_color = BLACK;
  m_flags = AXIS_SHOW_VALUES | AXIS_SHOW_VALUEMARKS;
}
