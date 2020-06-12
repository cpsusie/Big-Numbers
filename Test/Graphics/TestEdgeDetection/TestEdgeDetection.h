#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

class CTestEdgeDetectionApp : public CWinApp {
public:
    CTestEdgeDetectionApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CTestEdgeDetectionApp theApp;

