#pragma once

void xverify(const TCHAR *fileName, int line, const TCHAR *exp);

#ifdef assert
#undef assert
#endif

#ifdef _DEBUG
void xassert(const TCHAR *fileName, int line, const TCHAR *exp);
#define assert(exp) (void)( (exp) || (xassert(_T(__FILE__), __LINE__, _T(#exp)), 0) )
#else
#define assert(exp)
#endif

#define verify(exp) (void)( (exp) || (xverify(_T(__FILE__), __LINE__, _T(#exp)), 0) )
