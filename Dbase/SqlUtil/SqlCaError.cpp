#include "stdafx.h"

sqlca::sqlca(long error, const TCHAR *format, ...) { 
  va_list argptr;
  va_start(argptr, format);
  vseterror(error, format, argptr);
  va_end(argptr);
}

void sqlca::init(long error) {
  memset(this, 0, sizeof(sqlca));
  sqlcode = error;
}

void sqlca::vseterror(long error, const TCHAR *format, va_list argptr) {
  init(error); 
  String errmsg = vformat(format, argptr);
  _tcsncpy(sqlerrmc, errmsg.cstr(),ARRAYSIZE(sqlerrmc)-1);
  sqlerrmc[ARRAYSIZE(sqlerrmc)-1] = '\0';
}

void sqlca::seterror(long error, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vseterror(error, format, argptr);
  va_end(argptr);
}

void sqlca::dump(FILE *f) {
  _ftprintf(f,_T("sqlca.code:%ld\n")     , sqlcode );
  _ftprintf(f,_T("sqlca.sqlerrmc:<%s>\n"), sqlerrmc);
}

void throwSqlError(long error, const TCHAR *format,...) {
  sqlca ca;
  va_list argptr;
  va_start(argptr, format);
  ca.vseterror(error, format, argptr);
  va_end(argptr);
  throw ca;
}

