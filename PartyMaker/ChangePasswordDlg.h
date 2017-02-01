#pragma once

class CChangePasswordDlg : public CDialog {
private:
  int m_tryCount;
public:
  CChangePasswordDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_CHANGEPASSWORDDIALOG };
    CString m_oldPassword;
    CString m_newPassword;
    CString m_newPassword2;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    BOOL OnInitDialog();

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

