#include "stdafx.h"
#include "LinePrinter.h"

LinePrinter::LinePrinter() {
}

void LinePrinter::printf(const TCHAR *format, ...) {
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
  void vprintf(const TCHAR *format, va_list argptr) {}
};

LinePrinter *LinePrinter::getNullPrinter() { // static
  static NullPrinter np;
  return &np;
}
