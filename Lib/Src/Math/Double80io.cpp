#include "pch.h"
#include "Double80IO.h"

namespace Double80IO {

D80StringFields::D80StringFields(const Double80 &x)
  : FloatStringFields(2) {
  TCHAR tmp[50];
#if defined(_DEBUG)
  memset(tmp, 0, sizeof(tmp));
#endif
  init(d80tot(tmp, x));
}

};
