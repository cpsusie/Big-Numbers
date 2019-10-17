#include "pch.h"
#include <DebugLog.h>

#define LINESIZE 16

String hexdumpString(const void *data, size_t size) {
  const String LINESTRING = (spaceString(60, _T('-')) + _T("\n"));
  String       result     = format(_T("hexdump(size=%zu)\n"), size);
  const BYTE  *p          = (const BYTE*)data;
  result += LINESTRING;
  for (size_t i = 0; i < size; i += LINESIZE) {
    result += format(_T("%p:"), p + i);
    size_t j;
    for (j = i; j < i + LINESIZE && j < size; j++) {
      result += format(_T("%02x "), p[j]);
    }
    for (; j < i + LINESIZE; j++) {
      result += _T("   ");
    }
    result += _T("      ");
    for (j = i; j < i + LINESIZE && j < size; j++) {
      result += format(_T("%c "), isprint(p[j]) ? p[j] : '.');
    }
    result += _T("\n");
  }
  result += LINESTRING;
  return result;
}