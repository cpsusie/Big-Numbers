#include "pch.h"
#include <DebugLog.h>

void hexdump(const void *data, size_t size, FILE *f) {
  const String str = createHexdumpString(data, size);
  if(f) {
    _ftprintf(f, _T("%s"), str.cstr());
  } else {
    debugLog(_T("%s"), str.cstr());
  }
}
