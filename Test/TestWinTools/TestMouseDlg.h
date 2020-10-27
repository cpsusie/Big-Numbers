#pragma once

class CTestMouseDlg : public CDialog {
private:
    HCURSOR m_systemCursor;
    HCURSOR m_createdCursor;

    void initComboMouseCursor();
    void showFlags(const TCHAR *function, UINT flags);
    void releaseCreatedCursor();
public:
    CTestMouseDlg(CWnd *pParent = nullptr);
    ~CTestMouseDlg();

    enum { IDD = IDD_TESTMOUSE_DIALOG };

public:
    virtual BOOL DestroyWindow();
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnNormalSystemCursor();
    afx_msg void OnSetSystemCursor1();
    afx_msg void OnSetSystemCursor2();
    afx_msg void OnNormalWindowCursor();
    afx_msg void OnSetWindowCursor1();
    afx_msg void OnSetWindowCursor2();
    afx_msg void OnNormalControlCursor();
    afx_msg void OnSetControlCursor1();
    afx_msg void OnSetControlCursor2();
    afx_msg void OnSetCursorPos();
    afx_msg void OnHideCursor();
    afx_msg void OnWaitCursor();
    afx_msg void OnClipCursor();
    afx_msg void OnUnclipCursor();
    afx_msg void OnCreateCursor();
    afx_msg void OnSelchangeComboMouseCursor();
    virtual BOOL OnInitDialog();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLoadAnimatedCursor();
    DECLARE_MESSAGE_MAP()
};

