#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CFernApp : public CWinApp {
public:
    CFernApp();

public:
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CFernApp theApp;
