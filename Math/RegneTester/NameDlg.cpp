#include "stdafx.h"
#include <Random.h>
#include "RegneTester.h"
#include "NameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CNameDlg::CNameDlg(CWnd *pParent /*=NULL*/) : CDialog(CNameDlg::IDD, pParent) {
  m_name = EMPTYSTRING;
}


void CNameDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDV_MaxChars(pDX, m_name, 49);
}


BEGIN_MESSAGE_MAP(CNameDlg, CDialog)
    ON_BN_CLICKED(IDC_CANCEL, OnCancel)
END_MESSAGE_MAP()

BOOL CNameDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  const int resId = randInt() % (IDB_LAST_HIGHSCOREIMAGE - IDB_FIRST_HIGHSCOREIMAGE + 1) + IDB_FIRST_HIGHSCOREIMAGE;
  m_bitmap.LoadBitmap(resId);
  ((CStatic*)GetDlgItem(IDC_HIGHSCOREIMAGE))->SetBitmap(m_bitmap);

  gotoEditBox(this, IDC_EDITNAME);
  return FALSE;
}

void CNameDlg::OnOK() {
  UpdateData();
  if(m_name.GetLength() == 0) {
    MessageBox(_T("Indtast dit navn"), _T("Fejl"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDITNAME);
    return;
  }

  CDialog::OnOK();
}

void CNameDlg::OnCancel() {
  if(MessageBox(_T("Er du sikker på, at du ikke vil gemme din tid"), _T("Annuller"), MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CDialog::OnCancel();
  }
}
