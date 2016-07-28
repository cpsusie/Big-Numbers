#pragma once

class ErrorsDlg : public CDialog {
public:
    ErrorsDlg(int maxErrorCount, int cascadeCount, CWnd *pParent = NULL);

    enum { IDD = IDD_DIALOGERRORS };
    UINT    m_maxErrorCount;
    UINT    m_cascadeCount;


    public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    HACCEL m_accelTable;

    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoErrorCascadeCount();
    afx_msg void OnGotoMaxErrorCount();
    DECLARE_MESSAGE_MAP()
};

