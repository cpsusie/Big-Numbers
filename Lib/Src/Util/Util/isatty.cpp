#include "pch.h"
#include <io.h>

bool isatty(FILE *f) {
  return _isatty(_fileno(f)) ? true : false;
}
