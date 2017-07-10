#pragma once

typedef enum {
  PICK_IDLE
 ,PICK_CENTER
 ,PICK_DESTINATION
} PickMode;

class CTestBlendDlg : public CDialog {
private:
    HICON    m_hIcon;
    int      m_rotationAngle;
    CPoint   m_rotationDesination;
    CPoint   m_rotationCenter;
    PickMode m_pickMode;
    int  getStretchMode();
    void setRotationAngle(      int a          );
    void setRotationDestination(const CPoint &p);
    void setRotationCenter(     const CPoint &p);
    void setPickMode(PickMode mode);
    void setCursors(int winCursor, int imageCursor);
    void drawKingRotated();
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
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnButtonDraw();
    afx_msg void OnButtonBlend();
    afx_msg void OnBnClickedButtonPickRotationDestination();
    afx_msg void OnBnClickedButtonPickRotationCenter();
    DECLARE_MESSAGE_MAP()
};

