#pragma once

#include "targetver.h"

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#define OEMRESOURCE

#include <MFCUtil/WinTools.h>
#include <MFCUtil/LayoutManager.h>
#include <TinyBitSet.h>

#include "wmpplayer4.h"
#include "wmpmediacollection.h"
#include "wmpmedia.h"
#include "wmpcontrols.h"
#include "wmpsettings.h"
#include "wmpplaylistcollection.h"
#include "wmpplaylistArray.h"
#include "wmpplaylist.h"
#include "wmpplayerapplication.h"
#include "resource.h"       // main symbols

#include "MediaDatabase.h"
#include "Options.h"



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


