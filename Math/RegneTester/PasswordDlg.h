#pragma once

class CPasswordDlg : public CDialog {
private:
  CString m_expectedPassword;
public:
    CPasswordDlg(const String &expectedPassword, CWnd *pParent = nullptr);

    enum { IDD = IDD_PASSWORD_DIALOG };
    CString m_password;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

