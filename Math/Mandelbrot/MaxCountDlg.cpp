#include "stdafx.h"
#include "Mandelbrot.h"
#include "MaxCountDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CMaxCountDlg, CDialog)

CMaxCountDlg::CMaxCountDlg(UINT maxCount, CWnd* pParent /*=nullptr*/)
  : CDialog(IDD_MAXCOUNT_DIALOG, pParent)
  , m_maxCount(maxCount)
{
}

CMaxCountDlg::~CMaxCountDlg()
{
}

void CMaxCountDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_MAXCOUNT, m_maxCount);
  DDV_MinMaxUInt(pDX, m_maxCount, 10, 1000000);
}


BEGIN_MESSAGE_MAP(CMaxCountDlg, CDialog)
  ON_COMMAND(ID_GOTO_MAXCOUNT       , OnGotoMaxCount    )
END_MESSAGE_MAP()


BOOL CMaxCountDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAXCOUNT_ACCELERATOR));

  OnGotoMaxCount();
  return false;
}

BOOL CMaxCountDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CMaxCountDlg::OnOK() {
  if(!UpdateData()) return;
  __super::OnOK();
}

void CMaxCountDlg::OnGotoMaxCount() {
  gotoEditBox(this, IDC_EDIT_MAXCOUNT);
}
