#include "stdafx.h"
#include "resource.h"
#include "Calculator.h"
#include "PrecisionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PrecisionDlg::PrecisionDlg(int prec, CWnd *pParent) : CDialog(PrecisionDlg::IDD, pParent) {
	m_precision = prec;
}

void PrecisionDlg::DoDataExchange(CDataExchange *pDX) {
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITPRECISION, m_precision);
}

BEGIN_MESSAGE_MAP(PrecisionDlg, CDialog)
    ON_COMMAND(     ID_GOTOPRECISION, OnGotoPrecision)
END_MESSAGE_MAP()

BOOL PrecisionDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTabel = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORPRECISION));

  OnGotoPrecision();

  return FALSE;
}

void PrecisionDlg::OnOK() {
  if(!UpdateData()) {
    return;
  }
  if(m_precision < 1 || m_precision > MAXPRECISION) {
    MessageBox(format(_T("Please enter an integer between 1 and %d"), MAXPRECISION).cstr(), EMPTYSTRING, MB_ICONEXCLAMATION);
    OnGotoPrecision();
    return;
  }
  __super::OnOK();
}

void PrecisionDlg::OnGotoPrecision() {
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITPRECISION);
  e->SetSel(0, 20);
  e->SetFocus();
}

BOOL PrecisionDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTabel, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
