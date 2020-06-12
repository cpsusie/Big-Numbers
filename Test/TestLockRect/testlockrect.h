
// testlockrect.h : main header file for the PROJECT_NAME application
//

#pragma once

#if !defined(__AFXWIN_H__)
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestlockrectApp:
// See testlockrect.cpp for the implementation of this class
//

class CtestlockrectApp : public CWinApp
{
public:
	CtestlockrectApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CtestlockrectApp theApp;
