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
  unsigned int      m_maxQueueSize;
  double            m_currentSum;
public:
  RollingAverageQueue(unsigned int maxQueueSize = 40) : m_maxQueueSize(maxQueueSize) {
    m_currentSum = 0;
  };
  void add(double n);
  void decrementMaxSize(unsigned int amount);
  double getCurrentAverage() const;
  unsigned int getCurrentSize() const {
    return (int)m_queue.size();
  }
  unsigned int getMaxSize() const {
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
  const unsigned int   m_updateRate;
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
	CProgressDlg(CWnd* pParent, Thread &thread, InteractiveRunnable &jobToDo, unsigned int updateRate);

	//{{AFX_DATA(CProgressDlg)
	enum { IDD = _IDD_PROGRESSDIALOG };
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CProgressDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer( UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonSuspend();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
