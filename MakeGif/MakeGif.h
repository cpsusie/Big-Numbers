#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CMakeGifApp : public CWinApp {
public:
    CMakeGifApp();
    PixRectDevice m_device;
    String getRecentFile(int index);
    void removeFromRecentFile(int index);

    virtual BOOL InitInstance();
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CMakeGifApp theApp;
