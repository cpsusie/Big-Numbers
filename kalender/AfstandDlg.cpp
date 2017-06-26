#include "stdafx.h"
#include "AfstandDlg.h"
#include <Date.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAfstandDlg::CAfstandDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CAfstandDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CAfstandDlg)
  m_afstand = 0;
  m_dag1 = _T("");
  m_dag2 = _T("");
  //}}AFX_DATA_INIT
}

void CAfstandDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAfstandDlg)
  DDX_Text(pDX, IDC_EDITAFSTAND, m_afstand);
  DDX_Text(pDX, IDC_EDITDAG1, m_dag1);
  DDX_Text(pDX, IDC_EDITDAG2, m_dag2);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAfstandDlg, CDialog)
  //{{AFX_MSG_MAP(CAfstandDlg)
  ON_BN_CLICKED(IDC_RESULT, OnResult)
  ON_BN_CLICKED(IDC_RADIOADDER, OnRadioadder)
  ON_BN_CLICKED(IDC_RADIOAFSTAND, OnRadioafstand)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAfstandDlg::Beregnafstand() {
  Date d1,d2;
  try {
    d1 = Date(m_dag1.GetBuffer(m_dag1.GetLength()));
  } catch(Exception e) {
    MessageBox(e.what(),_T("Helligdage"));
    GetDlgItem(IDC_EDITDAG1)->SetFocus();
    return;
  }
    
  try {
    d2 = Date(m_dag2.GetBuffer(m_dag2.GetLength()));
  } catch(Exception e) {
    MessageBox(e.what(),_T("Helligdage"));
    GetDlgItem(IDC_EDITDAG2)->SetFocus();
    return;
  }
  m_afstand = d2 - d1;
}

void CAfstandDlg::Adder() {
  Date d1;
  try {
    d1 = Date(m_dag1.GetBuffer(m_dag1.GetLength()));
  } catch(Exception e) {
    MessageBox(e.what(),_T("Helligdage"));
    GetDlgItem(IDC_EDITDAG1)->SetFocus();
    return;
  }
  TCHAR tmp[100];
  m_dag2 = Date(d1 + m_afstand).tostr(tmp);
}

void CAfstandDlg::OnResult() {
  UpdateData();
  if(IsDlgButtonChecked(IDC_RADIOAFSTAND)) {
    Beregnafstand();
  } else {
    Adder();
  }
  UpdateData(false);
}

BOOL CAfstandDlg::OnInitDialog() {
  __super::OnInitDialog();
  GetDlgItem(IDC_EDITDAG1)->SetFocus();
  CheckDlgButton(IDC_RADIOAFSTAND,BST_CHECKED);
  return false;
}

void CAfstandDlg::OnRadioadder() {
  CEdit *dag2    = (CEdit*)GetDlgItem(IDC_EDITDAG2   );
  CEdit *afstand = (CEdit*)GetDlgItem(IDC_EDITAFSTAND);

  if(afstand->IsWindowEnabled()) return;

  WINDOWPLACEMENT wp1,wp2;
  int w1,w2;

  dag2->GetWindowPlacement(&wp1);
  w1 = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;

  afstand->GetWindowPlacement(&wp2);
  w2 = wp2.rcNormalPosition.right - wp2.rcNormalPosition.left;

  wp2.rcNormalPosition.right = wp2.rcNormalPosition.left + w1;
  dag2->SetWindowPlacement(&wp2);

  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + w2;
  afstand->SetWindowPlacement(&wp1);

  dag2->EnableWindow(false);
  afstand->EnableWindow(true);
  GetDlgItem(IDC_STATICTEXT2)->SetWindowText(_T("+ antal:"));
  SetWindowText(_T("Dato + antal dage"));
}

void CAfstandDlg::OnRadioafstand() {

  CEdit *dag2    = (CEdit*)GetDlgItem(IDC_EDITDAG2   );
  CEdit *afstand = (CEdit*)GetDlgItem(IDC_EDITAFSTAND);

  if(dag2->IsWindowEnabled()) return;

  WINDOWPLACEMENT wp1,wp2;
  int w1,w2;

  dag2->GetWindowPlacement(&wp1);
  w1 = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;

  afstand->GetWindowPlacement(&wp2);
  w2 = wp2.rcNormalPosition.right - wp2.rcNormalPosition.left;

  wp2.rcNormalPosition.right = wp2.rcNormalPosition.left + w1;
  dag2->SetWindowPlacement(&wp2);

  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + w2;
  afstand->SetWindowPlacement(&wp1);

  dag2->EnableWindow(true);
  afstand->EnableWindow(false);

  GetDlgItem(IDC_STATICTEXT2)->SetWindowText(_T("- dag2:"));
  SetWindowText(_T("Afstand mellem 2 dage"));
}
