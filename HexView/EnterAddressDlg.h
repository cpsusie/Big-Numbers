#pragma once

class CEnterAddressDlg : public CDialog, SettingsAccessor {
private:
  const UINT64 m_docSize;
  int          m_addrRadix;
  bool         m_addrUppercase;
  UINT64       m_addr;
  CString      m_addressText;
  bool         m_changeEditAddressActive;

  void   gotoEditAddress();
  bool   validateAddress();
  void   errorMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  CEdit *getAddressField();
public:
  CEnterAddressDlg(UINT64 docSize, CWnd *pParent = NULL);
  UINT64 getAddress();
  enum { IDD = IDD_DIALOGENTERADDR };

  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnChangeEditAddress();
  DECLARE_MESSAGE_MAP()
};

