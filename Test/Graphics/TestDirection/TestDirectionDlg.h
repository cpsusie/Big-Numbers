#pragma once

class CTestDirectionDlg : public CDialog {
private:
    HICON m_hIcon;
public:
    CTestDirectionDlg(CWnd *pParent = nullptr);
    CPoint m_mouseDown;
    CPoint m_lastMouse;
    void line(CPoint from, CPoint to, COLORREF color);
    void setMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...);

    enum { IDD = IDD_TESTDIRECTION_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

