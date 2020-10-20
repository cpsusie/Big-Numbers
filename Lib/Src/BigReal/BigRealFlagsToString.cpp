#include "pch.h"

String BigReal::flagsToString(BYTE flags) { // static
  String result;
  TCHAR *delim = nullptr;
#define ADDSTR(s)  { if(delim) result += delim; else delim = _T(" "); result += _T(s); }
#define ADDFLAG(f) { if(flags & BR_##f) ADDSTR(#f); }
  if(flags & BR_MUTABLE) ADDSTR("MUTABLE") else ADDSTR("CONST");
  ADDFLAG(INITDONE);
  ADDFLAG(NEG);
#undef ADDFLAG
  return result;
}
