#pragma once

#include <D3D9.h>

#ifdef _DEBUG

#define DECLARE_THISFILE static const TCHAR *_THISFILE = _T(__FILE__)

void checkD3DResult(const TCHAR *fileName, int line, HRESULT hr, bool exitOnError);

#define V( hr) checkD3DResult(_THISFILE,__LINE__,hr, true )
#define VW(hr) checkD3DResult(_THISFILE,__LINE__,hr, false)

#else

#define DECLARE_THISFILE

void checkD3DResult(HRESULT hr, bool exitOnError);

#define V( hr) checkD3DResult(hr, true )
#define VW(hr) checkD3DResult(hr, false)

#endif
