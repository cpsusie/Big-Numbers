
// BigText.h : main header file for the PROJECT_NAME application
//

#pragma once

#if !defined(__AFXWIN_H__)
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBigTextApp:
// See BigText.cpp for the implementation of this class
//

class CBigTextApp : public CWinApp
{
public:
	CBigTextApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CBigTextApp theApp;
