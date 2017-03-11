#pragma once

#include <MFCUtil/LayoutManager.h>

class CShowDirAndArgDlg : public CDialog {
private:
  HICON               m_hIcon;
  SimpleLayoutManager m_layoutManager;
public:
    CShowDirAndArgDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_SHOWDIRANDARG_DIALOG };
protected:
    virtual void    DoDataExchange(CDataExchange *pDX);
    virtual BOOL    OnInitDialog();
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

