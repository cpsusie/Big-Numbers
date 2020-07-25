#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

#include <MFCUtil/PixRectDevice.h>
#include "resource.h"       // main symbols
#include "Options.h"

class CMainFrame;

class CD3FunctionApp : public CWinApp {
public:
  PixRectDevice m_prDevice;
  Options       m_options;
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

void convertXMLFile(const String &fileName);
extern CD3FunctionApp theApp;
