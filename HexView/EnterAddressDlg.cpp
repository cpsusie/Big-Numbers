#include "stdafx.h"
#include "EnterAddressDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEnterAddressDlg::CEnterAddressDlg(unsigned __int64 docSize, CWnd *pParent)
: CDialog(IDD, pParent)
, m_docSize(docSize)
{
  const Settings &settings = getSettings();
  m_addrRadix     = settings.getAddrRadix();
  m_addrUppercase = settings.getAddrHexUppercase();

  m_changeEditAddressActive = false;
  m_addressText = EMPTYSTRING;
}

void CEnterAddressDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITADDRESS, m_addressText);
}

BEGIN_MESSAGE_MAP(CEnterAddressDlg, CDialog)
  ON_COMMAND(ID_GOTO_EDITADDRESS, OnGotoEditAddress  )
  ON_EN_CHANGE(IDC_EDITADDRESS  , OnChangeEditAddress)
END_MESSAGE_MAP()

BOOL CEnterAddressDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATORENTERADDR));
  OnGotoEditAddress();
  GetDlgItem(IDC_STATICADDRLABEL)->SetWindowText(format(_T("%s &Address:"), getSettings().getAddrRadixName()).cstr());
  return FALSE;
}

void CEnterAddressDlg::OnOK() {
  UpdateData();
  if(validateAddress()) {
    __super::OnOK();
  }
}

void CEnterAddressDlg::errorMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  OnGotoEditAddress();
  va_list argptr;
  va_start(argptr,format);
  vshowMessageBox(MB_ICONWARNING, format,argptr);
  va_end(argptr);
}

bool CEnterAddressDlg::validateAddress() {
  const TCHAR *s = (LPCTSTR)m_addressText;
  switch(m_addrRadix) {
  case 8 :
    if(_stscanf(s, _T("%I64o"), &m_addr) != 1) {
      errorMessage(_T("Address not an octal number"));
      return false;
    }
    break;

  case 10:
    if(_stscanf(s, _T("%I64u"), &m_addr) != 1) {
      errorMessage(_T("Address not a decimal number"));
      return false;
    }
    break;

  case 16:
    if(_stscanf(s, _T("%I64x"), &m_addr) != 1) {
      errorMessage(_T("Address not a hexadecimal number"));
      return false;
    }
    break;
  }
  if(m_addr > m_docSize) {
    const Settings &settings = getSettings();
    errorMessage(_T("Address too high. File size = %s (%s:%s)")
                ,format1000(m_docSize).cstr()
                ,settings.getAddrRadixShortName()
                ,settings.getAddrAsString(m_docSize).cstr());
    return false;
  }
  return true;
}

unsigned __int64 CEnterAddressDlg::getAddress() {
  return m_addr;
}

void CEnterAddressDlg::OnChangeEditAddress() {
  if(m_changeEditAddressActive) {
    return;
  }
  m_changeEditAddressActive = true;
  CString str;
  CEdit *e = getAddressField();
  e->GetWindowText(str);
  int startChar, endChar;
  e->GetSel(startChar, endChar);
  String s = (LPCTSTR)str;
  bool changed = false;
  for(intptr_t i = s.length()-1; i >= 0; i--) {
    const TCHAR ch = s[i];
    if(!Settings::isValidRadixChar(ch, m_addrRadix)) {
      s.remove(i);
      if(i < startChar) {
        startChar--;
      }
      if(i < endChar) {
        endChar--;
      }
      changed = true;
    }
  }
  const String tmp = m_addrUppercase ? toUpperCase(s) : toLowerCase(s);
  if(tmp != s) {
    s = tmp;
    changed = true;
  }
  if(changed) {
    e->SetWindowText(s.cstr());
    e->SetSel(startChar, endChar);
  }
  m_changeEditAddressActive = false;
}

CEdit *CEnterAddressDlg::getAddressField() {
  return (CEdit*)GetDlgItem(IDC_EDITADDRESS);
}

void CEnterAddressDlg::OnGotoEditAddress() {
  gotoEditBox(this, IDC_EDITADDRESS);
}

BOOL CEnterAddressDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return TRUE;
  }
  return __super::PreTranslateMessage(pMsg);
}
