#include "stdafx.h"
#include <Direct.h>
#include "resource.h"
#include "ShowDirAndArg.h"
#include "ShowDirAndArgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CShowDirAndArgDlg::CShowDirAndArgDlg(CWnd *pParent) : CDialog(CShowDirAndArgDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CShowDirAndArgDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShowDirAndArgDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
END_MESSAGE_MAP()

HCURSOR CShowDirAndArgDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CShowDirAndArgDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);
  
  String wd = GETCWD();
  
  String argStr;
  TCHAR *delim = NULL;
  for(TCHAR **argv = __targv; *argv; argv++) {
    if(delim) {
      argStr += delim;
    } else {
      delim = _T("\r\n");
    }
    argStr += *argv;
  }

  setWindowText(this, IDC_EDITCWD , wd    );
  setWindowText(this, IDC_EDITARGV, argStr);

  CListCtrl *list = (CListCtrl*)GetDlgItem(IDC_LISTENV);
  list->SetExtendedStyle(LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
  list->InsertColumn( 0,_T("Name" ), LVCFMT_LEFT, 200);
  list->InsertColumn( 1,_T("Value"), LVCFMT_LEFT, 500);
  int count = 0;
  for(TCHAR **env = _tenviron; *env; env++, count++) {
    const String ev = *env;
    Tokenizer tok(ev, _T("="));
    const String name  = tok.next();
    const String value = tok.hasNext()?tok.next():EMPTYSTRING;
    addData(*list, count, 0, name, true);
    addData(*list, count, 1, value);
  }

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDITCWD   , RELATIVE_WIDTH   );
  m_layoutManager.addControl(IDC_EDITARGV  , RELATIVE_SIZE    );
  m_layoutManager.addControl(IDC_STATICENV , RELATIVE_Y_POS   );
  m_layoutManager.addControl(IDC_LISTENV   , RELATIVE_Y_POS | RELATIVE_WIDTH | INIT_LISTHEADERS| RESIZE_LISTHEADERS);
  m_layoutManager.addControl(IDOK          , RELATIVE_POSITION);
  m_layoutManager.addControl(IDCANCEL      , RELATIVE_POSITION);
  return TRUE;
}

void CShowDirAndArgDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  CDialog::OnSize(nType, cx, cy);
}

void CShowDirAndArgDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
  }
}
