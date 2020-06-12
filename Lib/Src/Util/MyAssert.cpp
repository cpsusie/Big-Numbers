#include "pch.h"
#include <MyUtil.h>
#include <MyAssert.h>

#if defined(_DEBUG)
void xassert(const TCHAR *fileName, int line, const TCHAR *exp) {
  if(isatty(stderr)) {
    _ftprintf(stderr,_T("\n\nAssertion %s at %s line %d failed.\n"), exp, fileName, line);
  } else {
    MessageBox(0, format(_T("Assertion\r\n\r\n%s\r\n\r\nat %s\r\nline %d failed.\n"), exp, fileName, line).cstr(), _T("Assertion failed"), MB_ICONSTOP);
  }
  abort();
}
#endif

void xverify(const TCHAR *fileName, int line, const TCHAR *exp) {
  if(isatty(stderr)) {
    _ftprintf(stderr,_T("\n\nVerification %s at %s line %d failed.\n"), exp, fileName, line);
  } else {
    MessageBox(0, format(_T("Verification\r\n\r\n%s\r\n\r\nat %s\r\nline %d failed.\n"), exp, fileName, line).cstr(), _T("Verification failed"), MB_ICONSTOP);
  }
  abort();
}
