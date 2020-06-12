#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CGetresourcesApp : public CWinApp {
public:
    CGetresourcesApp();

public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CGetresourcesApp theApp;

