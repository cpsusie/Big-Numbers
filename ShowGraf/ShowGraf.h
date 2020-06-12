#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "MainFrm.h"

class CShowGrafApp : public CWinApp {
public:
    CShowGrafApp();

    String getRecentFile(int index);
public:
    virtual BOOL InitInstance();
    PixRectDevice m_device;
    CMainFrame *getMainWindow() {
      return (CMainFrame*)m_pMainWnd;
    }
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CShowGrafApp theApp;
