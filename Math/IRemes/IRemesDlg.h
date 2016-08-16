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

class ExtremaWindowData {
private:
  void initStringArray(const Remes &r) {
    m_extremaStrings = r.getExtremaStringArray();
  }
public:
  StringArray m_extremaStrings;
  ExtremaWindowData() {}
  ExtremaWindowData(const Remes &r) {
    initStringArray(r);
  }
  ExtremaWindowData &operator=(const Remes &r) {
    initStringArray(r);
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
  ExtremaWindowData       m_extrWinData;
  String                  m_searchEString;
  String                  m_warning, m_error;

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
  void ajourDialogItems();
  void enableFieldList(const int *ids, int n, bool enabled);
  void showState(RemesState state);
  void showWarning(const String &str);
  void showCoefWindowData(   const CoefWindowData    &data);
  void showExtremaWindowData(const ExtremaWindowData &data);
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
	afx_msg void    OnRunGo();
	afx_msg void    OnRunDebug();
	afx_msg void    OnRunSingleIteration();
	afx_msg void    OnRunSingleSubIteration();
	afx_msg void    OnRunStop();
	afx_msg void    OnGotoInterval();
	afx_msg void    OnGotoM();
	afx_msg void    OnGotoK();
	afx_msg void    OnGotoDigits();
  afx_msg void    OnHelpAboutIRemes();
  afx_msg LRESULT OnMsgRunStateChanged(      WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgStateChanged(         WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgMainIterationChanged( WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgSearchEItChanged(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgExtremaCountChanged(  WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgWarningChanged(       WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgShowErrorPlot(        WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgShowError(            WPARAM wp, LPARAM lp);
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
