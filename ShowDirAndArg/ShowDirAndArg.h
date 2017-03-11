#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CShowDirAndArgApp : public CWinApp {
public:
  CShowDirAndArgApp();

  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CShowDirAndArgApp theApp;

