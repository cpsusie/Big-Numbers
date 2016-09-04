#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

class CShowGrafApp : public CWinApp {
public:
    CShowGrafApp();

    String getRecentFile(int index);
    public:
    virtual BOOL InitInstance();
    PixRectDevice m_device;

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CShowGrafApp theApp;
