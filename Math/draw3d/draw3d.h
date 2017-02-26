#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

class CDraw3dApp : public CWinApp {
public:
  CDraw3dApp();

public:
  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CDraw3dApp theApp;

