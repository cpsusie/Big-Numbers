#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CAvlTreeWndApp : public CWinApp {
public:
	CAvlTreeWndApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CAvlTreeWndApp theApp;
