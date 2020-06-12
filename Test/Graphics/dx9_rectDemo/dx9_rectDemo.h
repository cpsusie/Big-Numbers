
// dx9_rectDemo.h : main header file for the PROJECT_NAME application
//

#pragma once

#if !defined(__AFXWIN_H__)
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cdx9_rectDemoApp:
// See dx9_rectDemo.cpp for the implementation of this class
//

class Cdx9_rectDemoApp : public CWinApp
{
public:
	Cdx9_rectDemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cdx9_rectDemoApp theApp;
