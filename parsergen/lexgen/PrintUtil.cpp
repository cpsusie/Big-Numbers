#include "stdafx.h"
#include "NFA.h"

String binToAscii(int c, bool useHex) {
  char result[10];
  c &= 0xff;
  if(' ' < c  &&  c < 0x7f  &&  c != '\'' && c != '\\') {
    result[0] = c;
    result[1] = '\0';
  } else {
    result[0] = '\\' ;
    result[2] = '\0' ;

    switch(c) {
    case '\\': result[1] = '\\'; break;
    case '\'': result[1] = '\''; break;
    case '\b': result[1] = 'b' ; break;
    case '\t': result[1] = 't' ; break;
    case '\f': result[1] = 'f' ; break;
    case '\r': result[1] = 'r' ; break;
    case '\n': result[1] = 'n' ; break;
    case ' ' : result[1] = 's' ; break;
    default  : sprintf(&result[1], useHex ? "x%02x" : "%03o", c);
               break;
    }
  }
  return result;
}

void printChar(MarginFile &f, int c) {
  f.printf(_T("%s"), binToAscii(c).cstr());
}

#define FORMATCHAR(ch) binToAscii(ch)
#define FLUSHRANGE()                                                            \
{ String tmp;                                                                   \
  if(delim) tmp += delim; else delim = _T(",");                                 \
  if(first == last) {                                                           \
    tmp = FORMATCHAR(first);                                                    \
  } else {                                                                      \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");    \
    tmp = format(formatStr, FORMATCHAR(first).cstr(), FORMATCHAR(last).cstr()); \
  }                                                                             \
  if(charsPrinted + tmp.length() > RMARGIN) {                                   \
    f.printf(_T("\n"));                                                         \
    charsPrinted = f.getLeftMargin();                                           \
  }                                                                             \
  f.printf(_T("%s"), tmp.cstr());                                               \
  charsPrinted += tmp.length();                                                 \
}

void printCharClass(MarginFile &f, BitSet &set) {
  size_t charsPrinted = f.getLeftMargin();
  unsigned int first = 1, last = 0;
  const TCHAR *delim = NULL;
  for(Iterator<size_t> it = ((BitSet&)set).getIterator(); it.hasNext();) {
    const UINT ch = (UINT)it.next();
    if(first > last) {
      first = last = ch;
    } else if(ch == last+1) {
      last = ch;
    } else {
      FLUSHRANGE();
      first = last = ch;
    }
  }
  if(first <= last) {
    FLUSHRANGE();
  }
}
/*
void printCharClass(MarginFile &f, BitSet &set) {
  size_t charsPrinted = f.getLeftMargin();
  for(Iterator<size_t> it = set.getIterator(); it.hasNext();) {
    int i = (int)it.next();
    String tmp = binToAscii(i);
    if(charsPrinted + tmp.length() > RMARGIN) {
      f.printf(_T("\n"));
      charsPrinted = f.getLeftMargin();
    }
    f.printf(_T("%s"), tmp.cstr());
    charsPrinted += tmp.length();
  }
}
*/
void printSet(MarginFile &f, BitSet &set) {
  f.printf(_T("["));
  size_t charsPrinted = f.getLeftMargin();
  for(Iterator<size_t> it = set.getIterator(); it.hasNext();) {
    int i = (int)it.next();
    String tmp = format(_T("%d "), i);
    if(charsPrinted + tmp.length() > RMARGIN) {
      f.printf(_T("\n"));
      charsPrinted = f.getLeftMargin();
    }
    f.printf(_T("%s"), tmp.cstr());
    charsPrinted += tmp.length();
  }

  f.printf(_T("]"));
}
