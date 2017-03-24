#pragma once

class CGetresourcesDlg : public CDialog {
private:
    HICON   m_hIcon;
    HACCEL  m_accelTable;
public:
    CGetresourcesDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_GETRESOURCES_DIALOG };

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileExit();
    afx_msg void OnHelpAboutGetResources();
    afx_msg void OnFileOpenmodule();
    DECLARE_MESSAGE_MAP()
};

