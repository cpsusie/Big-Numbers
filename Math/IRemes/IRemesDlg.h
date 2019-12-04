#pragma once

#include <MFCUtil/CoordinateSystem/CoordinateSystem.h>
#include "DebugThread.h"
#include "ListBoxDiffMarks.h"

class DynamicTargetFunction : public RemesTargetFunction {
private:
  String          m_name;
  BigRealInterval m_domain;
  UINT            m_digits;
public:
  void setName(const String &name) {
    m_name = name;
  }
  String getName() const {
    return m_name;
  }
  void setDomain(double from, double to);
  const BigRealInterval &getDomain() const {
    return m_domain;
  }
  String         getDefaultName() const;
  DoubleInterval getDefaultDomain() const;
  void setDigits(UINT digits) {
    m_digits = digits;
  }
  UINT getDigits() const {
    return m_digits;
  }
  BigReal operator()(const BigReal &x);
};

typedef enum {
  RUNMENU_EMPTY
 ,RUNMENU_IDLE
 ,RUNMENU_RUNNING
 ,RUNMENU_PAUSED
} RunMenuState;

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

class MonitorVariables {
private:
  mutable FastSemaphore   m_lock, m_approxLock;
  CoefWindowData          m_coefWinData;
  ExtremaStringArray      m_extrStrArray;
  String                  m_searchEString, m_stateString, m_warning, m_error;
  RationalFunction        m_lastApprox;
  double                  m_maxError;
  Point2DArray            m_errorPointArray;
  int                     m_coefVectorIndexForPointArray;
public:
  MonitorVariables() : m_coefVectorIndexForPointArray(-1) {
  }
  void setCoefWinData(const CoefWindowData &src) {
    m_lock.wait();
    m_coefWinData = src;
    m_lock.notify();
  }
  void getCoefWinData(CoefWindowData &dst) const {
    m_lock.wait();
    dst = m_coefWinData;
    m_lock.notify();
  }

  void setExtremaStringArray(const ExtremaStringArray &src) {
    m_lock.wait();
    m_extrStrArray = src;
    m_lock.notify();
  }
  void getExtremaStringArray(ExtremaStringArray &dst) const {
    m_lock.wait();
    dst = m_extrStrArray;
    m_lock.notify();
  }
  void setSearchEString(const String &src) {
    m_lock.wait();
    m_searchEString = src;
    m_lock.notify();
  }
  void getSearchEString(String &dst) const {
    m_lock.wait();
    dst = m_searchEString;
    m_lock.notify();
  }
  void setStateString(const String &src) {
    m_lock.wait();
    m_stateString = src;
    m_lock.notify();
  }
  void getStateString(String &dst) const {
    m_lock.wait();
    dst = m_stateString;
    m_lock.notify();
  }
  void setErrorString(const String &src) {
    m_lock.wait();
    m_error = src;
    m_lock.notify();
  }
  void getErrorString(String &dst) const {
    m_lock.wait();
    dst = m_error;
    m_lock.notify();
  }
  void setWarningString(const String &src) {
    m_lock.wait();
    m_warning = src;
    m_lock.notify();
  }
  void getWarningString(String &dst) const {
    m_lock.wait();
    dst = m_warning;
    m_lock.notify();
  }
  void setApproximation(const RationalFunction &src) {
    m_approxLock.wait();
    m_lastApprox = src;
    m_approxLock.notify();
  }
  void getApproximation(RationalFunction &dst) const {
    m_approxLock.wait();
    dst = m_lastApprox;
    m_approxLock.notify();
  }
  bool isApproxEmpty() const {
    m_approxLock.wait();
    const bool b = m_lastApprox.isEmpty();
    m_approxLock.notify();
    return b;
  }
  void setMaxError(double maxError) {
    m_lock.wait();
    m_maxError = maxError;
    m_lock.notify();
  }
  double getMaxError() const {
    m_lock.wait();
    const double result = m_maxError;
    m_lock.notify();
    return result;
  }
  void setPointArray(const Point2DArray &a, int key) {
    m_lock.wait();
    m_errorPointArray = a;
    m_coefVectorIndexForPointArray = key;
    m_lock.notify();
  }
  void getPointArray(Point2DArray &dst) {
    m_lock.wait();
    dst = m_errorPointArray;
    m_lock.notify();
  }
  int getPointArrayKey() {
    return m_coefVectorIndexForPointArray;
  }
  void clearPointArray() {
    m_lock.wait();
    m_errorPointArray.clear();
    m_coefVectorIndexForPointArray = -1;
    m_lock.notify();
  }
};

class CIRemesDlg : public CDialog, public PropertyChangeListener {
  friend class ErrorPlotCalculator;
private:
  HACCEL                  m_accelTable;
  HICON                   m_hIcon;
  SimpleLayoutManager     m_layoutManager;
  RunMenuState            m_runMenuState;
  CListBoxDiffMarks       m_coefListBox;
  CCoordinateSystem       m_coorSystemError, m_coorSystemSpline;
  DynamicTargetFunction   m_targetFunction;
  bool                    m_reduceToInterpolate;
  ExtremaStringArray      m_extrStrArrayOld;
  Remes                  *m_remes;
  MonitorVariables        m_debugInfo;
  int                     m_subM, m_subK;
  Debugger               *m_debugger;
  ErrorPlotCalculator    *m_errorPlotCalculator;
  CString                 m_name;
  UINT	                  m_M;
  UINT	                  m_K;
  UINT                    m_MTo;
  UINT                    m_KTo;
  UINT                    m_maxMKSum;
  double                  m_xFrom;
  double                  m_xTo;
  UINT	                  m_digits;
  VisiblePrecisions       m_visiblePrecisions;
  UINT                    m_maxSearchEIterations;
  BOOL	                  m_relativeError;
  BOOL                    m_skipExisting;

  bool validateInput();
  void startDebugger(bool singleStep);
  void createDebugger();
  void destroyDebugger();
  void startErrorPlotCalculator();
  void stopErrorPlotCalculator();
  bool hasErrorPlotCalculator() const {
    return m_errorPlotCalculator != NULL;
  }
  void setSubMK(int subM, int subK);
  void adjustMaxMKSum();
  String getThreadStateName() const;
  inline bool hasRemes() const {
    return m_remes != NULL;
  }
  inline bool hasDebugger() const {
    return m_debugger != NULL;
  }
  inline bool isDebuggerPaused() const {
    return hasDebugger() && !m_debugger->isRunning() && !m_debugger->isTerminated();
  }
  inline bool isDebuggerRunning() const {
    return hasDebugger() && m_debugger->isRunning();
  }
  inline bool isDebuggerTerminated() const {
    return hasDebugger() && m_debugger->isTerminated();
  }
  inline bool hasSolution() const {
    return isDebuggerTerminated() && (m_remes->getState() == REMES_SUCCEEDED);
  }
  void ajourDialogItems();
  void setRunMenuState(RunMenuState menuState);
  void enableFieldList(const int *ids, int n, bool enabled);
  void showThreadState();
  void showState(const String &str);
  void showWarning(const String &str);
  void showCoefWindowData(const CoefWindowData &data);
  void showExtremaStringArray(const ExtremaStringArray &a);
  void showSearchE(const String &s);
  void clearErrorPointArray();
  void createErrorPointArray();
  void showErrorPointArray();
  void setErrorFunctionVisible(bool visible);
  void removeErrorPlot();
  void setSplineVisible(bool visible);
  bool isErrorFunctionVisible();
  bool isSplineVisible();
  int  getErrorPlotXPixelCount() const;
  void updateErrorPlotXRange();
  inline DoubleInterval getXRange() const {
    return DoubleInterval(m_xFrom, m_xTo);
  }
public:
  CIRemesDlg(CWnd *pParent = NULL);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void handleRemesProperty(const Remes &r, int id, const void *oldValue, const void *newValue);

  enum { IDD = IDR_MAINFRAME };

public:
  virtual BOOL    PreTranslateMessage(MSG *pMsg);
  virtual void    DoDataExchange(CDataExchange *pDX);
  virtual BOOL    OnInitDialog();
  virtual void    OnOK();
  virtual void    OnCancel();
  afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void    OnSize(UINT nType, int cx, int cy);
  afx_msg void    OnClose();
  afx_msg void    OnPaint();
  afx_msg void    OnFileShowMaxErrors();
  afx_msg void    OnFileExit();
  afx_msg void    OnViewGrid();
  afx_msg void    OnViewShowErrorFunction();
  afx_msg void    OnViewShowSpline();
  afx_msg void    OnViewDisplayedPrecision();
  afx_msg void    OnGenerateCcodeD64();
  afx_msg void    OnGenerateCcodeD80();
  afx_msg void    OnGenerateJavacodeD64();
  afx_msg void    OnRunGo();
  afx_msg void    OnRunF5();
  afx_msg void    OnRunDebug();
  afx_msg void    OnRunContinue();
  afx_msg void    OnRunRestart();
  afx_msg void    OnRunStop();
  afx_msg void    OnRunBreak();
  afx_msg void    OnRunSingleIteration();
  afx_msg void    OnRunSingleSubIteration();
  afx_msg void    OnRunReduceToInterpolate();
  afx_msg void    OnGotoDomain();
  afx_msg void    OnGotoM();
  afx_msg void    OnGotoK();
  afx_msg void    OnGotoDigits();
  afx_msg void    OnGotoMaxSearchEIterations();
  afx_msg void    OnHelpAboutIRemes();
  afx_msg void    OnEnKillfocusEditmFrom();
  afx_msg void    OnEnKillfocusEditkFrom();
  afx_msg void    OnEnKillfocusEditmTo();
  afx_msg void    OnEnKillfocusEditkTo();
  afx_msg void    OnEnUpdateEditkTo();
  afx_msg void    OnEnUpdateEditmTo();
  afx_msg LRESULT OnMsgThrRunStateChanged(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgThrTerminatedChanged(   WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgThrErrorChanged(        WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgStateChanged(           WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgCoefficientsChanged(    WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgSearchEIterationChanged(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgExtremaCountChanged(    WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgMaxErrorChanged(        WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgShowErrorFunction(      WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgClearErrorFunction(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgUpdateInterpolation(    WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgWarningChanged(         WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgApproximationChanged(   WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgErrorPointArrayChanged( WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};
