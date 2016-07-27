// TestLexStreamDlg.cpp : implementation file
//

#include "stdafx.h"
#include <MyUtil.h>
#include "TestLexStream.h"
#include "TestLexStreamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTestLexStreamDlg::CTestLexStreamDlg(CWnd* pParent /*=NULL*/) : CDialog(CTestLexStreamDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CTestLexStreamDlg)
	m_count = 0;
	m_text = _T("");
	//}}AFX_DATA_INIT
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestLexStreamDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestLexStreamDlg)
	DDX_Text(pDX, IDC_EDITCOUNT, m_count);
	DDV_MinMaxUInt(pDX, m_count, 0, 10000);
	DDX_Text(pDX, IDC_EDITTEXT, m_text);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestLexStreamDlg, CDialog)
    //{{AFX_MSG_MAP(CTestLexStreamDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_BUTTONREAD, OnButtonRead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestLexStreamDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    
    m_stream.close();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestLexStreamDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialog::OnSysCommand(nID, lParam);
    }
}

void CTestLexStreamDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting

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
      const BOOL enable = m_stream.ok() ? TRUE : FALSE;
      GetDlgItem(IDC_BUTTONREAD)->EnableWindow(enable);
      GetDlgItem(IDC_EDITCOUNT )->EnableWindow(enable);
      CDialog::OnPaint();
    }
}

HCURSOR CTestLexStreamDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CTestLexStreamDlg::OnCancel() {
}

void CTestLexStreamDlg::OnOK() {
}

void CTestLexStreamDlg::OnClose() {
  EndDialog(IDOK);
}

void CTestLexStreamDlg::OnOpen() {
  CFileDialog dlg(TRUE);
  CString fileName;
  if(dlg.DoModal() == IDOK) {
    fileName = dlg.m_ofn.lpstrFile;
    m_stream.open((LPCTSTR)fileName);
    if(!m_stream.ok()) {
      MessageBox(format(_T("Cannot open %s"), (LPCTSTR)fileName).cstr(), _T("Error"), MB_ICONWARNING);
      return;
    }
    m_text = _T("");
    UpdateData(FALSE);
  } else {
    return;
  }
}

void CTestLexStreamDlg::OnButtonRead() {
  TCHAR *buf = NULL;
  try {
    if(!UpdateData()) {
      return;
    }
    buf = new TCHAR[m_count+1];
    int got = m_stream.getChars((_TUCHAR*)buf, m_count);
    if(got > 0) {
      buf[got] = 0;
      m_text += buf;
      UpdateData(FALSE);
    }
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }
  if(buf) {
    delete[] buf;
  }
}
