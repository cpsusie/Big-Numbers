#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define OEMRESOURCE

#include "targetver.h"

#include <MFCUtil/WinTools.h>
#include <D3DGraphics/D3DeviceFactory.h>
#include <D3DGraphics/D3Math.h>
#include <D3DGraphics/D3Error.h>
#include <D3DGraphics/Profile.h>
#include "D3Scene.h"
#include "resource.h"       // main symbols




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


