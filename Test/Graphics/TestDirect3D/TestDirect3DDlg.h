#pragma once

#include "Image.h"

class CTestDirect3DDlg : public CDialog {
public:
    CTestDirect3DDlg(CWnd *pParent = nullptr); // standard constructor

    HICON  m_hIcon;
    Image *m_boardImage;
    Image *m_blackKnightImage;

    enum { IDD = IDD_TESTDIRECT3D_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    DECLARE_MESSAGE_MAP()
};
