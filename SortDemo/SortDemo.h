#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CSortDemoApp : public CWinApp {
public:
	CSortDemoApp();

	public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CSortDemoApp theApp;
