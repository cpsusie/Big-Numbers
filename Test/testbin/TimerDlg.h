#pragma once

class CTimerDlg : public CDialog {
public:
    CTimerDlg(UINT timerInterval, CWnd *pParent = NULL);

    enum { IDD = IDD_TIMER_DIALOG };
    UINT    m_timerInterval;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};
