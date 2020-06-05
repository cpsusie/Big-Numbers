#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(caseStr)
#undef  caseStr
#endif
#define caseStr(f) case D3DLIGHT_##f: return _T(#f);

String lightTypeToString(D3DLIGHTTYPE type) {
  switch(type) {
  caseStr(POINT      )
  caseStr(SPOT       )
  caseStr(DIRECTIONAL)
  default: throwInvalidArgumentException(__TFUNCTION__,_T("type=%d"),type);
  }
  return EMPTYSTRING;
}

D3DLIGHTTYPE lightTypeFromString(const String &str) {
  static const D3DLIGHTTYPE table[] = { D3DLIGHT_POINT, D3DLIGHT_SPOT,D3DLIGHT_DIRECTIONAL };
  return linearStringSearch(str, table, ARRAYSIZE(table), lightTypeToString);
}
