#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTestExtractIconsApp : public CWinApp {
public:
    CTestExtractIconsApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestExtractIconsApp theApp;
