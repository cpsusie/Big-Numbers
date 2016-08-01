#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define OEMRESOURCE 

#include "targetver.h"

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define _CRT_SECURE_NO_WARNINGS 1

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <MyUtil.h>
#include <MFCUtil/WinTools.h>

#include "Settings.h"
#include "SearchMachine.h"
#include "resource.h"       // main symbols

#include "HexViewDoc.h"
#include "HexViewView.h"
#include "HexView.h"
#include "MainFrm.h"

