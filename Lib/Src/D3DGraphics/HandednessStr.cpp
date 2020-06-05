#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

String handednessToString(bool rightHanded) {
  return rightHanded ? _T("Righthanded") : _T("Lefthanded");
}

bool handednessFromString(const String &str) {
  static const bool table[] = { true, false };
  return linearStringSearch(str, table, ARRAYSIZE(table), handednessToString);
}
