#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class CHexViewApp : public CWinApp {
public:
  CHexViewApp();
  void   addToRecentFileList(const String &name);
  String getRecentFile(        int index);
  void   removeFromRecentFiles(int index);
  Settings m_settings;

  virtual BOOL InitInstance();

  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CHexViewApp theApp;
