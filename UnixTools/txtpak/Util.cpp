#include "StdAfx.h"
#include <Console.h>

bool overwriteAll = false;

static int windowWidth = Console::getWindowSize().X;

bool canOverWrite(const String &fname) {
  if(overwriteAll || ACCESS(fname, 0) < 0) {
    return true;
  }

  for(;;) {
    printMessage(_T("%s already exist. Overwrite (y/n) (a=yes for all files, q=quit)? "), fname.cstr());
    const int ch = Console::getKey();
    printNewline();
    switch(ch) {
    case 'y':
    case 'Y': return true;
    case 'n':
    case 'N': return false;
    case 'a':
    case 'A':
      overwriteAll = true;
      return true;
    case 'q':
    case 'Q':
      exit(0);
    }
  }
}

void vprintMessage(BYTE flags, _In_z_ _Printf_format_string_ TCHAR const * const Format, va_list argptr) {
  static int lastLength = 0;
  String msg = vformat(Format, argptr).trimRight();
  if(flags & PR_1LINE) {
    if(msg.length() > windowWidth) {
      msg = format(_T("%-*.*s..."), windowWidth - 3, windowWidth - 3, msg.cstr());
    }
    _ftprintf(stderr, _T("\r%s"), msg.cstr());
    const int diff = (int)msg.length() - lastLength;
    if(diff < 0) {
      _ftprintf(stderr, _T("%-*s"), -diff, _T(" "));
    }
    lastLength = (int)msg.length();
  } else {
    _vftprintf(stderr, Format, argptr);
  }
  if(flags & PR_ADDNL) {
    _ftprintf(stderr, _T("\n"));
    lastLength = 0;
  }
}

void printMessage(BYTE flags, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintMessage(flags, format, argptr);
  va_end(argptr);
}

void printMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintMessage(PR_1LINE, format, argptr);
  va_end(argptr);
}

void printWarning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  printNewline();
  va_list argptr;
  va_start(argptr, format);
  vprintMessage(PR_ADDNL, format, argptr);
  va_end(argptr);
}
