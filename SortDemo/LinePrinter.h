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
  virtual void vprintf(const TCHAR *format, va_list argptr) = 0;
  virtual void terminate();
  void printf(const TCHAR *format,...);
  static LinePrinter *getNullPrinter();
};
