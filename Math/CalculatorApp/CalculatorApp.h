#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CCalculatorApp : public CWinApp {
public:
  CCalculatorApp();

public:
  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CCalculatorApp theApp;
