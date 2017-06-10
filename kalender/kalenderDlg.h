#pragma once

class CKalenderDlg : public CDialog {
private:
    HICON m_hIcon;
public:
    CKalenderDlg(CWnd* pParent = NULL);
    void printhelligdage(CClientDC &dc, int year);
    void printhelligdag(CClientDC &dc, int row, const String &name, const Date &d);
    CFont     m_PrintFont;


    enum { IDD = IDD_KALENDERDIALOG };
    int     m_year;

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnDeltaposSpinyear(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDageAfstand();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
};

