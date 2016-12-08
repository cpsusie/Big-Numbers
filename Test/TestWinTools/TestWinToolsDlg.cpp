#include "stdafx.h"
#include "TestWinTools.h"
#include "TestWinToolsDlg.h"
#include "afxdialogex.h"
#include <ProcessTools.h>
#include <MFCUtil/WinTools.h>
#include "TestWinTools.h"
#include "TestWinToolsDlg.h"
#include "TestProgressWindowDlg.h"
#include "TestConfirmDlg.h"
#include "TestLayoutManagerDlg.h"
#include "TestMouseDlg.h"
#include "TestOBMBitmapsDlg.h"
#include "TestObmButtonDlg.h"
#include "TestGIFDlg.h"
#include "TestColorControlDlg.h"
#include <MFCUtil/SelectDirDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestWinToolsDlg dialog

CTestWinToolsDlg::CTestWinToolsDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_TESTWINTOOLS_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestWinToolsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestWinToolsDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_COMMAND(ID_FILE_EXIT, OnFileExit)
  ON_COMMAND(ID_TEST_PROGRESSWINDOW, OnTestProgressWindow)
  ON_COMMAND(ID_TEST_TESTCONFIRMDIALOG, OnTestConfirmDialog)
  ON_COMMAND(ID_TEST_LAYOUTMANAGER, OnTestLayoutManager)
  ON_COMMAND(ID_TEST_MOUSE, OnTestMouse)
  ON_COMMAND(ID_TEST_OBMBITMAPS, OnTestOBMBitmaps)
  ON_COMMAND(ID_TEST_GIF, OnTestGif)
  ON_COMMAND(ID_TEST_TESTCOLORCONTROL, OnTestColorControl)
  ON_COMMAND(ID_TEST_TESTOBMBUTTON, OnTestOBMButton)
  ON_COMMAND(ID_TEST_TESTSELECTFOLDER, OnTestSelectFolder)
  ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CTestWinToolsDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)   {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestWinToolsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }   else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestWinToolsDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialogEx::OnPaint();
    showInfo();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestWinToolsDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CTestWinToolsDlg::showInfo() {
  const PROCESS_MEMORY_COUNTERS memCounters = getProcessMemoryUsage();
  const ResourceCounters        resCounters = getProcessResources();

  const String msg = format(_T("Mem.Usage:%s, GDI-objects:%d, User-objects:%d")
                           ,format1000(memCounters.WorkingSetSize).cstr()
                           ,resCounters.m_gdiObjectCount
                           ,resCounters.m_userObjectCount
                           );

  setWindowText(this, IDC_STATICINFO, msg);
}

BOOL CTestWinToolsDlg::PreTranslateMessage(MSG* pMsg) {
  if (TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CTestWinToolsDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestWinToolsDlg::OnClose() {
  OnFileExit();
}

void CTestWinToolsDlg::OnCancel() {
}

void CTestWinToolsDlg::OnTestProgressWindow() {
  CTestProgressWindowDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestConfirmDialog() {
  CTestConfirmDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestLayoutManager() {
  CTestLayoutManagerDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestMouse() {
  CTestMouseDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestOBMBitmaps() {
  CTestOBMBitmapsDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestOBMButton() {
  CTestOBMButtonDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestGif() {
  CTestGIFDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestColorControl() {
  CTestColorControlDlg dlg;
  dlg.DoModal();
  showInfo();
}

void CTestWinToolsDlg::OnTestSelectFolder() {
  CSelectDirDlg dlg(m_currentDir);
  if (dlg.DoModal() == IDOK) {
    m_currentDir = dlg.getSelectedDir();
    MessageBox(m_currentDir.cstr(), _T("Result"));
  }
  else {
    MessageBox(_T("Cancelled"), _T("Result"));
  }
  showInfo();
}
