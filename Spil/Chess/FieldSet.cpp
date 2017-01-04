#include "stdafx.h"

FieldSet::FieldSet(int f,...) {
  clear();
  va_list argptr;
  va_start(argptr, f);
  for(;f >= 0; f = va_arg(argptr, int)) {
    add(f);
  }
  va_end(argptr);
}

String FieldSet::toString() const {
  String result = _T("(");
  Iterator<UINT> it = ((FieldSet*)this)->getIterator();
  if(it.hasNext()) {
    result += getFieldName(it.next());
    while(it.hasNext()) {
      result += ',';
      result += getFieldName(it.next());
    }
  }
  result += _T(")");
  return result;
}

void FieldSet::dump() const {
  if(isEmpty()) {
    debugLog(_T("empty\n"));
    return;
  }
  String str = _T("  abcdefgh  \n");
  for (int r = 7; r >= 0; r--) {
    str += format(_T("%d "), r+1);
    for (int c = 0; c < 8; c++) {
      str += contains(MAKE_POSITION(r, c)) ? _T("x") : _T(" ");
    }
    str += format(_T("%d \n"), r+1);
  }
  str += _T("  abcdefgh  \n");
  debugLog(_T("%s\n%s"), toString().cstr(), str.cstr());
}
