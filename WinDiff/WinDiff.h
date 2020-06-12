#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Options.h"

class CWinDiffApp : public CWinApp {
public:
    Options m_options;
    CWinDiffApp();
    String getRecentFile(int index);
    void   removeFromRecentFiles(int index);
    void   addToRecentFileList(LPCTSTR lpszPathName);

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    afx_msg void OnAppAbout();
    afx_msg void OnFilePrint();
    DECLARE_MESSAGE_MAP()
};

extern CWinDiffApp theApp;
