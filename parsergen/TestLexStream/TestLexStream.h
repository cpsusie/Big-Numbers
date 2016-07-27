#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CTestLexStreamApp : public CWinApp {
public:
    CTestLexStreamApp();

    //{{AFX_VIRTUAL(CTestLexStreamApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CTestLexStreamApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
