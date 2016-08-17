#pragma once

#include <MFCUtil/CoordinateSystem/CoordinateSystem.h>
#include "DebugThread.h"

class DynamicTargetFunction : public RemesTargetFunction {
private:
  String                  m_name;
  NumberInterval<BigReal> m_interval;
  int                     m_digits;
public:
  void setName(const String &name) {
    m_name = name;
  }
  String getName() const {
    return m_name;
  }
  void setInterval(double from, double to);
  const NumberInterval<BigReal> &getInterval() const {
    return m_interval;
  }
  void setDigits(int digits) {
    m_digits = digits;
  }
  int getDigits() const {
    return m_digits;
  }
  BigReal operator()(const BigReal &x);
};

typedef enum {
  DBGMENU_EMPTY
 ,DBGMENU_IDLE
 ,DBGMENU_RUNNING
 ,DBGMENU_PAUSED
} DebugMenuState;

class CoefWindowData {
private:
  void initData(const Remes &r) {
    m_M = r.getM(); m_K = r.getK(); m_coefStrings = r.getCoefficientStringArray();
  }
public:
  int m_M, m_K;
  StringArray m_coefStrings;
  CoefWindowData() {}
  CoefWindowData(const Remes &r) {
    initData(r);
  }
  CoefWindowData &operator=(const Remes &r) {
    initData(r);
    return *this;
  }
};

class CIRemesDlg : public CDialog, public PropertyChangeListener {
private:
  HACCEL                  m_accelTable;
  HICON                   m_hIcon;
  SimpleLayoutManager     m_layoutManager;
  Semaphore               m_gate;
  CCoordinateSystem       m_coorSystem;
  DynamicTargetFunction   m_targetFunction;
  Remes                  *m_remes;
  DebugThread            *m_debugThread;
  CoefWindowData          m_coefWinData;
  ExtremaStringArray      m_extrStrArray, m_extrStrArrayOld;
  String                  m_searchEString;
  String                  m_warning, m_error;
  DebugMenuState          m_dbgMenuState;

  void startThread(bool singleStep);
  void createThread();
  void destroyThread();
  String getThreadStateName() const;
  void showError(const Exception &e);
  inline bool hasDebugThread() const {
    return m_debugThread != NULL;
  }
  inline bool isThreadPaused() const {
    return hasDebugThread() && !m_debugThread->isRunning() && !m_debugThread->isTerminated();
  }
  inline bool isThreadRunning() const {
    return hasDebugThread() && m_debugThread->isRunning();
  }
  inline bool isThreadTerminated() const {
    return hasDebugThread() && m_debugThread->isTerminated();
  }
  void ajourDialogItems();
  void setDebugMenuState(DebugMenuState menuState);
  void enableFieldList(const int *ids, int n, bool enabled);
  void showThreadState();
  void showState(RemesState state);
  void showWarning(const String &str);
  void showCoefWindowData(const CoefWindowData &data);
  void showExtremaStringArray();
  void showSearchE(const String &s);
  void clearErrorPlot();
  bool createErrorPlot(const Remes &r);
  void showErrorPlot();
  int  getErrorPlotXPixelCount() const;
  void updateErrorPlotXRange();
  int  getLastErrorPlotKey();
  inline DoubleInterval getXRange() const {
    return DoubleInterval(m_xFrom, m_xTo);
  }
public:
  CIRemesDlg(CWnd *pParent = NULL);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void handleRemesProperty(const Remes &r, int id, const void *oldValue, const void *newValue);

	enum { IDD = IDD_IREMES_DIALOG };

public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  virtual BOOL    OnInitDialog();
  afx_msg void    OnPaint();
	afx_msg void    OnSize(UINT nType, int cx, int cy);
  virtual void    OnOK();
  virtual void    OnCancel();
  afx_msg void    OnClose();
  afx_msg void    OnFileExit();
  afx_msg void    OnViewGrid();
	afx_msg void    OnRunGo();
	afx_msg void    OnRunDebug();
	afx_msg void    OnRunContinue();
	afx_msg void    OnRunRestart();
	afx_msg void    OnRunStop();
  afx_msg void    OnRunBreak();
	afx_msg void    OnRunSingleIteration();
	afx_msg void    OnRunSingleSubIteration();
	afx_msg void    OnGotoInterval();
	afx_msg void    OnGotoM();
	afx_msg void    OnGotoK();
	afx_msg void    OnGotoDigits();
  afx_msg void    OnHelpAboutIRemes();
  afx_msg LRESULT OnMsgThrRunStateChanged(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgThrTerminatedChanged(   WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgThrErrorChanged(        WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgStateChanged(           WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgCoefficientsChanged(    WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgSearchEIterationChanged(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgExtremaCountChanged(    WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgMaxErrorChanged(        WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgWarningChanged(         WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()

private:
  CString m_name;
	UINT	  m_K;
	UINT	  m_M;
	double	m_xFrom;
	double	m_xTo;
	BOOL	  m_relativeError;
	UINT	  m_digits;
};
