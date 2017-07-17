#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

class CShwgraphApp : public CWinApp {
public:
    CShwgraphApp();

    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CShwgraphApp theApp;
