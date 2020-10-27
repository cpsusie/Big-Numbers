#pragma once

#include <Date.h>
#include "CPUHeaterThread.h"
#include "Options.h"

class CHeatCPUDlg : public CDialog {
private:
  HICON           m_hIcon;
  CPUHeaterThread m_CPUHeaterThread[4];
  Options         m_options;
  bool            m_timerIsRunning;
  Timestamp       m_lastTimerEvent;
  int             m_paintCount;

  CSliderCtrl *getSliderCtrl() {
    return (CSliderCtrl*)GetDlgItem(IDC_SLIDERCPULOAD);
  }

  void startTimer();
  void stopTimer();
  void showCounterWindows(bool show);
  void addStatusIcon();
  void deleteStatusIcon();
  void setCPULoad(int pct);  // pct = [0..100]
  void refreshAfterShutdown();
  void hideWindow();
  void showCounters(bool show);
  String getToolTipText() const;
public:
    CHeatCPUDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_HEATCPU_DIALOG };

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnClose();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileExit();
    afx_msg void OnOptionsLaunchAtStartup();
    afx_msg void OnOptionsShowCounters();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    DECLARE_MESSAGE_MAP()
};

