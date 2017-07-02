#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

class CTestQRGraphicsApp : public CWinApp {
public:
    CTestQRGraphicsApp();
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CTestQRGraphicsApp theApp;
