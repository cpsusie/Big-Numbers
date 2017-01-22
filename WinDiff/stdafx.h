#pragma once

#include <MFCUtil/WinTools.h>
#include "WinDiff.h"

//#define MEASURE_STEPTIME

#define BLACK                RGB(  0,  0,  0)
#define LIGHTGREY            RGB(235,235,235)
#define LINENUMBERBACKGROUND RGB(240,240,240)
#define GREY                 RGB(192,192,192)
#define WHITE                RGB(255,255,255)
#define BLUE                 RGB(  0,  0,255)
#define GREEN                RGB(  0,128,  0)
#define RED                  RGB(255,  0,  0)
#define LIGHTBLUE            RGB(198,214,253)
#define LIGHTRED             RGB(255,140,140)
#define LIGHTGREEN           RGB(140,255,140)


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


