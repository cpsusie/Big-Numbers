#pragma once

#include <PropertyContainer.h>

typedef enum {
  LINEPRINTER_VISIBLE
 ,LINEPRINTER_TERMINATED
} LinePrinterProperty;

class LinePrinter : public PropertyContainer {
public:
  LinePrinter();

  virtual void setTitle(const String &title)               = 0;
  virtual void clear()                                     = 0;
  virtual void vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) = 0;
  virtual void terminate();
  void printf(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  static LinePrinter *getNullPrinter();
};
