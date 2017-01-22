#pragma once

class CAsciiwDlg : public CDialog {
public:
    CAsciiwDlg(CWnd* pParent = NULL);

    enum { IDD = IDD_ASCIIW_DIALOG };

    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    HICON               m_hIcon;
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CFont               m_defaultFont,*m_currentFont;
    CSize               m_currentFontSize;

    void setCurrentFont(CFont *newfont);

protected:
    bool isHexChecked();
    bool isDecChecked() {
      return !isHexChecked();
    }
    void printHeader(CDC &dc, int line, int radix);
    void printAscii(CDC &dc, int radix);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileExit();
    afx_msg void OnViewHex();
    afx_msg void OnViewDec();
    afx_msg void OnViewFont();
    afx_msg void OnSize(UINT nType, int cx, int cy);
//  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

