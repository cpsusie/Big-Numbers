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

TCHAR *Timestamp::_tostr(TCHAR *dst, const TCHAR *format) const {
  int year, month, day, hour, minute, second, millisecond;
  getDMY(day, month, year);
  getHMS(hour, minute, second, millisecond);
  TCHAR *t = dst;
  for(const TCHAR *cp = format; *cp;) {
    switch(*cp) {
    CASECH('y', year          );
    CASECH('M', month         );
    CASECH('w', getWeek()     );
    CASECH('d', day           );
    CASECH('D', getDayOfYear());
    CASECH('h', hour          );
    CASECH('m', minute        );
    CASECH('s', second        );
    CASECH('S', millisecond   );
    default :
      *(t++) = *(cp++);
      break;
    }
  }
  *t = 0;
  return dst;
}
