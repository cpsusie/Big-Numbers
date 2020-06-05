#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(caseStr)
#undef  caseStr
#endif
#define caseStr(f) case D3DFILL_##f: return _T(#f);

String fillModeToString(D3DFILLMODE mode) {
  switch(mode) {
  caseStr(POINT    )
  caseStr(WIREFRAME)
  caseStr(SOLID    )
  default: throwInvalidArgumentException(__TFUNCTION__,_T("mode=%d"),mode);
  }
  return EMPTYSTRING;
}

D3DFILLMODE fillModeFromString(const String &str) {
  static const D3DFILLMODE table[] = { D3DFILL_POINT, D3DFILL_WIREFRAME, D3DFILL_SOLID };
  return linearStringSearch(str, table, ARRAYSIZE(table), fillModeToString);
}
