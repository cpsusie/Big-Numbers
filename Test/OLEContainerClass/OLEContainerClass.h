#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class COLEContainerClassApp : public CWinApp {
public:
    COLEContainerClassApp();

    virtual BOOL InitInstance();

    COleTemplateServer m_server;
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern COLEContainerClassApp theApp;

