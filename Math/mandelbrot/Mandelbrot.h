#pragma once

#if !defined(__AFXWIN_H__)
	#error include 'stdafx.h' before including this file for PCH
#endif

class CMandelbrotApp : public CWinApp {
public:
	CMandelbrotApp();

public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  PixRectDevice m_device;

  DECLARE_MESSAGE_MAP()
};

extern CMandelbrotApp theApp;
