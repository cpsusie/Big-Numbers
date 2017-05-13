#pragma once

#ifdef _DEBUG

#define DECLARE_THISFILE static const TCHAR *_THISFILE = __TFILE__

void checkD3DResult(const TCHAR *fileName, int line, HRESULT hr, bool exitOnError);

#define FV(hr) checkD3DResult(__TFILE__, __LINE__, hr, true )
#define FW(hr) checkD3DResult(__TFILE__, __LINE__, hr, false)
#define V( hr) checkD3DResult(_THISFILE, __LINE__, hr, true )
#define VW(hr) checkD3DResult(_THISFILE, __LINE__, hr, false)
#else

#define DECLARE_THISFILE

void checkD3DResult(HRESULT hr, bool exitOnError);

#define V( hr) checkD3DResult(hr, true )
#define VW(hr) checkD3DResult(hr, false)
#define FV(hr) V(hr)
#define FW(hr) VW(hr)

#endif
