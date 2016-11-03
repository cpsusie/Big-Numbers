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
