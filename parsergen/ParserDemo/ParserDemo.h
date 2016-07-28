#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CParserDemoApp : public CWinApp {
public:
	CParserDemoApp();

	public:
	virtual BOOL InitInstance();


	DECLARE_MESSAGE_MAP()
};

