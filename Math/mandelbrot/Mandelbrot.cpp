#include "stdafx.h"
#include "Mandelbrot.h"
#include "MandelbrotDlg.h"
#include <Math/BigReal.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMandelbrotApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CMandelbrotApp::CMandelbrotApp() {
}

CMandelbrotApp theApp;

BOOL CMandelbrotApp::InitInstance() {

#pragma warning(disable : 4101)

  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  __super::InitInstance();
  AfxEnableControlContainer();
  BigReal::loadPow2Cache();
  DigitPool *digitPool = BigRealResourcePool::getInstance().fetchDigitPool();
  try {
    CMandelbrotDlg dlg(digitPool);
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if(nResponse == IDCANCEL) {
    }
  } catch(Exception e) {
    showException(e);
  }
  BigRealResourcePool::getInstance().releaseDigitPool(digitPool);

  return FALSE;
}


int CMandelbrotApp::ExitInstance() {
  theApp.m_device.detach();
  return __super::ExitInstance();
}
