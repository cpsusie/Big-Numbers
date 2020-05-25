#include "pch.h"
#include <Math/TrigonometricMode.h>

typedef struct {
  const TrigonometricMode m_mode;
  const TCHAR            *m_name;
} ModeName;

static const ModeName s_modeNames[] = {
  RADIANS, _T("Radians")
 ,DEGREES, _T("Degrees")
 ,GRADS  , _T("Grads"  )
};

const TCHAR *_TrigonometricModeName::modeToStr(TrigonometricMode mode) { // static
  for(ModeName m : s_modeNames) {
    if(m.m_mode == mode) {
      return m.m_name;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("mode=%d"), mode);
  return NULL;
}

TrigonometricMode _TrigonometricModeName::strToMode(const TCHAR *str ) { // static
  for(ModeName m : s_modeNames) {
    if(_tcsicmp(m.m_name,str) == 0) {
      return m.m_mode;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("str=%s"), str);
  return RADIANS;
}

