#pragma once

class CGetFormatNameDlg: public CDialog {
public:
  CGetFormatNameDlg(const String &name, CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGENTERFORMATNAME };
  CString   m_name;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

