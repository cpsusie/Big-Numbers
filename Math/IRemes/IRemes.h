#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CIRemesApp : public CWinApp {
public:
  CIRemesApp();

  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CIRemesApp theApp;

template<class T> bool getValue(CDialog *dlg, int id, T &v) {
  String str = getWindowText(dlg, id);
  std::wstringstream stream(str.cstr());
  stream >> v;
  if(!stream) {
    return false;
  }
  return true;
}