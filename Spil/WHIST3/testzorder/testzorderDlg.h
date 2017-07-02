#pragma once

class CTestzorderDlg : public CDialog {
private:
    HICON m_hIcon;
public:
    CTestzorderDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_TESTZORDER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnStatic1();
    afx_msg void OnStatic2();
    DECLARE_MESSAGE_MAP()
};

