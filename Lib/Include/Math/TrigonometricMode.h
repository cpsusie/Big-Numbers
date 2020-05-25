#pragma once

#include <MyString.h>

typedef enum {
  RADIANS
 ,DEGREES
 ,GRADS
} TrigonometricMode;

class _TrigonometricModeName {
public:
  // throws exception if type not in {RADIANS,DEGREES,GRADS}
  static const TCHAR      *modeToStr(TrigonometricMode mode);
  // throws exception if str not in {"radians","degrees", "grads"} (comparing ignore case)
  static TrigonometricMode strToMode(const TCHAR *str );
};

inline String toString(TrigonometricMode mode) {
  return _TrigonometricModeName::modeToStr(mode);
}
