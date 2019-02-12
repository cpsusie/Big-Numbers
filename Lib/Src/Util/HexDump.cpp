#include "pch.h"

#define LINESIZE 16

#define LINESTRING()  (spaceString(60, _T('-'))+_T("\n"))

String hexdumpString(const void *data, size_t size) {
  String result = format(_T("hexdump(size=%zu)\n"), size);
  const BYTE *p = (const BYTE*)data;
  result += LINESTRING();
  for(size_t i = 0; i < size; i += LINESIZE) {
    result += format(_T("%p:"),p+i);
    size_t j;
    for(j = i; j < i+LINESIZE && j < size; j++) {
      result += format(_T("%02x "),p[j]);
    }
    for(; j < i+LINESIZE; j++) {
      result += _T("   ");
    }
    result += _T("      ");
    for(j = i; j < i+LINESIZE && j < size; j++) {
      result += format(_T("%c "), isprint(p[j])?p[j]:'.');
    }
    result += _T("\n");
  }
  result += LINESTRING();
  return result;
}

void hexdump(const void *data, size_t size, FILE *f) {
  const String str = hexdumpString(data, size);
  if(f) {
    _ftprintf(f, _T("%s"), str.cstr());
  } else {
    debugLog(_T("%s"), str.cstr());
  }
}

String bytesToString(const void *data, size_t size) {
  BYTE *p = (BYTE*)data;
  String result;
  for(size_t i = 0; i < size; i++) {
    result += format(_T("%02x "), p[i]);
  }
  return result;
}
