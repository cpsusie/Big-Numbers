#include "stdafx.h"
#include "whist3.h"
#include "RulesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RulesDialog::RulesDialog(CWnd* pParent): CDialog(RulesDialog::IDD, pParent) {
}

void RulesDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RulesDialog, CDialog)
END_MESSAGE_MAP()

BOOL RulesDialog::OnInitDialog() {
  CDialog::OnInitDialog();
  GetDlgItem(IDC_TEXT)->SetWindowText(_T("http://www.spillemagasinet.dk/spil/3-mands-whist"));
  return TRUE;  
}
