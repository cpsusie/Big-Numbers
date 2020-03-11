#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include <MFCUtil/PixRect.h>
#include <MFCUtil/PixRectDevice.h>

class CChessApp : public CWinApp {
private:
  static void createFileAssociation();
public:
  CChessApp();
  PixRectDevice m_device;
  Options       m_options;
public:
  virtual BOOL InitInstance();
  virtual int  ExitInstance();

  DECLARE_MESSAGE_MAP()
};

extern CChessApp theApp;
