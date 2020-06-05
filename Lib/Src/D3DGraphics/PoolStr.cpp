#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(caseStr)
#undef  caseStr
#endif
#define caseStr(f) case D3DPOOL_##f: return _T(#f);

String poolToString(D3DPOOL pool) {
  switch(pool) {
  caseStr(DEFAULT            )
  caseStr(MANAGED            )
  caseStr(SYSTEMMEM          )
  caseStr(SCRATCH            )
  default: throwInvalidArgumentException(__TFUNCTION__,_T("pool=%d"), pool);
  }
  return EMPTYSTRING;
}

D3DPOOL poolFromString(const String &str) {
  static const D3DPOOL table[] = { D3DPOOL_DEFAULT, D3DPOOL_MANAGED, D3DPOOL_SYSTEMMEM, D3DPOOL_SCRATCH };
  return linearStringSearch(str, table, ARRAYSIZE(table), poolToString);
}
