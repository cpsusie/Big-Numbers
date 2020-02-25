#include "stdafx.h"
#include "GetIntDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGetIntDlg::CGetIntDlg(const TCHAR *title, const TCHAR *prompt, const IntInterval *legalInterval, int defaultValue, CWnd *pParent)
: CDialog(IDD, pParent)
, m_title(title)
, m_prompt(prompt)
{
  m_intervalSpecified = legalInterval != NULL;
  if(m_intervalSpecified) {
    m_interval = *legalInterval;
  }

  m_int = defaultValue;
}


void CGetIntDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITINT, m_int);
}


BEGIN_MESSAGE_MAP(CGetIntDlg, CDialog)
END_MESSAGE_MAP()

BOOL CGetIntDlg::OnInitDialog() {
  __super::OnInitDialog();

  SetWindowText(m_title);
  GetDlgItem(IDC_PROMPT)->SetWindowText(m_prompt);

  CEdit *edit = (CEdit*)GetDlgItem(IDC_EDITINT);
  edit->SetFocus();
  edit->SetSel(0,10);
  return FALSE;
}

void CGetIntDlg::OnOK() {
  if(!UpdateData())
    return;
  if(m_intervalSpecified && !m_interval.contains(getValue())) {
    showWarning(_T("Please enter an integer in the interval [%d..%d]"),m_interval.getMin(),m_interval.getMax());
    return;
  }
  __super::OnOK();
}
