#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CIRemesApp : public CWinApp {
public:
  CIRemesApp();

  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CIRemesApp theApp;
