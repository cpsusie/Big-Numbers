#pragma once

#include <LayoutManager.h>

class ShowStateDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  CDialog            *m_mainDialog;
  void ajourState();
public:
    ShowStateDlg(CDialog *mainDialog, CWnd* pParent = NULL);

    //{{AFX_DATA(ShowStateDlg)
    enum { IDD = IDD_DIALOGSHOWSTATE };
    CString m_data;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(ShowStateDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(ShowStateDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnShowStateUpdate(WPARAM wp, LPARAM lp);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
