#pragma once

class DegreeDlg : public CDialog {
public:
    DegreeDlg(int degree, CWnd *pParent = nullptr);

    enum { IDD = IDD_DEGREE_DIALOG };
    UINT    m_degree;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};

