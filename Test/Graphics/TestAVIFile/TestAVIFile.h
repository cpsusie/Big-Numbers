#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CTestAVIFileApp : public CWinApp {
public:
    CTestAVIFileApp();
    PixRectDevice m_device;

    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CTestAVIFileApp theApp;

