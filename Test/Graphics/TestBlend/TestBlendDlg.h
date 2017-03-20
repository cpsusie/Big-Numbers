#pragma once

class CTestBlendDlg : public CDialog {
public:
    CTestBlendDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_TESTBLEND_DIALOG };
    UINT    m_srcConstAlpha;
    UINT    m_alpha1;
    UINT    m_alpha2;
    UINT    m_scale;
    CString m_stretchMode;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    HICON m_hIcon;
    int getStretchMode();
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnButtondraw();
    afx_msg void OnButtonblend();
    DECLARE_MESSAGE_MAP()
};

