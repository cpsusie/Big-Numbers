#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTestDirect3DApp : public CWinApp {
public:
    CTestDirect3DApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestDirect3DApp theApp;
