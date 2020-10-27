#pragma once

class CRunLayoutManagerDlg : public CDialog {
private:
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  int                 m_windowFlags;
  int                 m_list1Flags;
  int                 m_list2Flags;
  int                 m_buttonFlags;

  void scaleFont(double scale);
  void enableLayoutMenues();
  void showLayout();
public:
    CRunLayoutManagerDlg(int windowFlags, int list1Flags, int list2Flags, int buttonFlags, CWnd *pParent = nullptr);

    enum { IDD = IDD_RUNTLAYOUTMANAGER_DIALOG };

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
//  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnFontSize1();
    afx_msg void OnFontSize15();
    afx_msg void OnFontSize175();
    afx_msg void OnFontSize2();
    afx_msg void OnFontSize3();
    afx_msg void OnLayoutUpperListVisible();
    afx_msg void OnLayoutLowerListVisible();
    afx_msg void OnLayoutLowerListHalfsize();
    afx_msg void OnOnSize();
    afx_msg void OnFileExit();
    DECLARE_MESSAGE_MAP()
};

