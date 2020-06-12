#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

class CRegneTesterApp : public CWinApp {
public:
    CRegneTesterApp();

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CRegneTesterApp theApp;

