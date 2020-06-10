#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(caseStr)
#undef  caseStr
#endif
#if defined(NV)
#undef NV
#endif
#define caseStr(v) case D3DRTYPE_##v: return _T(#v);
#define NV(v)      D3DRTYPE_##v, _T(#v)

String resourceTypeToString(D3DRESOURCETYPE type) {
  switch(type) {
  caseStr(SURFACE            )
  caseStr(VOLUME             )
  caseStr(TEXTURE            )
  caseStr(VOLUMETEXTURE      )
  caseStr(CUBETEXTURE        )
  caseStr(VERTEXBUFFER       )
  caseStr(INDEXBUFFER        )
  default: throwInvalidArgumentException(__TFUNCTION__,_T("type=%d"), type);
  }
  return EMPTYSTRING;
}

D3DRESOURCETYPE resourceTypeFromString(const String &str) {
  static const D3DRESOURCETYPE table[] = {
    D3DRTYPE_VERTEXBUFFER, D3DRTYPE_INDEXBUFFER
   ,D3DRTYPE_TEXTURE     , D3DRTYPE_SURFACE
   ,D3DRTYPE_VOLUME      , D3DRTYPE_VOLUMETEXTURE
   ,D3DRTYPE_CUBETEXTURE
  };
  return linearStringSearch(str, table, ARRAYSIZE(table), resourceTypeToString);
}
