#pragma once

class CTestDirectXDlg : public CDialog {
private:
    HICON m_hIcon;
public:
    CTestDirectXDlg(CWnd *pParent = nullptr);

    PixRect *m_image;

    enum { IDD = IDD_TESTDIRECTX_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileExit();
    virtual void OnCancel();
    afx_msg void OnClose();
    virtual void OnOK();
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    DECLARE_MESSAGE_MAP()
};

