#include "pch.h"

String BigReal::flagsToString(BYTE flags) { // static
  String result;
#define ADDFLAG(f) if(flags & BR_##f) { result += _T(#f); result += _T(" "); }
  ADDFLAG(NEG);
  ADDFLAG(INITDONE);
  ADDFLAG(MUTABLE);
#undef ADDFLAG
  return result;
}
