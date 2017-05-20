#pragma once

#include <Thread.h>

class CTestbinDlg;

class CounterThread : public Thread {
private:
  CTestbinDlg     &m_dlg;
  UINT64 m_counter;
public:
  CounterThread(CTestbinDlg &dlg) : m_dlg(dlg) { m_counter = 0; }
  UINT run();
  UINT64 getCounter() const { return m_counter; }
  void step() { m_counter++; }
  void reset() { m_counter = 0; }
};


class CTestbinDlg : public CDialog {
private:
    HICON          m_hIcon;
    HACCEL         m_accelTable;
public:
    CTestbinDlg(CWnd *pParent = NULL);
    bool           m_timerIsRunning;
    UINT   m_timerInterval;
    CounterThread *m_counterThread;
    void showCounter();
    void startTimer();
    void stopTimer();

    enum { IDD = IDD_TESTBIN_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL PreTranslateMessage(MSG *pMsg);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnStartButton();
    afx_msg void OnStopbutton();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileExit();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnViewTimer();
    afx_msg void OnStepbutton();
    afx_msg void OnResetbutton();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
};

