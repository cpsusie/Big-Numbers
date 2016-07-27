#pragma once

class ErrorsDlg : public CDialog {
public:
    ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd* pParent = NULL);

    //{{AFX_DATA(ErrorsDlg)
    enum { IDD = IDD_DIALOGERRORS };
    UINT    m_maxErrorCount;
    UINT    m_cascadeCount;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(ErrorsDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    HACCEL m_accelTable;

    //{{AFX_MSG(ErrorsDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoErrorCascadeCount();
    afx_msg void OnGotoMaxErrorCount();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
