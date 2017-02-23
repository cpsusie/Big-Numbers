#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

class CMakeAviApp : public CWinApp {
public:
    CMakeAviApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CMakeAviApp theApp;