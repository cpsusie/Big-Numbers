#pragma once

class CTimtalkDlg : public CDialog {
public:
    CTimtalkDlg(CWnd *pParent = NULL);


    enum { IDD = IDD_TIMTALK_DIALOG };
    CString m_sentence;
    int     m_lix;
    BOOL    m_lixfilter;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnCancel();
    afx_msg void OnButtonsentence();
    afx_msg void OnFileQuit();
    afx_msg void OnHelpAbout();
    afx_msg void OnEditLix();
    DECLARE_MESSAGE_MAP()
};
