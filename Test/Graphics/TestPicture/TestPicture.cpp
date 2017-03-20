#include "stdafx.h"
#include "TestPicture.h"
#include "TestPictureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CTestPictureApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestPictureApp::CTestPictureApp() {
}

CTestPictureApp theApp;

BOOL CTestPictureApp::InitInstance() {
    AfxEnableControlContainer();

    CTestPictureDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if(nResponse == IDOK) {
    } else if(nResponse == IDCANCEL) {
    }

    return FALSE;
}
