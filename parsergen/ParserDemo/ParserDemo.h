#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CParserDemoApp : public CWinApp {
public:
	CParserDemoApp();

	//{{AFX_VIRTUAL(CParserDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CParserDemoApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
