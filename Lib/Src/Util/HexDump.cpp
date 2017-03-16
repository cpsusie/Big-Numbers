#include "pch.h"

#define LINESIZE 16

#define LINESTRING()  spaceString(60, _T('-'))

String hexdumpString(const void *data, int size) {
  String result = format(_T("hexdump(size=%u)\n"), size);
  const BYTE *p = (const BYTE*)data;
  result += LINESTRING();
  for(int i = 0; i < size; i += LINESIZE) {
    result += format(_T("%p:"),p+i);
    int j;
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

void hexdump(const void *data, int size, FILE *f) {
  const String str = hexdumpString(data, size);
  if (f) {
    _ftprintf(f, _T("%s"), str.cstr());
  } else {
    debugLog(_T("%s"), str.cstr());
  }
}

String bytesToString(const void *data, int size) {
  BYTE *p = (BYTE*)data;
  String result;
  for(int i = 0; i < size; i++) {
    result += format(_T("%02x "), p[i]);
  }
  return result;
}
