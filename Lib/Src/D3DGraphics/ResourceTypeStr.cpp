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

static const NameValue<D3DRESOURCETYPE> typeNames[] = {
  NV(SURFACE            )
 ,NV(VOLUME             )
 ,NV(TEXTURE            )
 ,NV(VOLUMETEXTURE      )
 ,NV(CUBETEXTURE        )
 ,NV(VERTEXBUFFER       )
 ,NV(INDEXBUFFER        )
};

DefineNameLookupClass(D3DRESOURCETYPE, typeNames,Type);

D3DRESOURCETYPE resourceTypeFromString(const String &str) {
  return TypeLookupTable::getInstance().lookupName(str);
}
