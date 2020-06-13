#pragma once

#include <MFCUtil/CoordinateSystem/CoordinateSystem.h>
#include <MFCUtil/DialogWithDynamicLayout.h>
#include "Debugger.h"
#include "DebugInfo.h"
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

class CIRemesDlg : public CDialogWithDynamicLayout, public PropertyChangeListener {
  friend class ErrorPlotter;
private:
  HACCEL                  m_accelTable;
  HICON                   m_hIcon;
  RunMenuState            m_runMenuState;
  CListBoxDiffMarks       m_coefListBox;
  CCoordinateSystem       m_coorSystemError, m_coorSystemSpline;
  DynamicTargetFunction   m_targetFunction;
  bool                    m_reduceToInterpolate;
  ExtremaStringArray      m_extrStrArrayOld;
  Remes                  *m_remes;
  bool                    m_allowRemesProperties;
  DebugInfo               m_debugInfo;
  int                     m_subM, m_subK;
  Debugger               *m_debugger;
  DebuggerState           m_debuggerState;
  ErrorPlotter           *m_errorPlotter;
  bool                    m_errorPlotTimerRunning;
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
  void startErrorPlotter();
  void stopErrorPlotter();
  bool hasErrorPlotter() const {
    return m_errorPlotter != NULL;
  }
  void deallocateAll();
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
    return hasDebugger() && (m_debugger->getState() == DEBUGGER_PAUSED);
  }
  inline bool isDebuggerRunning() const {
    return hasDebugger() && (m_debugger->getState() == DEBUGGER_RUNNING);
  }
  inline bool isDebuggerTerminated() const {
    return hasDebugger() && (m_debugger->getState() == DEBUGGER_TERMINATED);
  }
  inline bool hasSolution() const {
    return isDebuggerTerminated() && (m_remes->getState() == REMES_SUCCEEDED);
  }
  void ajourDialogItems(DebuggerState state);
  void setRunMenuState(RunMenuState menuState);
  void enableFieldList(const int *ids, int n, bool enabled);
  void showDebuggerState(DebuggerState state);
  void showRemesState(const String &str);
  void showWarning(const String &str);
  void showCoefWindowData(const CoefWindowData &data);
  void showExtremaStringArray(const ExtremaStringArray &a);
  void showSearchEString(const String &s);
  void clearErrorPointArray();
  void createErrorPointArray();
  void showErrorPointArray();
  void setErrorFunctionVisible(bool visible);
  void removeErrorPlot();
  void startErrorPlotTimer();
  void stopErrorPlotTimer();
  void setSplineVisible(bool visible);
  bool isShowErrorFunctionChecked();
  bool isShowSplineChecked();
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
  afx_msg void    OnTimer(UINT_PTR nIDEvent);
  afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
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
  afx_msg void    OnRunStopDebugging();
  afx_msg void    OnRunBreak();
  afx_msg void    OnRunSingleIteration();
  afx_msg void    OnRunSingleSubIteration();
  afx_msg void    OnRunReduceToInterpolate();
  afx_msg void    OnHelpAboutIRemes();
  afx_msg void    OnEnKillfocusEditmFrom();
  afx_msg void    OnEnKillfocusEditkFrom();
  afx_msg void    OnEnKillfocusEditmTo();
  afx_msg void    OnEnKillfocusEditkTo();
  afx_msg void    OnEnUpdateEditkTo();
  afx_msg void    OnEnUpdateEditmTo();
  afx_msg LRESULT OnMsgDebuggerStateChanged(   WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgRemesStateChanged(      WPARAM wp, LPARAM lp);
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
