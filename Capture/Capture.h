#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif


class CCaptureApp : public CWinAppEx
{
public:
    CCaptureApp();
    PixRectDevice m_device;


// Overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation
    BOOL  m_bHiColorIcons;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CCaptureApp theApp;
