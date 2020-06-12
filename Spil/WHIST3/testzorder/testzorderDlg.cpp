// testzorderDlg.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include "testzorder.h"
#include "testzorderDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();


    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CTestzorderDlg::CTestzorderDlg(CWnd *pParent)
    : CDialog(CTestzorderDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestzorderDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestzorderDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_STATIC1, OnStatic1)
    ON_BN_CLICKED(IDC_STATIC2, OnStatic2)
END_MESSAGE_MAP()


BOOL CTestzorderDlg::OnInitDialog() {
    __super::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestzorderDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestzorderDlg::OnPaint()  {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
    __super::OnPaint();
  }
}

HCURSOR CTestzorderDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

static int myflags  = SWP_NOSIZE|SWP_NOMOVE|SWP_DRAWFRAME|SWP_SHOWWINDOW;

void CTestzorderDlg::OnStatic1()  {

  CStatic *s1 = (CStatic *)GetDlgItem(IDC_STATIC1);
  CStatic *s2 = (CStatic *)GetDlgItem(IDC_STATIC2);

  showInformation(_T("Click on 1"));

  assert(s1->SetWindowPos(&wndTopMost,0,0,0,0,myflags));
  assert(s2->SetWindowPos(&wndBottom,0,0,0,0,myflags));
  s2->Invalidate(true);
  s1->Invalidate(true);
}

void CTestzorderDlg::OnStatic2()  {
  CStatic *s1 = (CStatic *)GetDlgItem(IDC_STATIC1);
  CStatic *s2 = (CStatic *)GetDlgItem(IDC_STATIC2);

  showInformation(_T("Click on 2"));

  assert(s2->SetWindowPos(&wndTopMost,0,0,0,0,myflags));
  assert(s1->SetWindowPos(&wndBottom,0,0,0,0,myflags));
  s1->Invalidate(true);
  s2->Invalidate(true);
}
