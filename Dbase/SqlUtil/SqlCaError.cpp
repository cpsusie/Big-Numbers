#include "stdafx.h"

sqlca::sqlca(long error, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vseterror(error, format, argptr);
  va_end(argptr);
}

void sqlca::init(long error) {
  memset(this, 0, sizeof(sqlca));
  sqlcode = error;
}

void sqlca::vseterror(long error, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  init(error);
  String errmsg = vformat(format, argptr);
  _tcsncpy(sqlerrmc, errmsg.cstr(),ARRAYSIZE(sqlerrmc)-1);
  sqlerrmc[ARRAYSIZE(sqlerrmc)-1] = '\0';
}

void sqlca::seterror(long error, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vseterror(error, format, argptr);
  va_end(argptr);
}

void sqlca::dump(FILE *f) {
  _ftprintf(f,_T("%s"), toString().cstr());
}

String sqlca::toString() const {
  return format(_T("sqlca.code    :%ld\n"
                   "sqlca.sqlerrmc:<%s>\n")
               ,sqlcode, sqlerrmc);
}

void throwSqlError(long error, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  sqlca ca;
  va_list argptr;
  va_start(argptr, format);
  ca.vseterror(error, format, argptr);
  va_end(argptr);
  throw ca;
}
