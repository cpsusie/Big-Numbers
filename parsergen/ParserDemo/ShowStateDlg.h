#pragma once

#include <MFCUtil/LayoutManager.h>

class ShowStateDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  CDialog            *m_mainDialog;
  void ajourState();
public:
    ShowStateDlg(CDialog *mainDialog, CWnd *pParent = NULL);

    enum { IDD = IDD_DIALOGSHOWSTATE };

    CString m_data;

    public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnShowStateUpdate(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};

