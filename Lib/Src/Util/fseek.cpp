#include "pch.h"
#include <MyUtil.h>

void FSEEK(FILE *f, INT64 offset) {
  if(fsetpos(f, &offset) != 0 ) {
    throwException(_T("%s:fsetpos(%I64d) failed:%s. FileSize=%I64d")
                  ,__TFUNCTION__
                  ,offset
                  ,getErrnoText().cstr()
                  ,GETSIZE(f)
                  );
  }
}
