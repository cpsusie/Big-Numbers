#include "stdafx.h"
#include "LinePrinter.h"

LinePrinter::LinePrinter() {
}

void LinePrinter::printf(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(format,argptr);
  va_end(argptr);
}

void LinePrinter::terminate() {
}

class NullPrinter : public LinePrinter {
public:
  void setTitle(const String &title) {}
  void clear() {};
  void vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {}
};

LinePrinter *LinePrinter::getNullPrinter() { // static
  static NullPrinter np;
  return &np;
}
