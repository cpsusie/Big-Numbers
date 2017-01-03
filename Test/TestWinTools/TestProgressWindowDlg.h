#pragma once

class CTestProgressWindowDlg : public CDialog {
private:
public:
    CTestProgressWindowDlg(CWnd* pParent = NULL);

    enum { IDD = IDD_TESTPROGRESSWINDOW_DIALOG };
    BOOL    m_hasMessageBox;
    BOOL    m_hasTimeEstimate;
    BOOL    m_autoCorrelateEstimate;
    BOOL    m_interruptable;
    BOOL    m_hasProgressBar;
    BOOL    m_showPercent;
    BOOL    m_hasSubProgressBar;
    BOOL    m_suspendable;
    UINT    m_delayMSec;
    CString m_title;
    UINT    m_jobTime;
    UINT    m_updateRate;
    UINT    m_jobCount;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonStartJob();
    DECLARE_MESSAGE_MAP()
};
