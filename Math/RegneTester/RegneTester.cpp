#include "stdafx.h"
#include <Process.h>
#include "RegneTester.h"
#include "RegneTesterDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRegneTesterApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CRegneTesterApp::CRegneTesterApp() {
}

CRegneTesterApp theApp;

static void copyAndExit(const String &to, const String &md5Password) {
  Options options;
  if(md5Password != options.getMD5Password()) {
    exit(-1);
  }
  for(int i = 0; i < 5; i++) {
    try {
      copyFile(to, getModuleFileName());
      break;
    } catch(...) {
      Sleep(300);
    }
  }

  _tspawnl(P_NOWAIT, to.cstr(), to.cstr(), _T("-m"), getModuleFileName().cstr(), nullptr);
  exit(0);
}

BOOL CRegneTesterApp::InitInstance() {
  AfxEnableControlContainer();

  TCHAR **argv = __targv, *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'c':
        { const String md5Password = *(++argv);
          const String fileName    = *(++argv);
//            SetSystemCursor(GetCursor(), OCR_WAIT);
          copyAndExit(fileName, md5Password);
        }
      case 'm':
        { const String fileName = *(++argv);
          bool deleteDone = false;
          for(int i = 0; i < 5; i++) {
            if(unlink(fileName) >= 0) {
              deleteDone = true;
              break;
            }
            Sleep(300);
          }
          if(deleteDone) {
            showInformation(_T("Indstillinger gemt"));
            _tspawnl(P_NOWAIT, getModuleFileName().cstr(), getModuleFileName().cstr(), nullptr);
          }
          exit(0);
        }
      default:
        exit(0);
      }
    }
  }

  CRegneTesterDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if(nResponse == IDOK) {
  } else if(nResponse == IDCANCEL) {
  }

  return FALSE;
}
