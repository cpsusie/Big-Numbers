#pragma once

#include <afxcmn.h>         // MFC support for Windows Common Controls
#include <MFCUtil/resource.h>
#include "MFCUtil/InteractiveRunnableWrapper.h"
#include "MFCUtil/ProgressWithPctCtrl.h"

class CProgressDlg : public CDialog {
private:
  HACCEL                      m_accelTable;
  InteractiveRunnableWrapper &m_rw;
  const UINT                  m_updateRate;
  const int                   m_supportedFeatures;
  bool                        m_timerRunning;
  USHORT                      m_jobCount;
  String                      m_timeElapsedLabel, m_timeRemaingLabel;
  CProgressWithPctCtrl        m_newProgressCtrl;

  CButton       *getCancelButton();
  CButton       *getSuspendButton();
  CStatic       *getStaticTimeEstimate();
  CStatic       *getStaticProgressMessage(UINT index);
  CProgressCtrl *getSubProgressCtrl(      UINT index);
  int            setVisibleJobs(          UINT count);
  void startTimer();
  void stopTimer();
  void setWaitCursor(bool on = true);
  int moveControlsBelowUp(CWnd *win, int dh=0);
  void resumeJob();
  void suspendJob();
public:
  CProgressDlg(CWnd *pParent, InteractiveRunnableWrapper &rw, UINT updateRate);

  enum { IDD = _IDD_PROGRESSDIALOG };

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg void OnTimer( UINT_PTR nIDEvent);
  afx_msg void OnClose();
  afx_msg void OnButtonSuspend();
  afx_msg void OnOk();
  DECLARE_MESSAGE_MAP()
};
