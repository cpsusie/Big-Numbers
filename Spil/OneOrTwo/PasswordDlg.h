#pragma once

class CPasswordDlg : public CDialog {
public:
    CPasswordDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_PASSWORDDIALOG };
    CString m_password;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

