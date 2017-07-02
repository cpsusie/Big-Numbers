#include "stdafx.h"
#include "whist3.h"
#include "IdentifyDialog.h"

IdentifyDialog::IdentifyDialog(CWnd *pParent) : CDialog(IdentifyDialog::IDD, pParent) {
  const Options &options = getOptions();
  m_myName     = options.m_myName.cstr();
  m_dealerName = options.m_dealerName.cstr();
  m_connected  = options.m_connected;
}

void IdentifyDialog::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_MYNAME_EDIT     , m_myName    );
  DDX_Text( pDX, IDC_DEALER_NAME_EDIT, m_dealerName);
  DDX_Radio(pDX, IDC_DEALER_RADIO    , m_connected );
}


BEGIN_MESSAGE_MAP(IdentifyDialog, CDialog)
  ON_COMMAND(ID_GOTO_NAME        , OnGotoName      )
  ON_COMMAND(ID_GOTO_DEALERNAME  , OnGotoDealerName)
  ON_BN_CLICKED(IDC_DEALER_RADIO , OnDealerRadio   )
  ON_BN_CLICKED(IDC_CONNECT_RADIO, OnConnectRadio  )
  ON_BN_CLICKED(IDC_QUIT_BUTTON  , OnQuitButton    )
END_MESSAGE_MAP()
  
BOOL IdentifyDialog::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_IDENTIFY_ACCELERATOR));

  GetDlgItem(IDC_DEALER_NAME_EDIT)->EnableWindow(m_connected);
  GetDlgItem(IDC_DEALER_NAME_TEXT)->EnableWindow(m_connected);  

  CEdit *editName = (CEdit*)GetDlgItem(IDC_MYNAME_EDIT);
  editName->SetSel(0,30);
  editName->SetFocus();

  return FALSE;
}

BOOL IdentifyDialog::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return TRUE;
  }
  return __super::PreTranslateMessage(pMsg);
}

void IdentifyDialog::OnDealerRadio() {
  GetDlgItem(IDC_DEALER_NAME_EDIT)->EnableWindow(FALSE);
  GetDlgItem(IDC_DEALER_NAME_TEXT)->EnableWindow(FALSE);    
}

void IdentifyDialog::OnConnectRadio() {
  GetDlgItem(IDC_DEALER_NAME_EDIT)->EnableWindow(TRUE);
  GetDlgItem(IDC_DEALER_NAME_TEXT)->EnableWindow(TRUE); 
}

void IdentifyDialog::OnQuitButton() {
  exit(0);  
}

static CString trim(const CString &s) {
  CString t = s;
  t.TrimLeft();
  t.TrimRight();
  return t;
}

void IdentifyDialog::OnOK() {
  UpdateData();
  m_myName     = trim(m_myName);
  m_dealerName = trim(m_dealerName);

  if(m_myName == EMPTYSTRING) {
    UpdateData(false);
    MessageBox(_T("Indtast dit navn"),_T("Udfyld navn"), MB_ICONEXCLAMATION );
    OnGotoName();
    return;
  }
  if(m_connected && m_dealerName == EMPTYSTRING) {
    UpdateData(false);
    MessageBox(_T("Indtast kortgivers computernavn"), _T("Udfyld computernavn"), MB_ICONEXCLAMATION);
    OnGotoDealerName();
    return;
  }

  Options &options = getOptions();

  options.m_myName     = (LPCTSTR)m_myName;
  options.m_dealerName = (LPCTSTR)m_dealerName;
  options.m_connected  = m_connected ? true : false;
  options.save();

  __super::OnOK();
}

void IdentifyDialog::OnGotoName() {
  gotoEditBox(this, IDC_MYNAME_EDIT);
}

void IdentifyDialog::OnGotoDealerName() {
  gotoEditBox(this, IDC_DEALER_NAME_EDIT);
}
