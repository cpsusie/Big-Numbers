#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif


class CCaptureApp : public CWinApp {
public:
    CCaptureApp();
    PixRectDevice m_device;
public:
    virtual BOOL InitInstance();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CCaptureApp theApp;
