#include "stdafx.h"
#include "OLEContainerClass.h"
#include "MainFrm.h"
#include "IpFrame.h"
#include "OLEContainerClassDoc.h"
#include "OLEContainerClassView.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(COLEContainerClassApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassApp construction

COLEContainerClassApp::COLEContainerClassApp() {
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COLEContainerClassApp object

COLEContainerClassApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {A03FEFA9-8FC7-4FD3-9729-1807C544131B}
static const CLSID clsid =
{ 0xa03fefa9, 0x8fc7, 0x4fd3, { 0x97, 0x29, 0x18, 0x7, 0xc5, 0x44, 0x13, 0x1b } };

BOOL COLEContainerClassApp::InitInstance() {
    // Initialize OLE libraries
    if(!AfxOleInit()) {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    SetRegistryKey(_T("JGMData"));

    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    // serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(COLEContainerClassDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(COLEContainerClassView));
    pDocTemplate->SetServerInfo(
        IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
        RUNTIME_CLASS(CInPlaceFrame));
    AddDocTemplate(pDocTemplate);

    // Connect the COleTemplateServer to the document template.
    //  The COleTemplateServer creates new documents on behalf
    //  of requesting OLE containers by using information
    //  specified in the document template.
    m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
        // Note: SDI applications register server objects only if /Embedding
        //   or /Automation is present on the command line.

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Check to see if launched as OLE server
    if(cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated) {
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
    }

    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    m_server.UpdateRegistry(OAT_INPLACE_SERVER);
    COleObjectFactory::UpdateRegistryAll();

    // Dispatch commands specified on the command line
    if(!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };

  CAboutDlg() : CDialog(IDD) {
  }

  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void COLEContainerClassApp::OnAppAbout() {
  CAboutDlg().DoModal();
}

