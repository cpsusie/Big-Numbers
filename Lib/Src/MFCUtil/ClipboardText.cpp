#include "pch.h"
#include <shlobj.h>
#include <MFCUtil/clipboard.h>

#ifdef UNICODE
#define CF_TEXTFORMAT CF_UNICODETEXT
#else
#define CF_TEXTFORMAT CF_TEXT
#endif

#ifdef UNICODE

static String getClipboardUnicodeText() {
  OpenClipboard(NULL);
  HANDLE t = GetClipboardData(CF_UNICODETEXT);
  String result;
  if(t != NULL) {
    result = (TCHAR*)t;
  }
  CloseClipboard();
  return result;
}

#endif

static String getClipboardAsciiText() {
  OpenClipboard(NULL);
  HANDLE t = GetClipboardData(CF_TEXT);
  String result;
  if(t != NULL) {
    result = (char*)t;
  }
  CloseClipboard();
  return result;
}

String getClipboardText() {
#ifdef UNICODE
    return getClipboardUnicodeText();
#else
  return getClipboardAsciiText();
#endif
}

void putClipboard(HWND hwnd, const String &s) {
  if(!OpenClipboard(hwnd)) {
    throwLastErrorOnSysCallException(_T("OpenClipboard"));
  }

  try {
    if(!EmptyClipboard()) {
      throwLastErrorOnSysCallException(_T("EmptyClipboard"));
    }

    String tmp(s);
    tmp.replace(_T("\r\n"),_T("\n")).replace(_T('\n'),_T("\r\n"));
    const int characterCount = (int)tmp.length() + 1;
    const int nbytes = sizeof(TCHAR) * characterCount;
    HLOCAL buf = LocalAlloc(0,nbytes);
    if(buf == NULL) {
      throwLastErrorOnSysCallException(_T("LocalAlloc"));
    }
    memcpy(buf, tmp.cstr(), nbytes);

    if(SetClipboardData(CF_TEXTFORMAT, buf) == NULL) {
      throwLastErrorOnSysCallException(_T("SetClipboardData"));
    }
    CloseClipboard();
  } catch(...) {
    CloseClipboard();
    throw;
  }
}
