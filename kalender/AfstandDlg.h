#pragma once

class CAfstandDlg : public CDialog {
public:
    CAfstandDlg(CWnd *pParent = NULL);
    void Beregnafstand();
    void Adder();

    enum { IDD = IDD_AFSTANDDIALOG };
    int     m_afstand;
    CString m_dag1;
    CString m_dag2;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnResult();
    afx_msg void OnRadioadder();
    afx_msg void OnRadioafstand();
    DECLARE_MESSAGE_MAP()
};

