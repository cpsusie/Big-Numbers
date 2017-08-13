#include "stdafx.h"
#include "EnterOptionsNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEnterOptionsNameDlg::CEnterOptionsNameDlg(const String &currentName, CWnd *pParent /*=NULL*/)
    : CDialog(CEnterOptionsNameDlg::IDD, pParent)
{
    m_name = currentName.cstr();
}

void CEnterOptionsNameDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_COMBOOPTIONSNAME, m_name);
}

BEGIN_MESSAGE_MAP(CEnterOptionsNameDlg, CDialog)
END_MESSAGE_MAP()

BOOL CEnterOptionsNameDlg::OnInitDialog() {
  __super::OnInitDialog();

  CComboBox *cb = getNameCombo();
  const StringArray names = Options::getExistingNames();
  for(size_t i = 0; i < names.size(); i++) {
    cb->AddString(names[i].cstr());
  }
  cb->SetFocus();
  if((cb->GetCount() == 0) || (m_name.GetLength() == 0)) {
    cb->SetCurSel(0);
  } else {
    cb->SetCurSel((int)names.getFirstIndex((LPCTSTR)m_name));
  }
  return false;
}

void CEnterOptionsNameDlg::OnOK() {
  UpdateData();
  m_name.TrimLeft();
  m_name.TrimRight();
  if(m_name.GetLength() == 0) {
    getNameCombo()->SetFocus();
    showWarning(_T("Please enter a name"));
    return;
  }
  const StringArray names = Options::getExistingNames();
  if(names.size() >= 9 && !names.contains((LPCTSTR)m_name)) {
    getNameCombo()->SetFocus();
    showWarning(_T("Max 9 different settings can be saved"));
    return;
  }
  __super::OnOK();
}

void CEnterOptionsNameDlg::OnCancel() {
  __super::OnCancel();
}

CComboBox *CEnterOptionsNameDlg::getNameCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBOOPTIONSNAME);
}
