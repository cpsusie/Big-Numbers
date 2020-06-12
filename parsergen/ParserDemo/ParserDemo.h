#pragma once

#if !defined(__AFXWIN_H__)
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CParserDemoApp : public CWinApp {
public:
	CParserDemoApp();
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CParserDemoApp theApp;
