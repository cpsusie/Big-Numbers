#pragma once

#include <Thread.h>

class CTestbinDlg;

class CounterThread : public Thread {
private:
  CTestbinDlg &m_dlg;
  UINT64       m_counter;
public:
  CounterThread(CTestbinDlg &dlg) : m_dlg(dlg) { m_counter = 0; }
  UINT run();
  inline UINT64 getCounter() const       { return m_counter;  }
  inline void   setCounter(UINT64 value) { m_counter = value; }
  void step()  { m_counter++;   }
  void reset() { m_counter = 0; }
};


class CTestbinDlg : public CDialog {
private:
  HICON          m_hIcon;
  HACCEL         m_accelTable;
  CString        m_hexString;
  bool           m_editMode;
  CSize          m_staticTextWinSize;
public:
  CTestbinDlg(CWnd *pParent = NULL);
  bool           m_timerIsRunning;
  UINT           m_timerInterval;
  CounterThread *m_counterThread;
  inline void showCounter() {
    showCounter(m_counterThread->getCounter());
  }
  inline UINT64 hexStringToInt() const {
    UINT64 v;
    _stscanf((LPCTSTR)m_hexString, _T("%I64X"), &v);
    return v;
  }
  void showCounter(UINT64 n);
  void startTimer();
  void stopTimer();
  void ajourEnabling();
  void enterEditMode();
  void leaveEditMode();
  enum { IDD = IDD_TESTBIN_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL PreTranslateMessage(MSG *pMsg);

  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnStartButton();
  afx_msg void OnStopButton();
  afx_msg void OnStepButton();
  afx_msg void OnResetButton();
  afx_msg void OnEditButton();
  afx_msg void OnFileExit();
  afx_msg void OnViewTimer();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnClose();
  DECLARE_MESSAGE_MAP()
};
