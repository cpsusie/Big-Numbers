#include "pch.h"

static void invalidTrigonometricMode(const TCHAR *function, TrigonometricMode mode) {
  throwInvalidArgumentException(function, _T("Invalid trigonometricMode:%d"), mode);
}

Real sin(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return sin(x);
  case DEGREES: return sinDegrees(x);
  case GRADS  : return sinGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real cos(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return cos(x);
  case DEGREES: return cosDegrees(x);
  case GRADS  : return cosGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real tan(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return tan(x);
  case DEGREES: return tanDegrees(x);
  case GRADS  : return tanGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real cot(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return cot(x);
  case DEGREES: return cotDegrees(x);
  case GRADS  : return cotGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real asin(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return asin(x);
  case DEGREES: return asinDegrees(x);
  case GRADS  : return asinGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real acos(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return acos(x);
  case DEGREES: return acosDegrees(x);
  case GRADS  : return acosGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real atan(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return atan(x);
  case DEGREES: return atanDegrees(x);
  case GRADS  : return atanGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real atan2(const Real &y, const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return atan2(y,x);
  case DEGREES: return atan2Degrees(y,x);
  case GRADS  : return atan2Grads(y,x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real acot(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return acot(x);
  case DEGREES: return acotDegrees(x);
  case GRADS  : return acotGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real csc(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return csc(x);
  case DEGREES: return cscDegrees(x);
  case GRADS  : return cscGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real sec(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return sec(x);
  case DEGREES: return secDegrees(x);
  case GRADS  : return secGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real acsc(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return acsc(x);
  case DEGREES: return acscDegrees(x);
  case GRADS  : return acscGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

Real asec(const Real &x, TrigonometricMode mode) {
  DEFINEMETHODNAME;
  switch(mode) {
  case RADIANS: return asec(x);
  case DEGREES: return asecDegrees(x);
  case GRADS  : return asecGrads(x);
  default     : invalidTrigonometricMode(method, mode);
                return 0;
  }
}

String toString(TrigonometricMode mode) {
  switch(mode) {
  case RADIANS: return _T("Radians");
  case DEGREES: return _T("Degrees");
  case GRADS  : return _T("Grads"  );
  default     : return _T("Radians");
  }
}

TrigonometricMode getTrigonometricModeFromString(const String &str) {
  if(str.equalsIgnoreCase(_T("radians"))) {
    return RADIANS;
  } else if(str.equalsIgnoreCase(_T("degrees"))) {
    return DEGREES;
  } else if(str.equalsIgnoreCase(_T("grads"))) {
    return GRADS;
  } else {
    return RADIANS;
  }
}

