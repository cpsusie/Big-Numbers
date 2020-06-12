#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTimtalkApp : public CWinApp {
public:
    CTimtalkApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTimtalkApp theApp;

