#pragma once

#include <Exception.h>
#include "PragmaLib.h"

class MathException : public Exception {
public:
  MathException(const TCHAR *text) : Exception(text) {};
};

void throwMathException(const TCHAR *format,...);
