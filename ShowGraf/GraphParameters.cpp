#include "stdafx.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style) {
  m_name     = name;
  m_color    = color;
  m_rollSize = rollSize;
  m_style    = style;
};

void GraphParameters::setName(const String &name) {
  m_name = name;
}

String GraphParameters::getPartialName() const {
  return FileNameSplitter(m_name).getFileName();
}

static const TCHAR *styleName[] = {
  _T("Curve")
 ,_T("Point")
 ,_T("Cross")
};

String GraphParameters::graphStyleToString(GraphStyle style) {  // static
  if(style < 0 || style > 2) {
    throwInvalidArgumentException(_T("getStyleName"), _T("style=%d. Must be [0..2]"), style);
  }
  return styleName[style];
}

GraphStyle GraphParameters::graphStyleFromString(const String &s) {  // static
  for(int i = 0; i < ARRAYSIZE(styleName); i++) {
    if(s.equalsIgnoreCase(styleName[i])) {
      return (GraphStyle)i;
    }
  }
  return GSCURVE;
}

String GraphParameters::trigonometricModeToString(TrigonometricMode mode) {  // static
  switch(mode) {
  case RADIANS : return _T("RADIANS");
  case DEGREES : return _T("DEGREES");
  case GRADS   : return _T("GRADS");
  default      : return _T("RADIANS");
  }
}

TrigonometricMode GraphParameters::trigonometricModeFromString(const String &str) { // static
  if(str.equalsIgnoreCase(_T("radians"))) {
    return RADIANS;
  } else if(str.equalsIgnoreCase(_T("DEGREES"))) {
    return DEGREES;
  } else if(str.equalsIgnoreCase(_T("GRADS"))) {
    return GRADS;
  } else {
    return RADIANS;
  }
}
