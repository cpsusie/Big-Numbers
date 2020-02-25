#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Options.h"

class CMainFrame;
class C3DSceneView;

class D3ViewArray : public CompactArray<C3DSceneView*> {
public:
  // return index of view with m_hwnd == hwnd, -1 if none found
  int findIndex(HWND hwnd) const;
  C3DSceneView *findViewByHwnd(HWND hwnd) const;
  void remove(C3DSceneView*);
};

class CD3FunctionApp : public CWinApp {
public:
    Options     m_options;
    D3ViewArray m_3DViewArray;
    CD3FunctionApp();
    String getRecentFile(int index);
    void   removeFromRecentFiles(int index);
    void   addToRecentFileList(LPCTSTR lpszPathName);
    inline CMainFrame *getMainFrame() {
      return (CMainFrame*)m_pMainWnd;
    }
    DECLARE_MESSAGE_MAP()
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    afx_msg void OnAppAbout();
    afx_msg void OnFilePrint();
};

extern CD3FunctionApp theApp;
