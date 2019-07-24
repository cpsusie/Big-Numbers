#include "pch.h"
#include <MyUtil.h>

void FSEEK(FILE *f, __int64 offset) {
  if(fsetpos(f, &offset) != 0 ) {
    throwException(_T("FSEEK:fsetpos(%I64d) failed:%s"), offset, getErrnoText().cstr());
  }
}
