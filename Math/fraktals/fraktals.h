#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CFraktalsApp : public CWinApp {
public:
	CFraktalsApp();

  virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CFraktalsApp theApp;
