#include "stdafx.h"
#include "TestBin.h"
#include "CountInt64Dlg.h"
#include "IntStreamTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestBinApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestBinApp::CTestBinApp() {
}

CTestBinApp theApp;

BOOL CTestBinApp::InitInstance() {
  AfxEnableControlContainer();

  StartCommand command = CMD_COUNT;
  TCHAR **argv = __targv;
  argv++;
  if(*argv) {
    const String option = *argv;
    if(option == _T("float")) {
      command = CMD_TESTFLOAT;
    } else if (option == _T("int")) {
      command = CMD_TESTINT;
    }
  }
//  testIntStream();

  CCountInt64Dlg dlg(command);
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  return FALSE;
}
