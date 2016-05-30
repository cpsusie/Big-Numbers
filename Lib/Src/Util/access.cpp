#include "pch.h"
#include <io.h>

int ACCESS(const String &name, int mode) {
  return _taccess(name.cstr(), mode);
}
