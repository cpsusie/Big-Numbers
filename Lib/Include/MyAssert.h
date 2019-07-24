#pragma once

void xverify(const TCHAR *fileName, int line, const TCHAR *exp);

#ifdef assert
#undef assert
#endif

#ifdef _DEBUG
void xassert(const TCHAR *fileName, int line, const TCHAR *exp);
#define assert(exp) (void)( (exp) || (xassert(__TFILE__, __LINE__, _T(#exp)), 0) )
#else
#define assert(exp)
#endif

#define verify(exp) (void)( (exp) || (xverify(__TFILE__, __LINE__, _T(#exp)), 0) )

#ifdef DEBUG
#define NODEFAULT   assert(0)
#else
#define NODEFAULT   __assume(0)
#endif
