#pragma once

#include "PartyMaker.h"
#include "partymakerDlg.h"
#include "CheckFilesThread.h"

class CCheckFilesDlg : public CDialog {
public:
  CCheckFilesDlg(CWnd *pParent = NULL);
  CCheckFilesThread  *m_worker;
  HACCEL             m_accelTable;
  bool               m_workerIsRunning;

  void stopTimer();
  void startTimer();
  void startWorker();
  void stopWorker();

  enum { IDD = IDD_CHECKFILESDIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  HICON m_hIcon;

  virtual BOOL OnInitDialog();
  afx_msg void OnCheckstatus();
  afx_msg void OnTimer(UINT nIDEvent);
  virtual void OnCancel();
  afx_msg void OnShowErrors();
  DECLARE_MESSAGE_MAP()
};

