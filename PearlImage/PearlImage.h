#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#define APSTUDIO_INVOKED

#include "MainFrm.h"

class CPearlImageApp : public CWinApp {
  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetMainWnd();
  }

public:
  PixRectDevice m_device;
  CPearlImageApp();
  String getRecentFile(int index);

  virtual BOOL InitInstance();
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CPearlImageApp theApp;


