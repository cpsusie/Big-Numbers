#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class CPartyMakerApp : public CWinApp {
public:
  CPartyMakerApp();

public:
  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

void showErrors();

extern CPartyMakerApp theApp;
