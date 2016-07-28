#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_WARNINGS 1

#include "targetver.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <Grammar.h>

#undef TRY
#undef CATCH
#undef THROW


