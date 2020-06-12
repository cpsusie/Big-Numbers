#pragma once

#if !defined(__AFXWIN_H__)
    #error include 'stdafx.h' before including this file for PCH
#endif

class CD3FunctionPlotterApp : public CWinApp {
public:
  CD3FunctionPlotterApp();

  virtual BOOL InitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CD3FunctionPlotterApp theApp;
