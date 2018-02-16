#pragma once

#include <Exception.h>
#include "PragmaLib.h"

class MathException : public Exception {
public:
  MathException(const TCHAR *text) : Exception(text) {};
};

void throwMathException(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
