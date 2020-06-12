// testzorder.h : main header file for the TESTZORDER application
//

#if !defined(AFX_TESTZORDER_H__BE6B1805_BF9A_11D4_8BEC_005004C659B2__INCLUDED_)
#define AFX_TESTZORDER_H__BE6B1805_BF9A_11D4_8BEC_005004C659B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CTestzorderApp : public CWinApp {
public:
    CTestzorderApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TESTZORDER_H__BE6B1805_BF9A_11D4_8BEC_005004C659B2__INCLUDED_)
