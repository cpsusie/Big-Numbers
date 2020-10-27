#pragma once

class CEnterTextDlg : public CDialog {
public:
    CEnterTextDlg(const String &str, CWnd *pParent = nullptr);

    enum { IDD = IDD_ENTERTEXT_DIALOG };
    CString m_text;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

