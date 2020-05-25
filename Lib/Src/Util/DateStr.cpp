#include "pch.h"
#include <Date.h>

#define CASECH(ch, comp) case ch: {                         \
  int count = 1, scale = 10;                                \
  for(cp++; *cp == ch; cp++) {                              \
    count++;                                                \
    scale *= 10;                                            \
  }                                                         \
  String tmp = ::format(_T("%0*d"), count, (comp) % scale); \
  _tcscpy(t, tmp.cstr());                                   \
  t += tmp.length();                                        \
}                                                           \
break

TCHAR *Date::_tostr(TCHAR *dst, const TCHAR *format) const {
  int year, month, day;
  getDMY(day, month, year);
  TCHAR *t  = dst;
  for(const TCHAR *cp = format; *cp;) {
    switch(*cp) {
    CASECH(_T('y'), year          );
    CASECH(_T('M'), month         );
    CASECH(_T('w'), getWeek()     );
    CASECH(_T('d'), day           );
    CASECH(_T('D'), getDayOfYear());

    default :
      *(t++) = *(cp++);
      break;
    }
  }
  *t = 0;
  return dst;
}
