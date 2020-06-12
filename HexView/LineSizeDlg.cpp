#include "stdafx.h"
#include "LineSizeDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CLineSizeDlg::CLineSizeDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  const Settings &settings = getSettings();
  m_lineSize = settings.getLineSize();
    m_fitLines = settings.getFitLinesToWindow() ? TRUE : FALSE;
}

void CLineSizeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(      pDX, IDC_EDITLINESIZE, m_lineSize);
  DDV_MinMaxUInt(pDX, m_lineSize, 1, 65536);
  DDX_Check(     pDX, IDC_CHECKFITLINES, m_fitLines);
}

BEGIN_MESSAGE_MAP(CLineSizeDlg, CDialog)
  ON_COMMAND(ID_GOTO_LINESIZE, OnGotoLineSize)
  ON_BN_CLICKED(IDC_CHECKFITLINES, OnCheckFitLines)
END_MESSAGE_MAP()

BOOL CLineSizeDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORLINESIZE));

  UpdateData(FALSE);
  enableLineSize();
  OnGotoLineSize();
  return FALSE;
}

void CLineSizeDlg::OnOK() {
  if(UpdateData()) {
    Settings &settings = getSettings();
    settings.setLineSize(m_lineSize);
    settings.setFitLinesToWindow(m_fitLines?true:false);
    __super::OnOK();
  }
}

void CLineSizeDlg::OnGotoLineSize() {
  gotoEditBox(this,IDC_EDITLINESIZE);
}

void CLineSizeDlg::enableLineSize() {
  const BOOL enable = !IsDlgButtonChecked(IDC_CHECKFITLINES);
  GetDlgItem(IDC_STATICLINESIZE)->EnableWindow(enable);
  GetDlgItem(IDC_EDITLINESIZE  )->EnableWindow(enable);
  if(enable) {
    OnGotoLineSize();
  }
}

void CLineSizeDlg::OnCheckFitLines() {
  enableLineSize();
}

BOOL CLineSizeDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return TRUE;
  }
  return __super::PreTranslateMessage(pMsg);
}
