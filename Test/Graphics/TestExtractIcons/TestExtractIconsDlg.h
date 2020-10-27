#pragma once

#include <MFCUtil/LayoutManager.h>

class CTestExtractIconsDlg : public CDialog {
private:
  HICON               m_hIcon;
  SimpleLayoutManager m_layoutManager;
  CompactArray<HICON> m_largeIcons, m_smallIcons;
  void destroyAllIcons();
public:
    CTestExtractIconsDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_TESTEXTRACTICONS_DIALOG };
    BOOL    m_showLargeIcons;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnButtonOpen();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCheckShowLarge();
    DECLARE_MESSAGE_MAP()
};
