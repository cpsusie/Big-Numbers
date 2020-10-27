#include "stdafx.h"
#include "DirectionDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CDirectionDlg::CDirectionDlg(Direction dir, CWnd *pParent /*=nullptr*/) : m_dir(dir), CDialog(CDirectionDlg::IDD, pParent) {
}


void CDirectionDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDirectionDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONS , OnButtonS )
    ON_BN_CLICKED(IDC_BUTTONE , OnButtonE )
    ON_BN_CLICKED(IDC_BUTTONN , OnButtonN )
    ON_BN_CLICKED(IDC_BUTTONW , OnButtonW )
END_MESSAGE_MAP()

BOOL CDirectionDlg::OnInitDialog() {
  __super::OnInitDialog();
  int selectedButton = IDCANCEL;

  switch(m_dir) {
  case S : selectedButton = IDC_BUTTONS ; break;
  case E : selectedButton = IDC_BUTTONE ; break;
  case N : selectedButton = IDC_BUTTONN ; break;
  case W : selectedButton = IDC_BUTTONW ; break;
  }

  GetDlgItem(selectedButton)->SetFocus();
  setWindowText(this, IDC_STATICMSG, format(_T("Is start direction %s Ok?"), directionName[m_dir]));
  setWindowPosition(this, CPoint(700,100));
  return FALSE;
}

void CDirectionDlg::OnButtonS() {
  m_dir = S;
  OnOK();
}

void CDirectionDlg::OnButtonE() {
  m_dir = E;
  OnOK();
}

void CDirectionDlg::OnButtonN() {
  m_dir = N;
  OnOK();
}

void CDirectionDlg::OnButtonW() {
  m_dir = W;
  OnOK();
}

void CDirectionDlg::OnCancel() {
  m_dir = NODIR;
  __super::OnCancel();
}
