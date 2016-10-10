#pragma once

#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "resource.h"
#include <Thread.h>
#include <Date.h>
#include <QueueList.h>
#include "InteractiveRunnable.h"
#include "ProgressWithPctCtrl.h"

class RollingAverageQueue {
private:
  QueueList<double> m_queue;
  UINT              m_maxQueueSize;
  double            m_currentSum;
public:
  RollingAverageQueue(UINT maxQueueSize = 40) : m_maxQueueSize(maxQueueSize) {
    m_currentSum = 0;
  };
  void add(double n);
  void decrementMaxSize(UINT amount);
  double getCurrentAverage() const;
  UINT getCurrentSize() const {
    return (int)m_queue.size();
  }
  UINT getMaxSize() const {
    return m_maxQueueSize;
  }
  bool isFull() const {
    return getCurrentSize() == m_maxQueueSize;
  }
  bool isEmpty() {
    return m_queue.isEmpty();
  }
};

class CProgressDlg : public CDialog {
private:
  Thread              &m_thread;
  InteractiveRunnable &m_jobToDo;
  const UINT           m_updateRate;
  const int            m_supportedFeatures;
  RollingAverageQueue  m_rollingAverage;
  String               m_timeElapsedLabel, m_timeRemaingLabel;
  CProgressWithPctCtrl m_newProgressCtrl;
  HACCEL               m_accelTable;

  void substituteProgressControl();
  CButton       *getCancelButton();
  CButton       *getSuspendButton();
  CStatic       *getStaticTimeEstimate();
  CStatic       *getStaticProgressMessage();
  CProgressCtrl *getProgressCtrl();
  CProgressCtrl *getSubProgressCtrl();
  void startTimer();
  void stopTimer();
  void setWaitCursor(bool on = true);
  double getEstimatedSecondsLeft(short progress, short subPercent);
  int moveControlsBelowUp(CWnd *ctrl);
  void resumeJob();
  void suspendJob();
public:
	CProgressDlg(CWnd* pParent, Thread &thread, InteractiveRunnable &jobToDo, UINT updateRate);

	enum { IDD = _IDD_PROGRESSDIALOG };

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer( UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonSuspend();
	afx_msg void OnOk();
	DECLARE_MESSAGE_MAP()
};
