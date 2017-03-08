#include "stdafx.h"
#include "sudoku.h"
#include "LevelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLevelDlg::CLevelDlg(GameLevel level, CWnd *pParent) : CDialog(CLevelDlg::IDD, pParent) {
    m_level = level;
}

void CLevelDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLevelDlg, CDialog)
END_MESSAGE_MAP()

BOOL CLevelDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  int buttonid;
  switch(m_level) {
  case VERY_EASY     : buttonid = IDC_RADIOVERY_EASY;      break;
  case EASY          : buttonid = IDC_RADIOEASY;           break;
  case MEDIUM        : buttonid = IDC_RADIOMEDIUM;         break;
  case DIFFICULT     : buttonid = IDC_RADIODIFFICULT;      break;
  case VERY_DIFFICULT: buttonid = IDC_RADIOVERY_DIFFICULT; break;
  }
  CButton *b = (CButton*)GetDlgItem(buttonid);
  b->SetCheck(true);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CLevelDlg::OnOK() {
  if(IsDlgButtonChecked(IDC_RADIOVERY_EASY)) {
    m_level = VERY_EASY;
  } else if(IsDlgButtonChecked(IDC_RADIOEASY)) {
    m_level = EASY;
  } else if(IsDlgButtonChecked(IDC_RADIOMEDIUM)) {
    m_level = MEDIUM;
  } else if(IsDlgButtonChecked(IDC_RADIODIFFICULT)) {
    m_level = DIFFICULT;
  } else if(IsDlgButtonChecked(IDC_RADIOVERY_DIFFICULT)) {
    m_level = VERY_DIFFICULT;
  }
  CDialog::OnOK();
}
