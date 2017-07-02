#pragma once

#include <MFCUtil/LayoutManager.h>

class CTestRegexDlg : public CDialog {
private:
  SimpleLayoutManager            m_layoutManager;
public:
  CTestRegexDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_REGEXDEMOGEX_DIALOG };
    CString m_text;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

