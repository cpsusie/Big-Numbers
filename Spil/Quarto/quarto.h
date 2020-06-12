#pragma once

#if !defined(__AFXWIN_H__)
	#error include 'stdafx.h' before including this file for PCH
#endif

class CQuartoApp : public CWinApp {
public:
	CQuartoApp();
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CQuartoApp theApp;
