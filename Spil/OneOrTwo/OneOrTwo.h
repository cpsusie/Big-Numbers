#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

class COneOrTwoApp : public CWinApp {
public:
    COneOrTwoApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern COneOrTwoApp theApp;

