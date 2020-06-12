#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTestDirectXApp : public CWinApp {
public:
    CTestDirectXApp();
    PixRectDevice m_device;
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestDirectXApp theApp;

