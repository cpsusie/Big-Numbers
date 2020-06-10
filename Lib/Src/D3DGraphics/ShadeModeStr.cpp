#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(caseStr)
#undef  caseStr
#endif
#define caseStr(f) case D3DSHADE_##f: return _T(#f);

String shadeModeToString(D3DSHADEMODE mode) {
  switch(mode) {
  caseStr(FLAT    )
  caseStr(GOURAUD )
  caseStr(PHONG   )
  default: throwInvalidArgumentException(__TFUNCTION__,_T("mode=%d"),mode);
  }
  return EMPTYSTRING;
}

D3DSHADEMODE shadeModeFromString(const String &str) {
  static const D3DSHADEMODE table[] = { D3DSHADE_FLAT, D3DSHADE_GOURAUD, D3DSHADE_PHONG   };
  return linearStringSearch(str, table, ARRAYSIZE(table), shadeModeToString);
}
