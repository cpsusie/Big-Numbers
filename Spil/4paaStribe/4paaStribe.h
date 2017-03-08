#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CMy4paaStribeApp : public CWinApp {
public:
    CMy4paaStribeApp();

    public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CMy4paaStribeApp theApp;

