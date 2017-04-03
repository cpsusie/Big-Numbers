#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTestLexStreamApp : public CWinApp {
public:
    CTestLexStreamApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestLexStreamApp theApp;
