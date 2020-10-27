#include "stdafx.h"
#include "TestLexStreamDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();
  enum { IDD = IDD_ABOUTBOX };
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestLexStreamDlg::CTestLexStreamDlg(CWnd *pParent /*=nullptr*/) : CDialog(CTestLexStreamDlg::IDD, pParent) {
    m_count = 0;
    m_text  = EMPTYSTRING;
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestLexStreamDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITCOUNT, m_count);
    DDV_MinMaxUInt(pDX, m_count, 0, 10000);
    DDX_Text(pDX, IDC_EDITTEXT, m_text);
}

BEGIN_MESSAGE_MAP(CTestLexStreamDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_OPEN, OnOpen)
  ON_BN_CLICKED(IDC_BUTTONREAD, OnButtonRead)
END_MESSAGE_MAP()

BOOL CTestLexStreamDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_stream.close();
  return TRUE;
}

void CTestLexStreamDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestLexStreamDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    const int cxIcon = GetSystemMetrics(SM_CXICON);
    const int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect     rect   = getClientRect(this);
    const int x      = (rect.Width()  - cxIcon + 1) / 2;
    const int y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    const BOOL enable = m_stream.ok() ? TRUE : FALSE;
    GetDlgItem(IDC_BUTTONREAD)->EnableWindow(enable);
    GetDlgItem(IDC_EDITCOUNT )->EnableWindow(enable);
    __super::OnPaint();
  }
}

HCURSOR CTestLexStreamDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
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
  if(dlg.DoModal() != IDOK) return;
  CString fileName = dlg.m_ofn.lpstrFile;
  m_stream.open((LPCTSTR)fileName);
  if(!m_stream.ok()) {
    showWarning(_T("Cannot open %s"), (LPCTSTR)fileName);
    return;
  }
  m_text = EMPTYSTRING;
  UpdateData(FALSE);
  Invalidate();
}

void CTestLexStreamDlg::OnButtonRead() {
  TCHAR *buf = nullptr;
  try {
    if(!UpdateData()) {
      return;
    }
    buf = new TCHAR[m_count+1]; TRACE_NEW(buf);
    intptr_t got = m_stream.getChars((_TUCHAR*)buf, m_count);
    if(got > 0) {
      buf[got] = 0;
      m_text += buf;
      UpdateData(FALSE);
    }
  } catch(Exception e) {
    showException(e);
  }
  SAFEDELETE(buf);
}
