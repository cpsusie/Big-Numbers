#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "MainFrm.h"

class CPrShowApp : public CWinApp {
  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetMainWnd();
  }

public:
  CPrShowApp();
  PixRectDevice m_device;

  virtual BOOL InitInstance();

  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CPrShowApp theApp;
