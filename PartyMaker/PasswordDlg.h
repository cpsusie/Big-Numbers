#pragma once

class PasswordDlg : public CDialog {
private:
  int m_tryCount;

public:
  PasswordDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_PASSWORDDIALOG };
  CString   m_password;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  BOOL PasswordDlg::OnInitDialog();

  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

